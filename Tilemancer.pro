# Simple qmake project to get Tilemancer to compile on Ubuntu 16.04
#
# Dependencies:
#
#   libglm-dev liblua5.3-dev libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-image-dev
#

TEMPLATE = app
TARGET = Tilemancer
INCLUDEPATH += .

CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += \
    /usr/include/lua5.3 \
    /usr/include/glm

LIBS += -llua5.3 -lSDL2 -lpthread -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lGL

SOURCES += main.cpp
