QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LT/LibreTranslate.cpp \
    grpc_cli/cli_g.grpc.pb.cc \
    grpc_cli/cli_g.pb.cc \
    main.cpp \
    mainwindow.cpp \
    recognizer.cpp \
    screenshot.cpp \
    translator.cpp

HEADERS += \
    LT/LibreTranslate.h \
    alphabet.h \
    grpc_cli/cli_g.grpc.pb.h \
    grpc_cli/cli_g.pb.h \
    mainwindow.h \
    recognizer.h \
    screenshot.h \
    translator.h

FORMS += \
    mainwindow.ui



CONFIG(release, debug|release) {
LIBS += F:\Programs\vcpkg\installed\x64-windows\lib\*.lib
LIBS += "F:\Windows Kits\10\Lib\10.0.19041.0\um\x64\Ws2_32.lib" "F:\Windows Kits\10\Lib\10.0.19041.0\um\x64\Advapi32.lib"
}
CONFIG(debug, debug|release) {
LIBS += F:\Programs\vcpkg\installed\x64-windows\debug\lib\*.lib
LIBS += "F:\Windows Kits\10\Lib\10.0.19041.0\um\x64\Ws2_32.lib" "F:\Windows Kits\10\Lib\10.0.19041.0\um\x64\Advapi32.lib"
}
INCLUDEPATH += F:\Programs\vcpkg\installed\x64-windows\include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
