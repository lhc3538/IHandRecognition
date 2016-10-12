QT       += core gui
QT       += network
CONFIG += warn_off

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IHandRecognition
TEMPLATE = app

SOURCES += main.cpp\
        handgesturedialog.cpp \
    SRC/GestrueInfo.cpp \
    SRC/AIGesture.cpp \
    IplImageToQImage.cpp \
    facedetect.cpp

HEADERS  += handgesturedialog.h \
    SRC/GestureStruct.h \
    SRC/GestrueInfo.h \
    SRC/AIGesture.h \
    IplImageToQImage.h \
    facedetect.h

FORMS    += handgesturedialog.ui

INCLUDEPATH += C:\Qt\opencv3.0_use\include
INCLUDEPATH += C:\Qt\opencv3.0_use\include\opencv
INCLUDEPATH += C:\Qt\opencv3.0_use\include\opencv2
LIBS += C:\Qt\opencv3.0_use\lib\libopencv_*.a

LIBS += -lpthread
