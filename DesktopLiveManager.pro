#-------------------------------------------------
#
# Project created by QtCreator 2014-02-17T00:46:34
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DesktopLiveManager
TEMPLATE = app
LIBS += -lx264 -lmp3lame -lrtmp

linux {
    QT += x11extras
    LIBS += -lXfixes
}

HEADERS += \
    core/mainwindow.h \
    core/streamcontroller.h \
    util/flvstreamer.h \
    graphics/graphicscontroller.h \
    graphics/graphicsscene.h \
    graphics/itemmodel.h \
    graphics/itemview.h \
    external/rtmpstreamer.h \
    graphics/item/igraphicsitem.h \
    graphics/item/picture.h \
    graphics/item/pixmap.h \
    graphics/item/screenareaselector.h \
    graphics/item/screencapture.h \
    graphics/item/text.h \
    graphics/property/propertybase.h \
    graphics/property/propertycontroller.h \
    graphics/property/screencaptureproperty.h \
    graphics/property/pictureproperty.h \
    graphics/item/video.h \
    graphics/property/textproperty.h \
    core/audiocontroller.h \
    external/videoencoder.h \
    external/audioencoder.h \
    graphics/property/videoproperty.h \
    core/mediaprocessor.h \
    util/mousecursor.h

SOURCES += \
    core/main.cpp \
    core/mainwindow.cpp \
    core/streamcontroller.cpp \
    util/flvstreamer.cpp \
    graphics/graphicscontroller.cpp \
    graphics/graphicsscene.cpp \
    graphics/itemmodel.cpp \
    graphics/itemview.cpp \
    external/rtmpstreamer.cpp \
    graphics/item/igraphicsitem.cpp \
    graphics/item/picture.cpp \
    graphics/item/pixmap.cpp \
    graphics/item/screenareaselector.cpp \
    graphics/item/screencapture.cpp \
    graphics/item/text.cpp \
    graphics/property/propertybase.cpp \
    graphics/property/propertycontroller.cpp \
    graphics/property/screencaptureproperty.cpp \
    graphics/property/pictureproperty.cpp \
    graphics/item/video.cpp \
    graphics/property/textproperty.cpp \
    core/audiocontroller.cpp \
    external/videoencoder.cpp \
    external/audioencoder.cpp \
    graphics/property/videoproperty.cpp \
    core/mediaprocessor.cpp \
    util/mousecursor.cpp

FORMS    += \
    core/mainwindow.ui \
    graphics/property/propertybase.ui \
    core/general.ui \
    core/encoder.ui \
    graphics/property/videoproperty.ui \
    graphics/property/pictureproperty.ui \
    graphics/property/textproperty.ui

RESOURCES += \
    resource.qrc
