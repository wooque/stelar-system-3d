QT += core gui opengl widgets

INCLUDEPATH += src
LIBS += -lGLU
CONFIG += c++11

TARGET = solar-system-3d
TEMPLATE = app

SOURCES += src/NebeskoTelo.cpp \
    src/Tekstura.cpp \
    src/glaux.cpp \
    src/glwidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += src/NebeskoTelo.h \
    src/OpenGL_planete.h \
    src/Tekstura.h \
    src/glaux.h \
    src/glwidget.h \
    src/mainwindow.h
