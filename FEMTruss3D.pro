#-------------------------------------------------
#
# Project created by QtCreator 2017-07-11T22:01:18
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FEMTruss3D
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lGLU
LIBS += -L/usr/local/lib -lftgl

# Use OpenMP
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS *= -fopenmp
DEFINES += FEM_NUM_THREADS=8

QMAKE_CXXFLAGS_RELEASE += -w

INCLUDEPATH += ../library

# math and atlas
LIBS += -L../library/mth/lib/ -lmth
LIBS += -L/opt/atlas/lib/ -ltatlas

# dxflib
INCLUDEPATH += /opt/dxflib/include
LIBS += -L/opt/dxflib/lib -ldxflib


SOURCES += \
        main.cpp \
        femtruss3d.cpp \
    node3d.cpp \
    truss3delement.cpp \
    truss3d.cpp \
    material.cpp \
    arcball.cpp \
    graphicwindow.cpp \
    line3dbuffer.cpp \
    dxfreader.cpp \
    point3dbuffer.cpp
HEADERS += \
        femtruss3d.h \
    node3d.h \
    truss3delement.h \
    truss3d.h \
    material.h \
    arcball.h \
    graphicwindow.h \
    line3dbuffer.h \
    point3dbuffer.h \
    dxfreader.h

FORMS += \
        femtruss3d.ui

DISTFILES += \
    vertexshader.vert \
    fragmentshader.frag
