QT += \
    qml \
    gui \
    quick \
    quickcontrols2 \
    widgets \
    network

CONFIG += \
    c++11 \
    precompile_header

SOURCES += \
    Sources/Main.cpp \
    Sources/RegisterTypes.cpp \
    Sources/TumblrPCH.cpp \
    Sources/Tumblr/TumblrBlog.cpp \
    Sources/Tumblr/TumblrDatabase.cpp \
    Sources/Utils/HttpRequest.cpp \
    Sources/Utils/Image.cpp \
    Sources/Utils/Memory.cpp

RESOURCES += \
    Sources/Qml.qrc \
    Resources/Resources.qrc

HEADERS += \
    Sources/RegisterTypes.h \
    Sources/TumblrPCH.h \
    Sources/Tumblr/TumblrBlog.h \
    Sources/Tumblr/TumblrDatabase.h \
    Sources/Utils/HttpRequest.h \
    Sources/Utils/Image.h \
    Sources/Utils/Memory.h \
    Sources/Utils/TaskManager.h

#
# Global config
#
INCLUDEPATH += Sources
PRECOMPILED_HEADER = Sources/TumblrPCH.h
