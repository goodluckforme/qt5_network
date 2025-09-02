#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("网络速度监控");

    // 初始化定时器，每1000毫秒更新一次
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateNetworkStats);

    // 填充网络接口列表
    populateInterfaces();

    // 初始化时间戳
    m_prevUpdateTime = QDateTime::currentDateTime();

    // 启动定时器
    m_timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateInterfaces()
{
    // 获取所有网络接口
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach (const QNetworkInterface &interface, interfaces)
    {
        ui->interfaceComboBox->addItem(interface.humanReadableName(), interface.name());

        QList<QNetworkAddressEntry> addressEntries = interface.addressEntries();
        if (!addressEntries.isEmpty())
        {
            QNetworkAddressEntry addressEntry = addressEntries.first();
            ui->ipLabel->setText(addressEntry.ip().toString());
        }

        // 获取初始字节数
        QPair<qint64, qint64> traffic = getInterfaceTraffic(interface.name());
        qint64 received = traffic.first;
        qint64 sent = traffic.second;
        m_prevBytesReceived[interface.name()] = received;
        m_prevBytesSent[interface.name()] = sent;
        m_prevTotalBytes[interface.name()] = received + sent;
    }

    connect(ui->interfaceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onInterfaceChanged);
}

void MainWindow::onInterfaceChanged(int index)
{
    if (index >= 0)
    {
        QString interfaceName = ui->interfaceComboBox->itemData(index).toString();
        QNetworkInterface interface = QNetworkInterface::interfaceFromName(interfaceName);

        QList<QNetworkAddressEntry> addressEntries = interface.addressEntries();
        if (!addressEntries.isEmpty())
        {
            QNetworkAddressEntry addressEntry = addressEntries.first();
            ui->ipLabel->setText(addressEntry.ip().toString());
        }

        // 重置统计值
        QPair<qint64, qint64> traffic = getInterfaceTraffic(interfaceName);
        qint64 received = traffic.first;
        qint64 sent = traffic.second;
        m_prevBytesReceived[interfaceName] = received;
        m_prevBytesSent[interfaceName] = sent;
        m_prevTotalBytes[interfaceName] = received + sent;
    }
}

QString MainWindow::formatBytes(qint64 bytes)
{
    if (bytes < 1024)
    {
        return QString("%1 B").arg(bytes);
    }
    else if (bytes < 1024 * 1024)
    {
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 2);
    }
    else if (bytes < 1024 * 1024 * 1024)
    {
        return QString("%1 MB").arg(bytes / (1024.0 * 1024), 0, 'f', 2);
    }
    else
    {
        return QString("%1 GB").arg(bytes / (1024.0 * 1024 * 1024), 0, 'f', 2);
    }
}

QString MainWindow::formatSpeed(double bytesPerSecond)
{
    if (bytesPerSecond < 1024)
    {
        return QString("%1 B/s").arg(bytesPerSecond, 0, 'f', 2);
    }
    else if (bytesPerSecond < 1024 * 1024)
    {
        return QString("%1 KB/s").arg(bytesPerSecond / 1024.0, 0, 'f', 2);
    }
    else
    {
        return QString("%1 MB/s").arg(bytesPerSecond / (1024.0 * 1024), 0, 'f', 2);
    }
}

void MainWindow::updateNetworkStats()
{

    printf("updateNetworkStats currentIndex： %d \n", ui->interfaceComboBox->currentIndex());
    if (ui->interfaceComboBox->currentIndex() < 0)
        return;

    QString interfaceName = ui->interfaceComboBox->currentData().toString();

    printf("updateNetworkStats interfaceName %s \n", interfaceName.toStdString().c_str());
    QNetworkInterface interface = QNetworkInterface::interfaceFromName(interfaceName);
    bool flag = !interface.isValid();
    printf("Flag value: %s\n", flag ? "true" : "false"); // 输出: Flag value: true

    if (flag)
    {
        return;
    }

    // 获取当前字节数
    QPair<qint64, qint64> traffic = getInterfaceTraffic(interfaceName);
    qint64 received = traffic.first;
    qint64 sent = traffic.second;
    qint64 total = received + sent;

    printf("Received: %lld, Sent: %lld, Total: %lld\n", received, sent, total);

    // 计算时间差（秒）
    QDateTime currentTime = QDateTime::currentDateTime();
    double elapsed = m_prevUpdateTime.secsTo(currentTime);

    if (elapsed <= 0)
    {
        m_prevUpdateTime = currentTime;
        return;
    }

    // 计算速度（字节/秒）
    double downloadSpeed = (received - m_prevBytesReceived[interfaceName]) / elapsed;
    double uploadSpeed = (sent - m_prevBytesSent[interfaceName]) / elapsed;
    double totalSpeed = (total - m_prevTotalBytes[interfaceName]) / elapsed;

    // 更新UI
    ui->downloadSpeedLabel->setText(formatSpeed(downloadSpeed));
    ui->uploadSpeedLabel->setText(formatSpeed(uploadSpeed));
    ui->totalSpeedLabel->setText(formatSpeed(totalSpeed));

    ui->totalDownloadLabel->setText(formatBytes(received));
    ui->totalUploadLabel->setText(formatBytes(sent));
    ui->totalDataLabel->setText(formatBytes(total));

    // 更新上一次的值
    m_prevBytesReceived[interfaceName] = received;
    m_prevBytesSent[interfaceName] = sent;
    m_prevTotalBytes[interfaceName] = total;
    m_prevUpdateTime = currentTime;
}

QPair<qint64, qint64> MainWindow::getInterfaceTraffic(const QString &interfaceName)
{
    printf("Looking for interface: %s\n", interfaceName.toStdString().c_str());

    QFile file("/proc/net/dev");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        // 跳过前两行标题
        in.readLine();
        in.readLine();

        while (!in.atEnd())
        {
            QString line = in.readLine();
            printf("Checking line: %s\n", line.toStdString().c_str());

            // 处理接口名称，可能带有冒号或其他特殊字符
            QString lineInterfaceName = line.section(':', 0, 0).trimmed();
            printf("Interface in line: %s\n", lineInterfaceName.toStdString().c_str());

            if (lineInterfaceName == interfaceName)
            {
                QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                printf("Parts size: %d\n", parts.size());

                if (parts.size() >= 10)
                {
                    qint64 received = parts[1].toLongLong();
                    qint64 sent = parts[9].toLongLong();
                    printf("Found traffic - Received: %lld, Sent: %lld\n", received, sent);
                    file.close();
                    return qMakePair(received, sent);
                }
            }
        }
        file.close();
    }
    else
    {
        printf("Failed to open /proc/net/dev\n");
    }

    printf("Interface not found or error reading data\n");
    return qMakePair(0LL, 0LL);
}
