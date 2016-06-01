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

images.path = $$OUT_PWD
images.files += \
    bezierFillError.png \
    bezierFill.png \
    damage.png \
    effect0.png \
    effect10.png \
    effect11.png \
    effect12.png \
    effect13.png \
    effect14.png \
    effect15.png \
    effect16.png \
    effect1.png \
    effect2.png \
    effect3.png \
    effect4.png \
    effect5.png \
    effect6.png \
    effect7.png \
    effect8.png \
    effect9.png \
    font2.png \
    font3.png \
    font.png \
    grid.png \
    icon0.png \
    icon10.png \
    icon11.png \
    icon12.png \
    icon13.png \
    icon1.png \
    icon2.png \
    icon3.png \
    icon4.png \
    icon5.png \
    icon6.png \
    icon7.png \
    icon8.png \
    icon9.png \
    logo.png \
    pal.png

nodes.path = $$OUT_PWD/Nodes
nodes.files = \
    nodes.files = \
    Nodes/absolute.lua \
    Nodes/colorizer.lua \
    Nodes/blend.lua \
    Nodes/directional_warp.lua \
    Nodes/bricks.lua \
    Nodes/blur.lua \
    Nodes/shape.lua \
    Nodes/lighting.lua \
    Nodes/scatter.lua \
    Nodes/invert.lua \
    Nodes/edge_detect.lua \
    Nodes/cells.lua \
    Nodes/directional_blur.lua \
    Nodes/perlin_noise.lua

presets.path = $$OUT_PWD/Presets
presets.files = \
    Presets/bricks_2.lua \
    Presets/rocks_1.lua \
    Presets/rocks_2.lua \
    Presets/sand_1.lua \
    Presets/roughness.lua \
    Presets/bricks_1.lua

INSTALLS += images nodes presets
