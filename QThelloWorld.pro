QT       += core gui widgets concurrent dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17  link_pkgconfig

TARGET = QThelloWorld
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
QMAKE_CXXFLAGS += -DLIGHTDM_DEBUG
QMAKE_LFLAGS += -lX11

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Link com liblightdm-qt5
LIBS += -llightdm-qt5-3
INCLUDEPATH += /usr/include/lightdm-qt5-3

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
