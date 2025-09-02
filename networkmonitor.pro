QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetworkMonitor
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# 安装路径设置
target.path = /usr/bin
INSTALLS += target

# 图标设置
icons.path = /usr/share/icons/hicolor/64x64/apps
icons.files += network-monitor.png
INSTALLS += icons

# .desktop文件
desktop.path = /usr/share/applications
desktop.files += network-monitor.desktop
INSTALLS += desktop
