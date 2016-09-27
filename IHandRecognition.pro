#-------------------------------------------------
#
# Project created by QtCreator 2012-09-17T08:17:46
#
#-------------------------------------------------

 QT += core gui widgets

TARGET = IHandRecognition
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    IplImageToQImage.cpp

HEADERS  += widget.h \
    IplImageToQImage.h

FORMS    += widget.ui
QT       += network

INCLUDEPATH += C:\Qt\opencv3.0_use\include
INCLUDEPATH += C:\Qt\opencv3.0_use\include\opencv
INCLUDEPATH += C:\Qt\opencv3.0_use\include\opencv2
LIBS += C:\Qt\opencv3.0_use\lib\libopencv_*.a

OTHER_FILES += \
    haarcascade_frontalface_alt2.xml \
    haarcascade_frontalface_alt2.xml
