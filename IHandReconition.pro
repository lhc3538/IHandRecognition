#-------------------------------------------------
#
# Project created by QtCreator 2013-05-25T11:16:11
#
#-------------------------------------------------

QT       += core gui

CONFIG += warn_off

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HandGesture
TEMPLATE = app
QT       += network

SOURCES += main.cpp\
        handgesturedialog.cpp \
    SRC/GestrueInfo.cpp \
    SRC/AIGesture.cpp \
    IplImageToQImage.cpp

HEADERS  += handgesturedialog.h \
    SRC/GestureStruct.h \
    SRC/GestrueInfo.h \
    SRC/AIGesture.h \
    IplImageToQImage.h

FORMS    += handgesturedialog.ui

INCLUDEPATH += C:\Qt\opencv3.0_use\include
INCLUDEPATH += C:\Qt\opencv3.0_use\include\opencv
INCLUDEPATH += C:\Qt\opencv3.0_use\include\opencv2
LIBS += C:\Qt\opencv3.0_use\lib\libopencv_*.a

LIBS += -lpthread
