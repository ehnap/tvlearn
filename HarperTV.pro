QT       += core widgets openglwidgets multimedia network

TARGET = HarperTV
TEMPLATE = app
CONFIG += c++17

# MPV library configuration
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += mpv
}

win32 {
    # Windows-specific MPV configuration
    # Adjust these paths according to your MPV installation
    INCLUDEPATH += C:/mpv-dev/include
    LIBS += -LC:/mpv-dev/lib -lmpv
}

macx {
    # macOS-specific MPV configuration
    # MPV installed via Homebrew
    INCLUDEPATH += /opt/homebrew/Cellar/mpv/0.40.0_1/include
    LIBS += -L/opt/homebrew/Cellar/mpv/0.40.0_1/lib -lmpv
}

# Define source files
SOURCES += \
    src/main.cpp \
    src/ui/mainwindow.cpp \
    src/ui/videowidget.cpp \
    src/ui/playercontrols.cpp \
    src/ui/channelselector.cpp \
    src/ui/settingsdialog.cpp \
    src/core/mediaplayer.cpp \
    src/core/mpvcore.cpp \
    src/core/playbackcontroller.cpp \
    src/core/channelmanager.cpp \
    src/core/jsonparser.cpp \
    src/data/settings.cpp \
    src/data/channeldata.cpp

HEADERS += \
    src/ui/mainwindow.h \
    src/ui/videowidget.h \
    src/ui/playercontrols.h \
    src/ui/channelselector.h \
    src/ui/settingsdialog.h \
    src/core/mediaplayer.h \
    src/core/mpvcore.h \
    src/core/playbackcontroller.h \
    src/core/channelmanager.h \
    src/core/jsonparser.h \
    src/data/settings.h \
    src/data/channeldata.h

# Resource files
RESOURCES += \
    resources/resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
