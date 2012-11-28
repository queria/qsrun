#-------------------------------------------------
#
# Project created by QtCreator 2011-11-09T03:59:23
#
#-------------------------------------------------

QT       += core gui dbus script

# comment this out to get output on cli
DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = qsrun
TEMPLATE = app


SOURCES += src/main.cpp\
        src/runbar.cpp \
    src/apphinter.cpp \
    src/calculator.cpp \
    src/dbussingleinstance.cpp \
    src/settings.cpp \
    src/folderwatch.cpp

HEADERS  += src/runbar.h \
    src/apphinter.h \
    src/calculator.h \
    src/dbussingleinstance.h \
    src/settings.h \
    src/folderwatch.h

RESOURCES += \
    icons.qrc

MY_BUILD_DIR = $$PWD/build
DESTDIR = $$MY_BUILD_DIR
MOC_DIR = $$MY_BUILD_DIR/moc/
OBJECTS_DIR = $$MY_BUILD_DIR/obj/
RCC_DIR = $$MY_BUILD_DIR/rcc/
UI_DIR = $$MY_BUILD_DIR/ui/

