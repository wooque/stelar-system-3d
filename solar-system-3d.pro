QT += core gui opengl widgets

INCLUDEPATH += src
LIBS += -lGLU
CONFIG += c++14

TARGET = solar-system-3d
TEMPLATE = app

SOURCES += src/NebeskoTelo.cpp \
    src/Tekstura.cpp \
    src/glwidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += src/NebeskoTelo.h \
    src/Tekstura.h \
    src/glwidget.h \
    src/mainwindow.h
