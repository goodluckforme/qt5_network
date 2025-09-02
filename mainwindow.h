#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QNetworkInterface>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateNetworkStats();
    void onInterfaceChanged(int index);

private:
    Ui::MainWindow *ui;
    QTimer *m_timer;
    QMap<QString, qint64> m_prevBytesReceived;
    QMap<QString, qint64> m_prevBytesSent;
    QMap<QString, qint64> m_prevTotalBytes;
    QDateTime m_prevUpdateTime;

    QString formatBytes(qint64 bytes);
    QString formatSpeed(double bytesPerSecond);
    QPair<qint64, qint64> getInterfaceTraffic(const QString &interfaceName);
    void populateInterfaces();
};  
#endif // MAINWINDOW_H
