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
    core/audiocontroller.h \
    core/mainwindow.h \
    core/mediaprocessor.h \
    core/streamcontroller.h \
    external/audioencoder.h \
    external/rtmpstreamer.h \
    external/videoencoder.h \
    graphics/graphicscontroller.h \
    graphics/graphicsscene.h \
    graphics/item/camera.h \
    graphics/item/graphicsitem.h \
    graphics/item/igraphicsitem.h \
    graphics/item/picture.h \
    graphics/item/pixmap.h \
    graphics/item/screencapture.h \
    graphics/item/text.h \
    graphics/item/video.h \
    graphics/itemmodel.h \
    graphics/itemview.h \
    graphics/property/cameraproperty.h \
    graphics/property/pictureproperty.h \
    graphics/property/propertybase.h \
    graphics/property/propertycontroller.h \
    graphics/property/screencaptureproperty.h \
    graphics/property/textproperty.h \
    graphics/property/videoproperty.h \
    util/flvstreamer.h \
    util/mousecursor.h \
    util/screenareaselector.h

SOURCES += \
    core/audiocontroller.cpp \
    core/main.cpp \
    core/mainwindow.cpp \
    core/mediaprocessor.cpp \
    core/streamcontroller.cpp \
    external/audioencoder.cpp \
    external/rtmpstreamer.cpp \
    external/videoencoder.cpp \
    graphics/graphicscontroller.cpp \
    graphics/graphicsscene.cpp \
    graphics/item/camera.cpp \
    graphics/item/igraphicsitem.cpp \
    graphics/item/picture.cpp \
    graphics/item/pixmap.cpp \
    graphics/item/screencapture.cpp \
    graphics/item/text.cpp \
    graphics/item/video.cpp \
    graphics/itemmodel.cpp \
    graphics/itemview.cpp \
    graphics/property/cameraproperty.cpp \
    graphics/property/pictureproperty.cpp \
    graphics/property/propertybase.cpp \
    graphics/property/propertycontroller.cpp \
    graphics/property/screencaptureproperty.cpp \
    graphics/property/textproperty.cpp \
    graphics/property/videoproperty.cpp \
    util/flvstreamer.cpp \
    util/mousecursor.cpp \
    util/screenareaselector.cpp

FORMS    += \
    core/encoder.ui \
    core/general.ui \
    core/mainwindow.ui \
    core/view.ui \
    graphics/property/cameraproperty.ui \
    graphics/property/pictureproperty.ui \
    graphics/property/propertybase.ui \
    graphics/property/textproperty.ui \
    graphics/property/videoproperty.ui

RESOURCES += \
    resource.qrc
