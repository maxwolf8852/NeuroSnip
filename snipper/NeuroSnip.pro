QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a

RC_ICONS = Nico.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    grpc_cli/cli_g.grpc.pb.cc \
    grpc_cli/cli_g.pb.cc \
    main.cpp \
    mainwindow.cpp \
    recognizer.cpp \
    screenshot.cpp \
    translator.cpp

HEADERS += \
    alphabet.h \
    grpc_cli/cli_g.grpc.pb.h \
    grpc_cli/cli_g.pb.h \
    mainwindow.h \
    recognizer.h \
    screenshot.h \
    translator.h

FORMS += \
    mainwindow.ui

CONFIG += static

QMAKE_CXXFLAGS += /MT
QMAKE_CFLAGS_RELEASE = -MT
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -MT

INCLUDEPATH +=

CONFIG(release, debug|release) {

fls = $$files(F:\Programs\vcpkg\installed\x64-windows-static\lib\*.lib, true)
fls -= F:/Programs/vcpkg/installed/x64-windows-static/lib/zlib.lib
fls += F:/Programs/vcpkg/installed/x64-windows/lib/zlib.lib
fls += Crypt32.lib
fls += Iphlpapi.lib
message($$fls)
LIBS += $$fls
#LIBS += F:\Programs\vcpkg\installed\x64-windows-static\lib\*.lib
#LIBS += "F:\Windows Kits\10\Lib\10.0.19041.0\um\x64\Ws2_32.lib" "F:\Windows Kits\10\Lib\10.0.19041.0\um\x64\Advapi32.lib"
}

INCLUDEPATH += F:\Programs\vcpkg\installed\x64-windows-static\include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
