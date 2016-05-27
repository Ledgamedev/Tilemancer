/*
 
  Tilemancer
  Copyright (C) 2016  Lucca Pedersoli Junior
    
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
    
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
        
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
*/

#define _USE_MATH_DEFINES

#ifdef _WIN32
    #define GLEW_STATIC
    #include <SDL.h>
    #include <windows.h>
    #include <GL\glew.h>
    #include <gl\glu.h>
    #include "minizip/unzip.h"
    #include <SDL_image.h>
    #include <SDL_mixer.h>
    #include <SDL_ttf.h>
    #include "mingw.mutex.h"
    #include "mingw.thread.h"
    #include <Shlobj.h>
    #define SDL_GetGlobalMouseState SDL_GetMouseState
    #define glGenFramebuffers glGenFramebuffersEXT
    #define glBindFramebuffer glBindFramebufferEXT
    #define glFramebufferTexture2D glFramebufferTexture2DEXT
    #define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
    #define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
    #define GL_COLOR_ATTACHMENT1 GL_COLOR_ATTACHMENT1_EXT
    #define GL_COLOR_ATTACHMENT2 GL_COLOR_ATTACHMENT2_EXT
    #define GL_COLOR_ATTACHMENT3 GL_COLOR_ATTACHMENT3_EXT
    const char* light_vertex_shader_source = (char*)"#version 130\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 off; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = int(mod(frame, 5)); int ty = frame/5; txc = vec2(((t.x+tx)*frameSize.x+off.x)/texSize.x, ((t.y+ty)*frameSize.y+off.y)/texSize.y); }";
    const char* light_fragment_shader_source = (char*)"#version 130\nout vec4 fragColor; uniform vec3 color; uniform vec3 color2; in vec2 txc; void main() { fragColor = vec4(color*(1.0-txc.x)+color2*txc.x, 1.0); }";

    const char* blur_vertex_shader_source = (char*)"#version 130\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = int(mod(frame, 5)); int ty = frame/5; txc = vec2((t.x+tx)*frameSize.x/texSize.x, (t.y+ty)*frameSize.y/texSize.y); }";
    const char* blur_fragment_shader_source = (char*)"#version 130\n#define M_PI 3.1415926535897932384626433832795\nout vec4 fragColor; in vec2 txc; uniform sampler2D tex; uniform vec2 texSize; uniform float v; uniform bool tri; uniform bool full; void main() { float a = 0.0; vec2 dist = vec2(0.5, 0.5)-txc; a = ((length(dist)>0.5) ? 0.0 : 1.0); a = ((length(dist)<0.4) ? 0.0 : a); a = (tri ? 1.0 : a); float H = atan(dist.y, dist.x)/M_PI*180.0+180.0; H = (H==360 ? 0.0 : H); H = (tri ? v : H); float S = (tri ? txc.x/txc.y : 1.0); float V = (tri ? txc.y : 1.0); float C = V*S; float X = C*(1.0-abs(mod(H/60.0, 2)-1.0)); float m = V-C; vec3 rgb = vec3(0.0, 0.0, 0.0); if(0 <= H && H < 60) { rgb = vec3(C, X, 0.0); } else if(60 <= H && H < 120) { rgb = vec3(X, C, 0.0); } else if(120 <= H && H < 180) { rgb = vec3(0.0, C, X); } else if(180 <= H && H < 240) { rgb = vec3(0.0, X, C); } else if(240 <= H && H < 300) { rgb = vec3(X, 0.0, C); } else if(300 <= H && H < 360) { rgb = vec3(C, 0.0, X); } rgb = rgb+m; float d = 10.0; vec3 finalColor = vec3(0.0, 0.0, 0.0); for(int x = 0; x < texSize.x; x++) { for(int y = 0; y < texSize.y; y++) { vec3 color = vec3(texture2D(tex, vec2(x/texSize.x, y/texSize.y))); vec3 dist = rgb-color; float d2 = length(dist); if(d2 < d) { d = d2; finalColor = color; } } } if(full) { finalColor = rgb; }  fragColor = vec4(finalColor, a); }";

    const char* transition_vertex_shader_source = (char*)"#version 130\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; uniform float tSize; uniform float aspect; uniform vec2 count; uniform int on; uniform vec2 resolution; uniform vec2 ij; uniform float time; in vec2 t; out vec2 txc; void main() { vec2 totalSize = vec2(tSize, tSize)*count; vec2 p = vec2(ij.x*(tSize-1.0)+tSize/2.0, ij.y*(tSize-1.0)+tSize/2.0)+(resolution-totalSize)/2.0; float dx = 5.0; float dy = 5.0/aspect; float sizeX = 1.0-((count.x-ij.x)*(1.0-on)+ij.x*on+dx-time)/dx; float sizeY = 1.0-((count.y-ij.y)*(1.0-on)+ij.y*on+dy-time/aspect)/dy; float size = clamp(sizeX/2.0+sizeY/2.0, 0.0, 1.0); gl_Position = (proj*model) * vec4((position.xy-0.5)*vec2(tSize, tSize)*size+p, position.zw); txc = t; }";
    const char* transition_fragment_shader_source = (char*)"#version 130\nlayout(location = 0) out vec4 fragColor; uniform sampler2D tex; in vec2 txc; void main() { fragColor = texture2D(tex, txc); }";

    const char* my_vertex_shader_source = (char*)"#version 130\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 off; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = int(mod(frame, 5)); int ty = frame/5; txc = vec2(((t.x+tx)*frameSize.x+off.x)/texSize.x, ((t.y+ty)*frameSize.y+off.y)/texSize.y); }";
    const char* my_fragment_shader_source = (char*)"#version 130\n uniform sampler2D tex; in vec2 txc; out vec4 fragColor; uniform float alpha; void main() { fragColor = vec4(texture2D(tex, txc).xyz, texture2D(tex, txc).w*alpha); }";
    int OS = 0;
#elif __APPLE__
    #include <SDL2/SDL.h>
    #include <OpenGL/gl3.h>
    #include "CoreFoundation/CFBundle.h"
    #include <SDL2_image/SDL_image.h>
    #include <SDL2_mixer/SDL_mixer.h>
    #include <SDL2_ttf/SDL_ttf.h>
    #include <pwd.h>
    #include <future>
    #include <mach-o/dyld.h>
    const char* light_vertex_shader_source = (char*)"#version 330\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 off; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = frame%5; int ty = frame/5; txc = vec2(((t.x+tx)*frameSize.x+off.x)/texSize.x, ((t.y+ty)*frameSize.y+off.y)/texSize.y); }";
    const char* light_fragment_shader_source = (char*)"#version 330\nlayout(location = 0) out vec4 fragColor; uniform vec3 color; uniform vec3 color2; in vec2 txc; void main() { fragColor = vec4(color*(1.0-txc.x)+color2*txc.x, 1.0); }";

    const char* blur_vertex_shader_source = (char*)"#version 330\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = frame%5; int ty = frame/5; txc = vec2((t.x+tx)*frameSize.x/texSize.x, (t.y+ty)*frameSize.y/texSize.y); }";
    const char* blur_fragment_shader_source = (char*)"#version 330\n#define M_PI 3.1415926535897932384626433832795\nlayout(location = 0) out vec4 fragColor; in vec2 txc; uniform sampler2D tex; uniform vec2 texSize; uniform float v; uniform bool tri; uniform bool full; void main() { float a = 0.0; vec2 dist = vec2(0.5, 0.5)-txc; a = ((length(dist)>0.5) ? 0.0 : 1.0); a = ((length(dist)<0.4) ? 0.0 : a); a = (tri ? 1.0 : a); float H = atan(dist.y, dist.x)/M_PI*180.0+180.0; H = (H==360 ? 0.0 : H); H = (tri ? v : H); float S = (tri ? txc.x/txc.y : 1.0); float V = (tri ? txc.y : 1.0); float C = V*S; float X = C*(1.0-abs(mod(H/60.0, 2)-1.0)); float m = V-C; vec3 rgb = vec3(0.0, 0.0, 0.0); if(0 <= H && H < 60) { rgb = vec3(C, X, 0.0); } else if(60 <= H && H < 120) { rgb = vec3(X, C, 0.0); } else if(120 <= H && H < 180) { rgb = vec3(0.0, C, X); } else if(180 <= H && H < 240) { rgb = vec3(0.0, X, C); } else if(240 <= H && H < 300) { rgb = vec3(X, 0.0, C); } else if(300 <= H && H < 360) { rgb = vec3(C, 0.0, X); } rgb = rgb+m; float d = 10.0; vec3 finalColor = vec3(0.0, 0.0, 0.0); for(int x = 0; x < texSize.x; x++) { for(int y = 0; y < texSize.y; y++) { vec3 color = vec3(texture(tex, vec2(x/texSize.x, y/texSize.y))); vec3 dist = rgb-color; float d2 = length(dist); if(d2 < d) { d = d2; finalColor = color; } } } if(full) { finalColor = rgb; }  fragColor = vec4(finalColor, a); }";

    const char* transition_vertex_shader_source = (char*)"#version 330\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = frame%5; int ty = frame/5; txc = vec2((t.x+tx)*frameSize.x/texSize.x, (t.y+ty)*frameSize.y/texSize.y); }";
    const char* transition_fragment_shader_source = (char*)"#version 330\nlayout(location = 0) out vec4 fragColor; uniform float rot; uniform vec2 texSize; uniform sampler2D tex; uniform sampler2D texP; in vec2 txc; void main() { vec3 off = vec3(-1.0/texSize.x, 0.0, 1.0/texSize.x); vec2 size = vec2(2.0, 0.0); float s11 = texture(tex, txc).x; float s01 = texture(tex, txc+off.xy).x; float s21 = texture(tex, txc+off.zy).x; float s10 = texture(tex, txc+off.yx).x; float s12 = texture(tex, txc+off.yz).x; vec3 va = normalize(vec3(size.xy,s21-s01)); vec3 vb = normalize(vec3(size.yx,s12-s10)); vec3 bump = vec3(cross(va,vb)); vec3 light = vec3(sin(rot), -cos(rot), 0.3); float d = max(0.0, dot(normalize(light), bump)); vec2 ptxc = vec2(d, 0.0); vec3 colorFinal = vec3(texture(texP, ptxc)); fragColor = vec4(colorFinal, 1.0)";

    const char* my_vertex_shader_source = (char*)"#version 330\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 off; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = frame%5; int ty = frame/5; txc = vec2(((t.x+tx)*frameSize.x+off.x)/texSize.x, ((t.y+ty)*frameSize.y+off.y)/texSize.y); }";
    const char* my_fragment_shader_source = (char*)"#version 330\nlayout(location = 0) out vec4 fragColor; layout(location = 1) out vec4 fragColorN; layout(location = 2) out vec4 fragColorMisc; layout(location = 3) out vec4 fragColorB; uniform vec2 flip; uniform sampler2D tex; uniform float strength; uniform float alpha; uniform float depth; uniform sampler2D texN; in vec2 txc; void main() { fragColor = vec4(texture(tex, txc).xyz/texture(tex, txc).w, texture(tex, txc).w*alpha); }";
    CFBundleRef mainBundle;
    int OS = 1;
#elif __linux__
    int OS = 2;
#else
    int OS = -1;
#endif

#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/transform.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lua.hpp"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

const int Vmajor = 0;
const int Vminor = 2;
const int Vrevision = 0;

SDL_Window* window = NULL;
SDL_GLContext gContext = NULL;
SDL_Renderer* gRenderer = NULL;

class File {
public:
    File(string n, bool f);
    ~File();
    string name;
    bool folder;
};

class bPoint {
public:
    bPoint();
    bPoint(float x, float y);
    float x;
    float y;
};

class cBezier {
public:
    cBezier();
    bPoint getTangent(float t);
    bPoint getPoint(float t);
    void create();
    void update();
    int d = 100;
    GLuint bVbo;
    bPoint P0;
    bPoint P1;
    bPoint P2;
    bPoint P3;
};

class Layer;

class CPoint;

class Socket;

class nEffect;

class Parameter {
public:
    Parameter(int ID, string name, float x, float y, float w, float h, int value, int value2, int value3, string tt);
    ~Parameter();
    void render(int ex, int ey);
    void mouseDown(int mx, int my, int ex, int ey, int layer, nEffect* fx);
    void mouseMove(int mx, int my, int ex, int ey, int layer, nEffect* fx);
    void mouseUp(int mx, int my, int ex, int ey, int layer, nEffect* fx);
    int ID;
    string tt;
    string name;
    Layer* ref;
    Socket* s;
    vector<CPoint*> points;
    Parameter* p;
    int index;
    int selectedPoint;
    float x;
    float y;
    float w;
    float h;
    float resetValue;
    string typing;
    float value;
    float value2;
    float value3;
    float value4;
    bool dragging;
    bool dragging2;
    float oldmX;
    float oldmY;
    float initmX;
    float initmY;
};

class Color {
public:
    Color(int r, int g, int b);
    ~Color();
    bool equals(Color*);
    int r;
    int g;
    int b;
    int a;
    bool disabled;
};

class CPoint {
public:
    CPoint();
    float r;
    float g;
    float b;
    int i;
    float a;
};

class Texture;

class Socket {
public:
    Socket();
    ~Socket();
    float y;
    bool snapped;
    bool infloop;
    Socket* s;
    cBezier* b;
    int index;
    float px;
    float py;
    int futureN;
    int futureS;
    nEffect* parent;
    vector<float> texData;
    GLuint texture;
    string lastTexDir;
    string lastTexName;
};

class Text {
public:
    string name;
    float y;
};

class nEffect {
public:
    nEffect(string luafn, string fxname, bool preset = false);
    ~nEffect();
    lua_State *L;
    unsigned int seed;
    std::thread side;
    void sideUpdate();
    void clearTree();
    vector<Socket*> inputs;
    vector<Socket*> outputs;
    vector<Parameter*> params;
    vector<Text*> texts;
    vector<nEffect*> presetFxs;
    string luafn;
    string fxname;
    string name;
    string desc;
    int nextP;
    bool cleared;
    bool presetError;
    bool abort;
    bool deleted;
    bool loading;
    bool done;
    bool undone;
    int doneTimer;
    bool loaded;
    bool isPreset;
    float x;
    float y;
    float sx;
    float sy;
    float w;
    float h;
    float r;

    int setName(lua_State *L);
    int setDesc(lua_State *L);
    int setSize(lua_State *L);
    int addInput(lua_State *L);
    int addParameter(lua_State *L);
    int addInputParameter(lua_State *L);
    int addOutput(lua_State *L);
    int getTileSize(lua_State *L);
    int getValue(lua_State *L);
    int setPixel(lua_State *L);
    int addCRamp(lua_State *L);
    int finalize(lua_State *L);

    int luaSeed(lua_State *L);
    int luaRand(lua_State *L);

    int addNode(lua_State *L);
    int setParameter(lua_State *L);
    int addConnection(lua_State *L);
    void AbortLua(lua_State* L, lua_Debug* ar);
};

class Texture {
public:
    ~Texture();
    std::thread side;
    void genTex(bool first = true);
    void sideUpdate();
    bool loading;
    bool done;
    int doneTimer;
    bool loaded;
    bool abort;
    vector<float> texData;
    vector<nEffect*> fxs;
    GLuint texture;
    string lastTexDir;
    string lastTexName;
};

double dt = 1.0 / 60.0;
double currentTime;
double lastTime;
double accumulator;
unsigned int fps;
int fpsTimer;
TTF_Font* font;

float texSizeX;
float texSizeY;
float texType;
float pixelSize;
int screenW;
int screenH;
int displayW;
int displayH;
int screenPW;
int screenPH;
float camX;
float camY;
float nodeCSX;
float nodeCSY;
float nodeCX;
float nodeCY;
bool camMoving = false;
bool draggingCam = false;
bool draggingCam2 = false;
float camOffset;
float camOffsetSpeed;
float camOffsetMagnitude;
bool camPositive;
float camRot;
bool camFixed;
int camXFinal;
int camYFinal;
//int selectedTile = 32;
int zoom = 1;
int zoomA = 1;
float tilesW;
float tilesH;
bool draggingLayer;
int draggedLayer;
int draggedFX;
bool draggingNode;
int draggedNode;
bool draggingSocket;
bool cancelDrag;
int draggedSocket;
int targetDrag;
float screenScale = 2.0;
int logoTimer;
int logoTimerMax = 150;
bool logoType;
string toolTip;
string errorMessage;
int errorTimer;
vector<float> data;
vector<Color*> palette;
Color rampA(254, 237, 186);
Color rampB(54, 42, 96);
int selectedColor = 0;
vector<Parameter*> colorParams;
Parameter* textType;
nEffect* textTypeFx;
int textTypeLayer;
Parameter* textTypeTemp;
nEffect* textTypeFxTemp;
int textTypeLayerTemp;
int blinkTimer;
bool RoC = true;
vector<Texture*> texs;
vector<nEffect*> newEffects;
int collH = 18;
int expH = 92;
int expandedLayer;
bool layerExpanded;
int currentTexture;
Socket* currentSocket;
float barX;
float barXRight;
float barY;
float barY2;
float barY3;
float barY4;
float barY5;
float layersScroll;
float toolsScroll;
float palScroll;
float texScroll;
float newEScroll;
float layersScrollH;
bool scrollDir;
bool scrollSet;
float prevLayersScroll;
float browserScroll;
int selectedFX;
bool draggingParam;
bool draggingFX;
bool draggingNew;
bool draggingUI;
bool draggingSBar;
int sBarDrag;
float sRatio;
int rotTimer;
int UIdragType;
int UIdragRange = 5;
float mouseOX;
float mouseOY;
float mouseX;
float mouseY;
bool gameStarted;
int view = 0;
int doubleClickTimer;
int typeTimer;
float mouseSX;
float mouseSY;
float mouseSDX;
float mouseSDY;
float prevC;
float prevCount;
Socket* preview;
int previewTimer;
vector<string> listUndo;
vector<string> listRedo;
string currentDir;
string filenameB;
bool browserOpen;
int browserMode;
vector<File*> filenames;
vector<string> fnUndo;
vector<string> fnRedo;
string lastPalDir;
string lastPalName;
string lastSaveDir;
string lastSaveName;
string lastTexDir;
string lastTexName;
bool overwrite;
int selectedFile;
GLuint gridImg;
GLuint effectImg;
GLuint effectImg2;
GLuint effectImg3;
GLuint effectImg4;
GLuint effectImg5;
GLuint effectImg6;
GLuint effectImg7;
GLuint effectImg8;
GLuint effectImg9;
GLuint effectImg10;
GLuint effectImg11;
GLuint effectImg12;
GLuint effectImg13;
GLuint effectImg14;
GLuint effectImg15;
GLuint effectImg16;
GLuint effectImg17;
GLuint bezierFill;
GLuint bezierFillError;
GLuint iconImg0;
GLuint iconImg1;
GLuint iconImg2;
GLuint iconImg3;
GLuint iconImg4;
GLuint iconImg5;
GLuint iconImg6;
GLuint iconImg7;
GLuint iconImg8;
GLuint iconImg9;
GLuint iconImg10;
GLuint iconImg11;
GLuint iconImg12;
GLuint iconImg13;
GLuint palImg;
GLuint palImgReal;
GLuint postImg;
GLuint texImgFinal;
GLuint fontImg;
GLuint fontImg2;
GLuint fontImg3;

GLenum my_program;
GLenum my_vertex_shader;
GLenum my_fragment_shader;

GLenum light_program;
GLenum light_vertex_shader;
GLenum light_fragment_shader;

GLenum blur_program;
GLenum blur_vertex_shader;
GLenum blur_fragment_shader;

GLenum transition_program;
GLenum transition_vertex_shader;
GLenum transition_fragment_shader;

GLuint screenFbo;
GLuint screenTex;
GLuint screenTexN;
GLuint screenTexB;
GLuint screenTexMisc;

GLuint screenFboFinal;
GLuint screenTexFinal;

GLuint screenFboBloom;
GLuint screenTexBloom;

GLuint screenFboBloom2;
GLuint screenTexBloom2;

GLuint screenFboFinal2;
GLuint screenTexFinal2;

GLuint vbo;
GLuint vboTri;
GLuint vao;

GLuint lightTex;
GLuint foregroundTex;
GLuint hiddenImg;
GLuint hidden2Img;
GLuint digitsImg;
GLuint brightnessImg;
GLuint ditherTex;
GLuint noTex;
GLuint vignetteImg;
GLuint logoImage;
GLuint titleImage;
GLuint thanksImage;
GLuint flashImg;

GLint mp_tex;
GLint mp_texN;
GLint mp_frameSize;
GLint mp_texSize;
GLint mp_off;
GLint mp_frame;
GLint mp_depth;
GLint mp_alpha;
GLint mp_strength;
GLint mp_model;
GLint mp_flip;
GLint mp_proj;
lua_State *L;

glm::mat4 model = glm::mat4(1.0);
glm::mat4 proj = glm::mat4(1.0);

void renderSprite2(int frame, float x, float y, float w, float h, GLuint tex, float frameW, float frameH, float rot, float centerX, float centerY, float alpha, GLuint texN, float depth, float strength, bool flipX, bool flipY, float offX = 0, float offY = 0, int cutoff = -1, int cutoff2 = -1, int cutoff3 = -1, int cutoff4 = -1);

void renderSprite(int frame, float x, float y, float w, float h, GLuint tex, float frameW, float frameH, float rot, float centerX, float centerY, float alpha, GLuint texN, float depth, float strength, bool flipX, bool flipY, float offX = 0, float offY = 0, int cutoff = -1, int cutoff2 = -1, int cutoff3 = -1, int cutoff4 = -1);

#ifdef _WIN32
GLuint loadTexture(string path) {
    GLuint tex;
    unzFile data = unzOpen("data.lpf");
    unz_file_info info;
    Uint8* buffer;
    SDL_RWops* rw = NULL;
    SDL_Surface* ls = NULL;

    unzLocateFile(data, path.c_str(), NULL);
    unzOpenCurrentFile(data);
    unzGetCurrentFileInfo(data, &info, NULL, 0, NULL, 0, NULL, 0);
    buffer = (Uint8*)malloc(info.uncompressed_size);
    unzReadCurrentFile(data, buffer, info.uncompressed_size);
    rw = SDL_RWFromConstMem(buffer, info.uncompressed_size);
    ls = IMG_LoadPNG_RW(rw);
    free(buffer);

    if(ls != NULL) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        int mode = GL_RGB;
        int mode2 = GL_BGR;
        if(ls->format->BytesPerPixel == 4) {
            mode = GL_RGBA;
            mode2 = GL_BGRA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, mode, ls->w, ls->h, 0, mode, GL_UNSIGNED_BYTE, ls->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //SDL_FreeSurface(ls);
    } else {
        //cout << "Failed to load image: "+path << endl;
    }
    return tex;
}

GLuint loadTexture2(string path) {
    GLuint tex;
    SDL_Surface* ls = IMG_Load(path.c_str());
    if(ls != NULL) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        int mode = GL_RGB;
        int mode2 = GL_BGR;
        if(ls->format->BytesPerPixel == 4) {
            mode = GL_RGBA;
            mode2 = GL_BGRA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, mode, ls->w, ls->h, 0, mode, GL_UNSIGNED_BYTE, ls->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //SDL_FreeSurface(ls);
    } else {
        //cout << "Failed to load image: "+path << endl;
    }
    return tex;
}
#elif __APPLE__
GLuint loadTexture(string path) {
    GLuint tex;
    SDL_Surface* ls = IMG_Load(path.c_str());
    if(ls != NULL) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        int mode = GL_RGB;
        int mode2 = GL_BGR;
        if(ls->format->BytesPerPixel == 4) {
            mode = GL_RGBA;
            mode2 = GL_BGRA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, mode, ls->w, ls->h, 0, mode2, GL_UNSIGNED_BYTE, ls->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //SDL_FreeSurface(ls);
    } else {
        //cout << "Failed to load image: "+path << endl;
    }
    return tex;
}

GLuint loadTexture2(string path) {
    return NULL;
}
#elif __linux__
#else
#endif

void setPixel(vector<float> & data, int x, int y, Color* color, bool wrap) {
    bool render = true;
    if(x >= texSizeX) {
        if(wrap) {
            while(x >= texSizeX) {
                x -= texSizeX;
            }
        } else {
            render = false;
        }
    } else if(x < 0) {
        if(wrap) {
            while(x < 0) {
                x += texSizeX;
            }
        } else {
            render = false;
        }
    }
    if(y >= texSizeY) {
        if(wrap) {
            while(y >= texSizeY) {
                y -= texSizeY;
            }
        } else {
            render = false;
        }
    } else if(y < 0) {
        if(wrap) {
            while(y < 0) {
                y += texSizeY;
            }
        } else {
            render = false;
        }
    }
    if(render && data.size() > (y*texSizeX+x)*4+3 && color != NULL) {
        data[(y*texSizeX+x)*4+0] = color->r;
        data[(y*texSizeX+x)*4+1] = color->g;
        data[(y*texSizeX+x)*4+2] = color->b;
        data[(y*texSizeX+x)*4+3] = color->a;
    }
}

Color getColor(vector<float> & data, int x, int y, bool wrap) {
    Color c = Color(0, 0, 0);
    bool render = true;
    if(x >= texSizeX) {
        if(wrap) {
            while(x >= texSizeX) {
                x -= texSizeX;
            }
        } else {
            render = false;
        }
    } else if(x < 0) {
        if(wrap) {
            while(x < 0) {
                x += texSizeX;
            }
        } else {
            render = false;
        }
    }
    if(y >= texSizeY) {
        if(wrap) {
            while(y >= texSizeY) {
                y -= texSizeY;
            }
        } else {
            render = false;
        }
    } else if(y < 0) {
        if(wrap) {
            while(y < 0) {
                y += texSizeY;
            }
        } else {
            render = false;
        }
    }
    if(render && data.size() > (y*texSizeX+x)*4+3) {
        c.r = data[(y*texSizeX+x)*4+0];
        c.g = data[(y*texSizeX+x)*4+1];
        c.b = data[(y*texSizeX+x)*4+2];
        c.a = data[(y*texSizeX+x)*4+3];
    }
    return c;
}

void saveUndo();

Parameter::~Parameter() {
}

Color::~Color() {
}

File::~File() {
}

Texture::~Texture() {
}

void Texture::sideUpdate() {
}

void nEffect::sideUpdate() {
    luaL_dofile(this->L, luafn.c_str());
    lua_settop(this->L, 0);
    lua_getglobal(this->L, "apply");
    lua_pcall(this->L, 0, 0, 0);
    loaded = true;
}

void nEffect::clearTree() {
    cleared = true;
    done = false;
    doneTimer = 0;
    undone = false;
    for(int b = 0; b < outputs.size(); b++) {
        outputs.at(b)->texData.clear();
        for(int x = 0; x < texSizeX; x++) {
            for(int y = 0; y < texSizeY; y++) {
                for(int c = 0; c < 4; c++) {
                    outputs.at(b)->texData.push_back(0);
                }
            }
        }
    }
    for(int i = 0; i < texs.at(currentTexture)->fxs.size(); i++) {
        nEffect* fx = texs.at(currentTexture)->fxs.at(i);
        for(int in = 0; in < fx->inputs.size(); in++) {
            Socket* input = fx->inputs.at(in);
            if(input->s != NULL && input->s->parent == this) {
                if(fx->cleared) {
                    input->infloop = true;
                } else {
                    input->infloop = false;
                    fx->clearTree();
                }
            }
        }
    }
    cleared = false;
}

void Texture::genTex(bool first) {
    if(first) { //abort all before proceeding
        abort = true;
        for(int i = 0; i < fxs.size(); i++) {
            if(fxs.at(i)->loading) {
                fxs.at(i)->abort = true;
            }
        }
    } else {
        for(int i = 0; i < fxs.size(); i++) {
            if(!fxs.at(i)->done) {
                bool ready = true;
                for(int b = 0; b < fxs.at(i)->inputs.size(); b++) {
                    if(fxs.at(i)->inputs.at(b)->s != NULL) {
                        if(!fxs.at(i)->inputs.at(b)->s->parent->done) {
                            ready = false;
                        }
                    }
                }
                if(ready) {
                    if(!fxs.at(i)->loading) {
                        fxs.at(i)->loading = true;
                        fxs.at(i)->side = std::thread(&nEffect::sideUpdate, fxs.at(i));
                    }
                }
            }
        }
    }
}

Socket::Socket() {
    futureN = -1;
    futureS = -1;
    index = 0;
    infloop = false;
    s = NULL;
    b = new cBezier();
    b->create();
}

void exportTexSingle(string dir) {
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, texSizeX, texSizeY, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    glBindTexture(GL_TEXTURE_2D, currentSocket->texture);
    GLfloat *pixels = new GLfloat[int(texSizeX*texSizeY*4)];
    if(OS == 0) {
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    } else if(OS == 1) {
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels);
        for(int i = 0; i < texSizeX*texSizeY*4; i++) {
            Uint8 *p = (Uint8 *)surface->pixels+i*surface->format->BytesPerPixel/4;
            int val = pixels[i];
            if(val > 255) {
                val = 255;
            } else if(val < 0) {
                val = 0;
            }
            p[0] = (Uint8)val;
        }
    }
    IMG_SavePNG(surface, dir.c_str());
    SDL_FreeSurface(surface);
}

void exportPalette(string dir) {
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 16, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
    glBindTexture(GL_TEXTURE_2D, palImgReal);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    IMG_SavePNG(surface, dir.c_str());
    SDL_FreeSurface(surface);
}

Color HSVtoRGB(float H, float S, float V) {
    V /= 100;
    S /= 100;
    float C = V*S;
    float H2 = H/60.0;
    float X = C*(1.0-abs(fmod(H2, 2)-1.0));
    float R, G, B;
    if(0 <= H2 && H2 < 1) {
        R = C;
        G = X;
        B = 0;
    } else if(1 <= H2 && H2 < 2) {
        R = X;
        G = C;
        B = 0;
    } else if(2 <= H2 && H2 < 3) {
        R = 0;
        G = C;
        B = X;
    } else if(3 <= H2 && H2 < 4) {
        R = 0;
        G = X;
        B = C;
    } else if(4 <= H2 && H2 < 5) {
        R = X;
        G = 0;
        B = C;
    } else if(5 <= H2 && H2 < 6) {
        R = C;
        G = 0;
        B = X;
    }
    float m = V-C;
    R += m;
    G += m;
    B += m;
    R *= 255;
    G *= 255;
    B *= 255;

    Color rgb = Color(R, G, B);
    return rgb;
}

Color RGBtoHSV(float R, float G, float B) {
    float R1 = R/255.0;
    float G1 = G/255.0;
    float B1 = B/255.0;
    float cMax = max(max(R1, G1), B1);
    float cMin = min(min(R1, G1), B1);
    float delta = cMax-cMin;

    float H;
    if(delta == 0) {
        H = 0;
    } else if(cMax == R1) {
        H = 60*fmod(((G1-B1)/delta), 6);
    } else if(cMax == G1) {
        H = 60*((B1-R1)/delta+2);
    } else if(cMax == B1) {
        H = 60*((R1-G1)/delta+4);
    }

    while(H > 360) {
        H -= 360;
    }
    while(H < 0) {
        H += 360;
    }

    float S;
    if(cMax == 0) {
        S = 0;
    } else {
        S = delta/cMax;
    }
    S *= 100;

    float V = cMax*100;

    Color hsv = Color(H, S, V);
    return hsv;
}

Color RGBtoYUV(float R, float G, float B) {
    float Y = (( 0.299 * R + 0.587 * G + 0.114 * B) * 219 / 255) + 16;
    float U = ((-0.299 * R - 0.587 * G + 0.886 * B) * 224 / 1.772 / 255) + 128;
    float V = (( 0.701 * R - 0.587 * G - 0.114 * B) * 224 / 1.402 / 255) + 128;
    Color yuv = Color(Y, U, V);
    return yuv;
}

Color RGBtoLAB(float R, float G, float B) {//when procedural palettes were a thing
    float var_R = R / 255.0;
    float var_G = G / 255.0;
    float var_B = B / 255.0;

    if(var_R > 0.04045) {
        var_R = pow((var_R+0.055)/1.055, 2.4);
    } else {
        var_R = var_R / 12.92;
    }
    if(var_G > 0.04045) {
        var_G = pow((var_G+0.055)/1.055, 2.4);
    } else {
        var_G = var_G / 12.92;
    }
    if(var_B > 0.04045) {
        var_B = pow((var_B+0.055)/1.055, 2.4);
    } else {
        var_B = var_B / 12.92;
    }
    var_R = var_R * 100;
    var_G = var_G * 100;
    var_B = var_B * 100;

    float X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    float Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    float Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

    float var_X = X / 95.047;
    float var_Y = Y / 100.000;
    float var_Z = Z / 108.883;

    if(var_X > 0.008856) {
        var_X = pow(var_X, 1/3.0);
    } else {
        var_X = (7.787*var_X)+(16/116.0);
    }
    if(var_Y > 0.008856) {
        var_Y = pow(var_Y, 1/3.0);
    } else {
        var_Y = (7.787*var_Y)+(16/116.0);
    }
    if(var_Z > 0.008856) {
        var_Z = pow(var_Z, 1/3.0);
    } else {
        var_Z = (7.787*var_Z)+(16/116.0);
    }

    float L1 = (116*var_Y)-16;
    float A1 = 500*(var_X-var_Y);
    float B1 = 200*(var_Y-var_Z);

    /*cout << R << " " << G << " " << B << endl;
    cout << L1 << " " << A1 << " " << B1 << endl;
    cout << endl;*/
    Color lab = Color(L1, A1, B1);
    return lab;
}

double compareLAB(Color lab1, Color lab2) {
    double dx = lab2.r-lab1.r;
    double dy = lab2.g-lab1.g;
    double dz = lab2.b-lab1.b;
    double l = sqrt(dx*dx+dy*dy+dz*dz);
    return l;
}

double compareYUV(Color lab1, Color lab2) {
    double dx = lab2.r-lab1.r;
    dx = 0;
    double dy = lab2.g-lab1.g;
    double dz = lab2.b-lab1.b;
    double l = sqrt(dx*dx+dy*dy+dz*dz);
    return l;
}

Color* getPalColorReal(float R, float G, float B) {
    float dist = 36000.0;
    Color* final = NULL;
    Color lab1 = RGBtoLAB(R, G, B);
    Color yuv1 = RGBtoYUV(R, G, B);
    for(int i = 0; i < palette.size(); i++) {
        Color* pal = palette.at(i);
        Color lab2 = RGBtoLAB(pal->r, pal->g, pal->b);
        Color yuv2 = RGBtoYUV(pal->r, pal->g, pal->b);
        double l = compareLAB(lab1, lab2);
        double l2 = compareYUV(yuv1, yuv2);
        l2 = 0;
        double lFinal = sqrt(l*l+l2*l2);
        if(lFinal < dist && !pal->disabled && (prevC == 0 || lFinal < (prevC/prevCount)+10)) {
            dist = lFinal;
            final = pal;
            prevC += lFinal;
            prevCount++;
        }
    }
    return final;
}

Color* getPalColor(float H, float S, float V) {
    float C = V*S;
    float H2 = H/60.0;
    float X = C*(1.0-abs(fmod(H2, 2)-1.0));
    float R, G, B;
    if(0 <= H2 && H2 < 1) {
        R = C;
        G = X;
        B = 0;
    } else if(1 <= H2 && H2 < 2) {
        R = X;
        G = C;
        B = 0;
    } else if(2 <= H2 && H2 < 3) {
        R = 0;
        G = C;
        B = X;
    } else if(3 <= H2 && H2 < 4) {
        R = 0;
        G = X;
        B = C;
    } else if(4 <= H2 && H2 < 5) {
        R = X;
        G = 0;
        B = C;
    } else if(5 <= H2 && H2 < 6) {
        R = C;
        G = 0;
        B = X;
    }
    float m = V-C;
    R += m;
    G += m;
    B += m;
    R *= 255;
    G *= 255;
    B *= 255;
    float dist = 2550.0;
    Color* final = NULL;
    for(int i = 0; i < palette.size(); i++) {
        Color* pal = palette.at(i);
        float dx = pal->r - R;
        float dy = pal->g - G;
        float dz = pal->b - B;
        float l = sqrt(dx*dx+dy*dy+dz*dz);
        if(l < dist && !pal->disabled) {
            dist = l;
            final = pal;
        }
    }
    return final;
}

void beforePaletteChange() {//remove?
}

Color getGrad(Parameter* param, int index) {
    Color ffinal(0, 0, 0);
    Color* final2 = NULL;
    int darkest = 0;
    float da = 1.0;
    float ca = 0.0;
    float a = index/100.0;
    for(int pp = 0; pp < param->points.size(); pp++) {
        if(param->points.at(pp)->a <= da) {
            darkest = pp;
            da = param->points.at(pp)->a;
        }
        if(param->points.at(pp)->a >= ca && a > param->points.at(pp)->a) {
            ca = param->points.at(pp)->a;
            final2 = getPalColor(param->points.at(pp)->r, param->points.at(pp)->g, param->points.at(pp)->b);
        }
    }
    if(final2 == NULL) {
        final2 = getPalColor(param->points.at(darkest)->r, param->points.at(darkest)->g, param->points.at(darkest)->b);
    }
    ffinal.r = final2->r;
    ffinal.g = final2->g;
    ffinal.b = final2->b;
    return ffinal;
}

void updateGrad(Parameter* param) {//remove?
}

void paletteChanged() {
    vector<GLubyte> byteData;
    for(int p = 0; p < 16*16; p++) {
        Color* color = palette.at(0);
        if(p < palette.size()) {
            color = palette.at(p);
        }
        byteData.push_back(GLubyte(color->r));
        byteData.push_back(GLubyte(color->g));
        byteData.push_back(GLubyte(color->b));
    }
    glGenTextures(1, &palImgReal);
    glBindTexture(GL_TEXTURE_2D, palImgReal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, &byteData[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    bool indexed = false;
    if(colorParams.size() > 0) {
        indexed = colorParams.at(7)->value;
    }
    for(int i = 0; i < texs.size(); i++) {
        Texture* t = texs.at(i);
        bool updateTex = false;
        for(int e = 0; e < t->fxs.size(); e++) {
            nEffect* fx = t->fxs.at(e);
            for(int p = 0; p < fx->params.size(); p++) {
                Parameter* param = fx->params.at(p);
                if(param->ID == 0) {
                    if(indexed) {
                        Color* color = palette.at(param->value4);
                        Color hsv = RGBtoHSV(color->r, color->g, color->b);
                        param->value = hsv.r;
                        param->value2 = hsv.g/100.0;
                        param->value3 = hsv.b/100.0;
                    } else {
                        float H = param->value;
                        float S = param->value2;
                        float V = param->value3;
                        Color* final = getPalColor(H, S, V);
                        for(int col = 0; col < palette.size(); col++) {
                            Color* color = palette.at(col);
                            if(final->equals(color)) {
                                param->value4 = col;
                            }
                        }
                    }
                    fx->undone = true;
                    updateTex = true;
                }
                if(param->ID == 7) {
                    if(indexed) {
                        for(int cp = 0; cp < param->points.size(); cp++) {
                            Color* color = palette.at(param->points.at(cp)->i);
                            Color hsv = RGBtoHSV(color->r, color->g, color->b);
                            param->points.at(cp)->r = hsv.r;
                            param->points.at(cp)->g = hsv.g/100.0;
                            param->points.at(cp)->b = hsv.b/100.0;
                        }
                    } else {
                        for(int cp = 0; cp < param->points.size(); cp++) {
                            float H = param->points.at(cp)->r;
                            float S = param->points.at(cp)->g;
                            float V = param->points.at(cp)->b;
                            Color* final = getPalColor(H, S, V);
                            for(int col = 0; col < palette.size(); col++) {
                                Color* color = palette.at(col);
                                if(final->equals(color)) {
                                    param->points.at(cp)->i = col;
                                }
                            }
                        }
                    }
                    updateGrad(param);
                    fx->undone = true;
                    updateTex = true;
                }
            }
        }
        if(updateTex) {
            t->genTex();
        }
    }
}

void loadPalette() {
    for(int toDel = 0; toDel < palette.size(); toDel++) {
        delete palette.at(toDel);
    }
    palette.clear();
    glBindTexture(GL_TEXTURE_2D, palImg);
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    GLubyte *pixels = new GLubyte[w*h*3];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    if(w > 256 || h > 256) {
        if(errorMessage.size() > 0) {
            errorMessage += "\n";
        }
        errorMessage += "Image is too large";
        errorTimer = 160;
        
        palette.push_back(new Color(0, 0, 0));
        paletteChanged();
        selectedColor = 0;
    } else {
        for(int i = 0; i < w*h; i++) {
            Color* color = new Color(pixels[i*3+0], pixels[i*3+1], pixels[i*3+2]);
            bool alreadyIn = false;
            for(int c2 = 0; c2 < palette.size(); c2++) {
                Color* color2 = palette.at(c2);
                if(color->equals(color2)) {
                    alreadyIn = true;
                }
            }
            if(!alreadyIn) {
                palette.push_back(color);
            } else {
                delete color;
            }
        }
        delete [] pixels;
        selectedColor = 0;
        Color* rgb = palette.at(0);
        Color hsv = RGBtoHSV(rgb->r, rgb->g, rgb->b);
        colorParams.at(0)->value = hsv.r;
        colorParams.at(0)->value2 = hsv.g/100.0;
        colorParams.at(0)->value3 = hsv.b/100.0;
        colorParams.at(1)->value = rgb->r;
        colorParams.at(2)->value = rgb->g;
        colorParams.at(3)->value = rgb->b;
        colorParams.at(4)->value = hsv.r;
        colorParams.at(5)->value = hsv.g;
        colorParams.at(6)->value = hsv.b;
        paletteChanged();
    }
}

void report_errors(lua_State *L, int status) { //lua debugging
    if ( status!=0 ) {
        std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1); // remove error message
    }
}

void initLua() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

//LUA - C++ INTERACTION ------------------------

int nEffect::setName(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 0) {
        name = lua_tostring(L, 1);
    }
    return 0;
}

int nEffect::setDesc(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 0) {
        desc = lua_tostring(L, 1);
    }
    return 0;
}

int nEffect::setSize(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 1) {
        w = lua_tonumber(L, 1);
        h = lua_tonumber(L, 2);
    }
    return 0;
}

int nEffect::addInput(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 1) {
        Socket* s = new Socket();
        s->parent = this;
        s->y = lua_tonumber(L, 2)-4;
        s->index = nextP;
        inputs.push_back(s);
        int height = 8;
        Text* t = new Text();
        t->name = lua_tostring(L, 1);
        t->y = lua_tonumber(L, 2)-height/2.0;
        texts.push_back(t);
        nextP++;
    }
    return 0;
}

int nEffect::addParameter(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 5) {
        int offset = 4;
        int height = 14;
        int type = 1;
        if(n > 6) {
            if(lua_toboolean(L, 7)) {
                type = 2;
            }
        }
        Parameter* p = new Parameter(type, lua_tostring(L, 1), offset, lua_tonumber(L, 3)-height/2.0, w-offset*2, height, (int)lua_tonumber(L, 4), (int)lua_tonumber(L, 5), (int)lua_tonumber(L, 6), lua_tostring(L, 2));
        p->index = nextP;
        params.push_back(p);
        nextP++;
    }
    return 0;
}

int nEffect::addInputParameter(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 5) {
        Socket* s = new Socket();
        s->parent = this;
        s->y = lua_tonumber(L, 3)-4;
        s->index = nextP;
        inputs.push_back(s);
        int offset = 4;
        int height = 14;
        int type = 1;
        if(n > 6) {
            if(lua_toboolean(L, 7)) {
                type = 2;
            }
        }
        Parameter* p = new Parameter(type, lua_tostring(L, 1), offset, lua_tonumber(L, 3)-height/2.0, w-offset*2, height, (int)lua_tonumber(L, 4), (int)lua_tonumber(L, 5), (int)lua_tonumber(L, 6), lua_tostring(L, 2));
        p->s = s;
        p->index = nextP;
        params.push_back(p);
        nextP++;
    }
    return 0;
}

int nEffect::addOutput(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 0) {
        Socket* s = new Socket();
        s->parent = this;
        s->y = lua_tonumber(L, 1)-4;
        outputs.push_back(s);
    }
    return 0;
}

int nEffect::addCRamp(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 0) {
        int offset = 0;
        int height = 8;
        int s = 50;
        Parameter* p1 = new Parameter(7, "c", offset, lua_tonumber(L, 1)-height/2.0-1, 108, height, 0, 0, 0, "");
        params.push_back(p1);
        Parameter* p2 = new Parameter(8, "c", 4, lua_tonumber(L, 1)-height/2.0+8+4-1, 8, 8, 0, 0, 0, "");
        p2->p = p1;
        params.push_back(p2);
        Parameter* p3 = new Parameter(9, "c", 4+8+4, lua_tonumber(L, 1)-height/2.0+8+4-1, 8, 8, 0, 0, 0, "");
        p3->p = p1;
        params.push_back(p3);
        Parameter* pc = new Parameter(0, "c", 4+50-s/2.0, lua_tonumber(L, 1)-height/2.0+8+4-1, s, s, 0, 0, 1, "");
        pc->p = p1;
        p1->p = pc;
        params.push_back(pc);
    }
    return 0;
}

int nEffect::setPixel(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 5) {
        Socket* a = outputs.at((int)lua_tonumber(L, 1));
        Color b(lua_tonumber(L, 4)*255, lua_tonumber(L, 5)*255, lua_tonumber(L, 6)*255);
        ::setPixel(a->texData, (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3), &b, true);
    }
    return 0;
}

int nEffect::getValue(lua_State* L) {
    float r = 0;
    float g = 0;
    float b = 0;
    int n = lua_gettop(L);
    if(n > 3) {
        for(int i = 0; i < params.size(); i++) {
            if(params.at(i)->index == (int)lua_tonumber(L, 1)) {
                r = params.at(i)->value/lua_tonumber(L, 4);
                g = params.at(i)->value/lua_tonumber(L, 4);
                b = params.at(i)->value/lua_tonumber(L, 4);
            }
        }
        for(int i = 0; i < inputs.size(); i++) {
            if(inputs.at(i)->index == (int)lua_tonumber(L, 1)) {
                Socket* a = inputs.at(i);
                if(a->s != NULL) {
                    Color cc1 = getColor(a->s->texData, (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3), true);
                    Color cc2 = getColor(a->s->texData, (int)lua_tonumber(L, 2)+1, (int)lua_tonumber(L, 3), true);
                    Color cc3 = getColor(a->s->texData, (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3)+1, true);
                    Color cc4 = getColor(a->s->texData, (int)lua_tonumber(L, 2)+1, (int)lua_tonumber(L, 3)+1, true);
                    float alphaX = lua_tonumber(L, 2)-(int)lua_tonumber(L, 2);
                    float alphaY = lua_tonumber(L, 3)-(int)lua_tonumber(L, 3);
                    float ur = cc1.r*(1.0-alphaX)+cc2.r*alphaX;
                    float ug = cc1.g*(1.0-alphaX)+cc2.g*alphaX;
                    float ub = cc1.b*(1.0-alphaX)+cc2.b*alphaX;
                    float dr = cc3.r*(1.0-alphaX)+cc4.r*alphaX;
                    float dg = cc3.g*(1.0-alphaX)+cc4.g*alphaX;
                    float db = cc3.b*(1.0-alphaX)+cc4.b*alphaX;
                    r = ur*(1.0-alphaY)+dr*alphaY;
                    g = ug*(1.0-alphaY)+dg*alphaY;
                    b = ub*(1.0-alphaY)+db*alphaY;
                    r = r/255.0;
                    g = g/255.0;
                    b = b/255.0;
                }
            }
        }
    }
    lua_pushnumber(L, r);
    lua_pushnumber(L, g);
    lua_pushnumber(L, b);
    return 3;
}

int nEffect::finalize(lua_State* L) {
    for(int i = 0; i < texSizeX; i++) {
        for(int j = 0; j < texSizeY; j++) {
            Color c(0, 0, 0);
            if(inputs.at(0)->s != NULL) {
                c = getColor(inputs.at(0)->s->texData, i, j, true);
            }
            int value = c.r*99.0/255.0;
            Color a = getGrad(params.at(0), value);
            ::setPixel(outputs.at(0)->texData, i, j, &a, true);
        }
    }
    return 0;
}

int nEffect::getTileSize(lua_State* L) {
    lua_pushnumber(L, texSizeX);
    return 1;
}

int nEffect::addNode(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 2) {
        string name = "";
        for(int i = 0; i < newEffects.size(); i++) {
            if(newEffects.at(i)->fxname == lua_tostring(L, 1)) {
                name = newEffects.at(i)->luafn;
            }
        }
        if(name.size() > 0) {
            nEffect* fx = new nEffect(name, lua_tostring(L, 1));
            fx->x += lua_tonumber(L, 2);
            fx->y += lua_tonumber(L, 3);
            presetFxs.push_back(fx);
        } else {
            if(!presetError) {
                errorMessage = "";
                presetError = true;
            }
            if(errorMessage.size() > 0) {
                errorMessage += "\n";
            }
            errorMessage += "Unable to load \""+(string)lua_tostring(L, 1)+"\"";
            errorTimer = 160;
        }
    }
    return 0;
}

int nEffect::setParameter(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 2) {
        if(!presetError) {
            nEffect* fx = presetFxs.at((int)lua_tonumber(L, 1));
            Parameter* param = fx->params.at((int)lua_tonumber(L, 2));
            param->value = (int)lua_tonumber(L, 3);
        }
    }
    return 0;
}

int nEffect::addConnection(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 3) {
        if(!presetError) {
            nEffect* fx1 = presetFxs.at((int)lua_tonumber(L, 1));
            nEffect* fx2 = presetFxs.at((int)lua_tonumber(L, 3));
            Socket* output = fx1->outputs.at((int)lua_tonumber(L, 2));
            Socket* input = fx2->inputs.at((int)lua_tonumber(L, 4));
            input->s = output;
        }
    }
    return 0;
}

int my_rand(unsigned int *nextp) { //custom RNG for consistency between OSes
    *nextp = *nextp * 1103515245 + 12345;
    return (unsigned int)(*nextp / 65536) % 32768;
}

int nEffect::luaSeed(lua_State* L) {
    int n = lua_gettop(L);
    if(n > 0) {
        seed = (unsigned int)lua_tonumber(L, 1);
        my_rand(&seed);
    }
    return 0;
}

int nEffect::luaRand(lua_State* L) {
    int rm = 32767;
    float r = my_rand(&seed)%rm/(float)rm;
    switch (lua_gettop(L)) {
        case 0: {
            lua_pushnumber(L, r);
            break;
        }
        case 1: {
            int u = (int)lua_tonumber(L, 1);
            luaL_argcheck(L, 1<=u, 1, "interval is empty");
            lua_pushnumber(L, floor(r*u)+1);
            break;
        }
        case 2: {
            int l = (int)lua_tonumber(L, 1);
            int u = (int)lua_tonumber(L, 2);
            luaL_argcheck(L, l<=u, 2, "interval is empty");
            lua_pushnumber(L, floor(r*(u-l+1))+l);
            break;
        }
        default: return luaL_error(L, "wrong number of arguments");
    }
    return 1;
}

void nEffect::AbortLua(lua_State* L, lua_Debug* ar) {
    if(abort) {
        luaL_error(L, "Too Many Lines Error");
    }
}

//-----------------------------------

#ifdef _WIN32
void importFxs() {
    char cwd[1024];
    uint32_t size = sizeof(cwd);
    GetModuleFileName(NULL, cwd, size);
    string cwd2 = string(cwd);
    cwd2.erase(cwd2.rfind('\\'));
    if(cwd2.size() < 1) {
        cwd2 = "\\Nodes";
    } else {
        cwd2 += "\\Nodes";
    }
    DIR* dirr;
    struct dirent *ent;
    vector<File*> temp;
    if((dirr = opendir(cwd2.c_str())) != NULL) {
        while((ent = readdir(dirr)) != NULL) {
            if(ent->d_name[0] != '.') {
                string fn = ent->d_name;
                if(fn.substr(fn.find_last_of(".") + 1) == "lua") {
                    string fullfn = cwd2 + "\\" + fn;
                    lua_settop(L, 0);
                    lua_pushnil(L);
                    lua_setglobal(L, "init");
                    lua_settop(L, 0);
                    lua_pushnil(L);
                    lua_setglobal(L, "apply");
                    int s = luaL_dofile(L, fullfn.c_str());
                    if(s == 0) {
                        //cout << "Loaded" << endl;
                        //checking if everything works
                        bool OK = true;
                        lua_getglobal(L, "init");
                        lua_getglobal(L, "apply");
                        if (!lua_isfunction(L, -2)) {
                            OK = false;
                        }
                        if (!lua_isfunction(L, -1)) {
                            OK = false;
                        }
                        if(OK) {
                            //cout << "Success" << endl;
                            newEffects.push_back(new nEffect(fullfn, fn));
                        }
                    }
                }
            }
        }
        closedir(dirr);
    } else {
    }
}

void importPresets() {
    char cwd[1024];
    uint32_t size = sizeof(cwd);
    GetModuleFileName(NULL, cwd, size);
    string cwd2 = string(cwd);
    cwd2.erase(cwd2.rfind('\\'));
    if(cwd2.size() < 1) {
        cwd2 = "\\Presets";
    } else {
        cwd2 += "\\Presets";
    }
    DIR* dirr;
    struct dirent *ent;
    vector<File*> temp;
    if((dirr = opendir(cwd2.c_str())) != NULL) {
        while((ent = readdir(dirr)) != NULL) {
            if(ent->d_name[0] != '.') {
                string fn = ent->d_name;
                if(fn.substr(fn.find_last_of(".") + 1) == "lua") {
                    string fullfn = cwd2 + "\\" + fn;
                    lua_settop(L, 0);
                    lua_pushnil(L);
                    lua_setglobal(L, "init");
                    lua_settop(L, 0);
                    lua_pushnil(L);
                    lua_setglobal(L, "apply");
                    int s = luaL_dofile(L, fullfn.c_str());
                    if(s == 0) {
                        //cout << "Loaded" << endl;
                        //checking if everything works
                        bool OK = true;
                        lua_getglobal(L, "init");
                        lua_getglobal(L, "apply");
                        if (!lua_isfunction(L, -2)) {
                            OK = false;
                        }
                        if (!lua_isfunction(L, -1)) {
                            OK = false;
                        }
                        if(OK) {
                            //cout << "Success" << endl;
                            newEffects.push_back(new nEffect(fullfn, fn, true));
                        }
                    }
                }
            }
        }
        closedir(dirr);
    } else {
    }
}
#elif __APPLE__
void importFxs() {
    char cwd[1024];
    uint32_t size = sizeof(cwd);
    _NSGetExecutablePath(cwd, &size);
    string cwd2 = string(cwd);
    cwd2.erase(cwd2.rfind('/'));
    cwd2.erase(cwd2.rfind('/'));
    cwd2.erase(cwd2.rfind('/'));
    cwd2.erase(cwd2.rfind('/'));
    if(cwd2.size() < 1) {
        cwd2 = "/Nodes";
    } else {
        cwd2 += "/Nodes";
    }

    int num_entries;
    struct dirent **entries = NULL;
    num_entries = scandir(cwd2.c_str(), &entries, NULL, NULL);

    for(int i = 0; i < num_entries; i++) {
        if(entries[i]->d_name[0] != '.') {
            string fn = entries[i]->d_name;
            if(fn.substr(fn.find_last_of(".") + 1) == "lua") {
                string fullfn = cwd2 + "/" + fn;
                lua_settop(L, 0);
                lua_pushnil(L);
                lua_setglobal(L, "init");
                lua_settop(L, 0);
                lua_pushnil(L);
                lua_setglobal(L, "apply");
                int s = luaL_dofile(L, fullfn.c_str());
                if(s == 0) {
                    //cout << "Loaded" << endl;
                    //checking if everything works
                    bool OK = true;
                    lua_getglobal(L, "init");
                    lua_getglobal(L, "apply");
                    if (!lua_isfunction(L, -2)) {
                        OK = false;
                    }
                    if (!lua_isfunction(L, -1)) {
                        OK = false;
                    }
                    if(OK) {
                        //cout << "Success" << endl;
                        newEffects.push_back(new nEffect(fullfn, fn));
                    }
                }
            }
        }
    }
}
       void importPresets() {
           char cwd[1024];
           uint32_t size = sizeof(cwd);
           _NSGetExecutablePath(cwd, &size);
           string cwd2 = string(cwd);
           cwd2.erase(cwd2.rfind('/'));
           cwd2.erase(cwd2.rfind('/'));
           cwd2.erase(cwd2.rfind('/'));
           cwd2.erase(cwd2.rfind('/'));
           if(cwd2.size() < 1) {
               cwd2 = "/Presets";
           } else {
               cwd2 += "/Presets";
           }

           int num_entries;
           struct dirent **entries = NULL;
           num_entries = scandir(cwd2.c_str(), &entries, NULL, NULL);

           for(int i = 0; i < num_entries; i++) {
               if(entries[i]->d_name[0] != '.') {
                   string fn = entries[i]->d_name;
                   if(fn.substr(fn.find_last_of(".") + 1) == "lua") {
                       string fullfn = cwd2 + "/" + fn;
                       lua_settop(L, 0);
                       lua_pushnil(L);
                       lua_setglobal(L, "init");
                       lua_settop(L, 0);
                       lua_pushnil(L);
                       lua_setglobal(L, "apply");
                       int s = luaL_dofile(L, fullfn.c_str());
                       if(s == 0) {
                           //cout << "Loaded" << endl;
                           //checking if everything works
                           bool OK = true;
                           lua_getglobal(L, "init");
                           lua_getglobal(L, "apply");
                           if (!lua_isfunction(L, -2)) {
                               OK = false;
                           }
                           if (!lua_isfunction(L, -1)) {
                               OK = false;
                           }
                           if(OK) {
                               //cout << "Success" << endl;
                               newEffects.push_back(new nEffect(fullfn, fn, true));
                           }
                       }
                   }
               }
           }
       }
#elif __linux__
#else
#endif

Socket::~Socket() {
    delete b;
}

nEffect::~nEffect() {
    for(int toDel = 0; toDel < params.size(); toDel++) {
        delete params.at(toDel);
    }
    for(int toDel = 0; toDel < inputs.size(); toDel++) {
        delete inputs.at(toDel);
    }
    for(int toDel = 0; toDel < outputs.size(); toDel++) {
        delete outputs.at(toDel);
    }
    for(int toDel = 0; toDel < texts.size(); toDel++) {
        delete texts.at(toDel);
    }
}

//wizardry shit
typedef int (nEffect::*mem_func)(lua_State * L);
template <mem_func func>
int dispatch(lua_State * L) {
    nEffect * ptr = *static_cast<nEffect**>(lua_getextraspace(L));
    return ((*ptr).*func)(L);
}

//what the fuck?
typedef void (nEffect::*mem_func2)(lua_State * L, lua_Debug *ar);
template <mem_func2 func2>
void dispatch2(lua_State * L, lua_Debug * ar) {
    nEffect * ptr = *static_cast<nEffect**>(lua_getextraspace(L));
    return ((*ptr).*func2)(L, ar);
}

//need to override ALL lua math functions just because of the custom RNG
static int math_abs (lua_State *L) {
    lua_pushnumber(L, l_mathop(fabs)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_sin (lua_State *L) {
    lua_pushnumber(L, l_mathop(sin)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_sinh (lua_State *L) {
    lua_pushnumber(L, l_mathop(sinh)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_cos (lua_State *L) {
    lua_pushnumber(L, l_mathop(cos)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_cosh (lua_State *L) {
    lua_pushnumber(L, l_mathop(cosh)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_tan (lua_State *L) {
    lua_pushnumber(L, l_mathop(tan)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_tanh (lua_State *L) {
    lua_pushnumber(L, l_mathop(tanh)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_asin (lua_State *L) {
    lua_pushnumber(L, l_mathop(asin)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_acos (lua_State *L) {
    lua_pushnumber(L, l_mathop(acos)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_atan (lua_State *L) {
    lua_pushnumber(L, l_mathop(atan)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_atan2 (lua_State *L) {
    lua_pushnumber(L, l_mathop(atan2)(luaL_checknumber(L, 1),
                                      luaL_checknumber(L, 2)));
    return 1;
}

static int math_ceil (lua_State *L) {
    lua_pushnumber(L, l_mathop(ceil)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_floor (lua_State *L) {
    lua_pushnumber(L, l_mathop(floor)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_fmod (lua_State *L) {
    lua_pushnumber(L, l_mathop(fmod)(luaL_checknumber(L, 1),
                                     luaL_checknumber(L, 2)));
    return 1;
}

static int math_modf (lua_State *L) {
    lua_Number ip;
    lua_Number fp = l_mathop(modf)(luaL_checknumber(L, 1), &ip);
    lua_pushnumber(L, ip);
    lua_pushnumber(L, fp);
    return 2;
}

static int math_sqrt (lua_State *L) {
    lua_pushnumber(L, l_mathop(sqrt)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_pow (lua_State *L) {
    lua_Number x = luaL_checknumber(L, 1);
    lua_Number y = luaL_checknumber(L, 2);
    lua_pushnumber(L, l_mathop(pow)(x, y));
    return 1;
}

static int math_log (lua_State *L) {
    lua_Number x = luaL_checknumber(L, 1);
    lua_Number res;
    if (lua_isnoneornil(L, 2))
        res = l_mathop(log)(x);
    else {
        lua_Number base = luaL_checknumber(L, 2);
        if (base == (lua_Number)10.0) res = l_mathop(log10)(x);
        else res = l_mathop(log)(x)/l_mathop(log)(base);
    }
    lua_pushnumber(L, res);
    return 1;
}

#if defined(LUA_COMPAT_LOG10)
static int math_log10 (lua_State *L) {
    lua_pushnumber(L, l_mathop(log10)(luaL_checknumber(L, 1)));
    return 1;
}
#endif

static int math_exp (lua_State *L) {
    lua_pushnumber(L, l_mathop(exp)(luaL_checknumber(L, 1)));
    return 1;
}

static int math_deg (lua_State *L) {
    lua_pushnumber(L, luaL_checknumber(L, 1)/(M_PI/180.0));
    return 1;
}

static int math_rad (lua_State *L) {
    lua_pushnumber(L, luaL_checknumber(L, 1)*(M_PI/180.0));
    return 1;
}

static int math_frexp (lua_State *L) {
    int e;
    lua_pushnumber(L, l_mathop(frexp)(luaL_checknumber(L, 1), &e));
    lua_pushinteger(L, e);
    return 2;
}

static int math_ldexp (lua_State *L) {
    lua_Number x = luaL_checknumber(L, 1);
    int ep = (int)luaL_checknumber(L, 2);
    lua_pushnumber(L, l_mathop(ldexp)(x, ep));
    return 1;
}


static int math_min (lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    lua_Number dmin = luaL_checknumber(L, 1);
    int i;
    for (i=2; i<=n; i++) {
        lua_Number d = luaL_checknumber(L, i);
        if (d < dmin)
            dmin = d;
    }
    lua_pushnumber(L, dmin);
    return 1;
}

static int math_max (lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    lua_Number dmax = luaL_checknumber(L, 1);
    int i;
    for (i=2; i<=n; i++) {
        lua_Number d = luaL_checknumber(L, i);
        if (d > dmax)
            dmax = d;
    }
    lua_pushnumber(L, dmax);
    return 1;
}

static const luaL_Reg mathlib[] = {
    {"abs",   math_abs},
    {"acos",  math_acos},
    {"asin",  math_asin},
    {"atan2", math_atan2},
    {"atan",  math_atan},
    {"ceil",  math_ceil},
    {"cosh",   math_cosh},
    {"cos",   math_cos},
    {"deg",   math_deg},
    {"exp",   math_exp},
    {"floor", math_floor},
    {"fmod",   math_fmod},
    {"frexp", math_frexp},
    {"ldexp", math_ldexp},
#if defined(LUA_COMPAT_LOG10)
    {"log10", math_log10},
#endif
    {"log",   math_log},
    {"max",   math_max},
    {"min",   math_min},
    {"modf",   math_modf},
    {"pow",   math_pow},
    {"rad",   math_rad},
    {"random",     &dispatch<&nEffect::luaRand>},
    {"randomseed", &dispatch<&nEffect::luaSeed>},
    {"sinh",   math_sinh},
    {"sin",   math_sin},
    {"sqrt",  math_sqrt},
    {"tanh",   math_tanh},
    {"tan",   math_tan},
    {NULL, NULL}
};

LUAMOD_API int luaopen_math2 (lua_State *L) {
    luaL_newlib(L, mathlib);
    lua_pushnumber(L, M_PI);
    lua_setfield(L, -2, "pi");
    lua_pushnumber(L, HUGE_VAL);
    lua_setfield(L, -2, "huge");
    return 1;
}

LUALIB_API void luaL_requiref2 (lua_State *L, const char *modname,
                               lua_CFunction openf, int glb) {
    lua_pushcfunction(L, openf);
    lua_pushstring(L, modname);  /* argument to open function */
    lua_call(L, 1, 1);  /* open module */
    luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED");
    lua_pushvalue(L, -2);  /* make copy of module (call result) */
    lua_setfield(L, -2, modname);  /* _LOADED[modname] = module */
    lua_pop(L, 1);  /* remove _LOADED table */
    if (glb) {
        lua_pushvalue(L, -1);  /* copy of 'mod' */
        lua_setglobal(L, modname);  /* _G[modname] = module */
    }
}

nEffect::nEffect(string luafn, string fxname, bool preset) {
    this->isPreset = preset;
    this->fxname = fxname;
    this->nextP = 0;
    this->cleared = false;
    this->abort = false;
    this->deleted = false;
    this->loading = false;
    this->loaded = false;
    this->done = false;
    this->undone = true;
    this->L = luaL_newstate();
    luaL_openlibs(this->L);
    *static_cast<nEffect**>(lua_getextraspace(L)) = this;
    if(!isPreset) {
        lua_register(this->L, "setName", &dispatch<&nEffect::setName>);
        lua_register(this->L, "setDesc", &dispatch<&nEffect::setDesc>);
        lua_register(this->L, "setSize", &dispatch<&nEffect::setSize>);
        lua_register(this->L, "addInput", &dispatch<&nEffect::addInput>);
        lua_register(this->L, "addParameter", &dispatch<&nEffect::addParameter>);
        lua_register(this->L, "addInputParameter", &dispatch<&nEffect::addInputParameter>);
        lua_register(this->L, "addOutput", &dispatch<&nEffect::addOutput>);
        lua_register(this->L, "getTileSize", &dispatch<&nEffect::getTileSize>);
        lua_register(this->L, "getValue", &dispatch<&nEffect::getValue>);
        lua_register(this->L, "setPixel", &dispatch<&nEffect::setPixel>);

        lua_register(this->L, "addCRamp", &dispatch<&nEffect::addCRamp>);
        lua_register(this->L, "colorize", &dispatch<&nEffect::finalize>);
    } else {
        lua_register(this->L, "setName", &dispatch<&nEffect::setName>);
        lua_register(this->L, "setDesc", &dispatch<&nEffect::setDesc>);
        lua_register(this->L, "addNode", &dispatch<&nEffect::addNode>);
        lua_register(this->L, "setParameter", &dispatch<&nEffect::setParameter>);
        lua_register(this->L, "addConnection", &dispatch<&nEffect::addConnection>);
    }

    luaL_requiref2(this->L, "math", luaopen_math2, 1);

    lua_sethook(this->L, &dispatch2<&nEffect::AbortLua>, LUA_MASKCOUNT, 2);
    this->luafn = luafn;
    name = "";
    desc = "";
    sx = 0;
    sy = 0;
    w = 32;
    h = 32;
    r = 0;
    luaL_dofile(this->L, luafn.c_str());
    lua_settop(this->L, 0);
    lua_getglobal(this->L, "init");
    lua_pcall(this->L, 0, 0, 0);
    x = -w/2+screenW/2.0-nodeCX;
    y = -h/2+screenH/2.0-nodeCY;
}

void getHome();

void loadGen() {
    texSizeX = 32;
    texSizeY = texSizeX;
    texType = 0;

    nodeCX = 0;
    nodeCY = 0;
    /*palette.push_back(new Color(67, 44, 59));
    palette.push_back(new Color(95, 59, 52));
    palette.push_back(new Color(134, 87, 61));
    palette.push_back(new Color(174, 121, 81));*/

    gridImg = loadTexture("grid.png");
    effectImg = loadTexture("effect0.png");
    effectImg2 = loadTexture("effect1.png");
    effectImg3 = loadTexture("effect2.png");
    effectImg4 = loadTexture("effect3.png");
    effectImg5 = loadTexture("effect4.png");
    effectImg6 = loadTexture("effect5.png");
    effectImg7 = loadTexture("effect6.png");
    effectImg8 = loadTexture("effect7.png");
    effectImg9 = loadTexture("effect8.png");
    effectImg10 = loadTexture("effect9.png");
    effectImg11 = loadTexture("effect10.png");
    effectImg12 = loadTexture("effect11.png");
    effectImg13 = loadTexture("effect12.png");
    effectImg14 = loadTexture("effect13.png");
    effectImg15 = loadTexture("effect14.png");
    effectImg16 = loadTexture("effect15.png");
    effectImg17 = loadTexture("effect16.png");
    bezierFill = loadTexture("bezierFill.png");
    bezierFillError = loadTexture("bezierFillError.png");
    iconImg0 = loadTexture("icon0.png");
    iconImg1 = loadTexture("icon1.png");
    iconImg2 = loadTexture("icon2.png");
    iconImg3 = loadTexture("icon3.png");
    iconImg4 = loadTexture("icon5.png");
    iconImg5 = loadTexture("icon4.png");
    iconImg6 = loadTexture("icon6.png");
    iconImg7 = loadTexture("icon7.png");
    iconImg8 = loadTexture("icon8.png");
    iconImg9 = loadTexture("icon9.png");
    iconImg10 = loadTexture("icon10.png");
    iconImg11 = loadTexture("icon11.png");
    iconImg12 = loadTexture("icon12.png");
    iconImg13 = loadTexture("icon13.png");
    palImg = loadTexture("pal.png");
    fontImg = loadTexture("font.png");
    fontImg2 = loadTexture("font2.png");
    fontImg3 = loadTexture("font3.png");

    glGenTextures(1, &screenTexFinal2);
    glBindTexture(GL_TEXTURE_2D, screenTexFinal2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenFramebuffers(1, &screenFboFinal2);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFboFinal2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexFinal2, 0);
    GLenum d2[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, d2);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    barY = screenH-150;
    barX = 128+7;
    barXRight = barX;
    barY3 = expH;

    getHome();

    currentTexture = 0;
    expandedLayer = 0;

    Texture* t = new Texture();
    texs.push_back(t);

    initLua();

    //IMPORT FXS
    importFxs();
    importPresets();

    t->genTex();

    int space = 4;
    int s = 36;
    int s2 = (barXRight-7-space*4)-s;
    int paramSize = s2/2.0;
    int paramH = s/3.0;
    int title = collH-4;
    int offset = (paramH-7)/4.0;
    colorParams.push_back(new Parameter(4, "c", space, space, s, s, 0, 0, 1, "Color"));

    colorParams.push_back(new Parameter(1, "R", space+s+space+offset, space+offset, paramSize-offset*2, paramH, 0, 0, 255, "Red component"));
    colorParams.push_back(new Parameter(1, "G", space+s+space+offset, space+s/2.0-(paramH)/2.0, paramSize-offset*2, paramH, 0, 0, 255, "Green component"));
    colorParams.push_back(new Parameter(1, "B", space+s+space+offset, space+s-(paramH)-offset, paramSize-offset*2, paramH, 0, 0, 255, "Blue component"));
    colorParams.push_back(new Parameter(1, "H", space+s+space*2+offset+paramSize, space+offset, paramSize-offset*2, paramH, 0, 0, 360, "Hue component"));
    colorParams.push_back(new Parameter(1, "S", space+s+space*2+offset+paramSize, space+s/2.0-(paramH)/2.0, paramSize-offset*2, paramH, 0, 0, 100, "Saturation component"));
    colorParams.push_back(new Parameter(1, "V", space+s+space*2+offset+paramSize, space+s-(paramH)-offset, paramSize-offset*2, paramH, 0, 0, 100, "Value component"));
    barY2 = barY3+2+space*3+s+title+12;
    int c = 8;
    int w = barXRight-6+space-offset*2;
    float size = w/float(c);
    colorParams.push_back(new Parameter(3, "Indexed", space, space*2+s, barXRight-7-space*2, title, 0, 0, 1, "Indexed mode"));
    colorParams.push_back(new Parameter(1, "Size", space, space*2+s+3+9+2+14+4, barXRight-7-space*2, title, texSizeX, 1, 64, "Tile size"));
    barY4 = barY3+2+12+space*2+s+3+9+2+14+4+title+space;
    barY5 = barY4+3+3+8+4+96;

    loadPalette();
}

void initGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    proj = glm::ortho(0.0, (double)screenW*screenScale, (double)screenH*screenScale, 0.0, -1.0, 1.0);
}

File::File(string n, bool f) {
    name = n;
    folder = f;
}

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

void newFile() {
    listUndo.clear();
    listRedo.clear();
    nodeCX = 0;
    nodeCY = 0;
    nodeCSX = 0;
    nodeCSY = 0;
    texSizeX = 32;
    texSizeY = texSizeX;
    glGenTextures(1, &screenTexFinal2);
    glBindTexture(GL_TEXTURE_2D, screenTexFinal2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenFramebuffers(1, &screenFboFinal2);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFboFinal2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexFinal2, 0);
    GLenum d2[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, d2);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    for(int toDel = 0; toDel < texs.size(); toDel++) {
        delete texs.at(toDel);
    }
    texs.clear();
    Texture* t = new Texture();
    texs.push_back(t);

    currentTexture = 0;
    for(int toDel = 0; toDel < palette.size(); toDel++) {
        delete palette.at(toDel);
    }
    palette.clear();
    palette.push_back(new Color(0, 0, 0));
    paletteChanged();
    selectedColor = 0;
    zoom = 1;
    for(int i = 0; i < colorParams.size(); i++) {
        colorParams.at(i)->value = colorParams.at(i)->resetValue;
    }
    colorParams.at(0)->value = 0;
    colorParams.at(0)->value2 = 0;
    colorParams.at(0)->value3 = 0;
    lastPalDir = "";
    lastPalName = "";
    lastTexDir = "";
    lastTexName = "";
    lastSaveDir = "";
    lastSaveName = "";
}

void loadStuff(string str, bool newFile) {
    istringstream fin(str);
    if(fin.good()) {
        string line;
        getline(fin, line);
        vector<string> a = split(line, ' ');
        bool error = false;
        errorMessage = "";
        if(a.at(0) == "Tilemancer") { //valid
            vector<string> b = split(a.at(1), '.');
            int fileMj = atoi(b.at(0).c_str());
            int fileMn = atoi(b.at(1).c_str());
            int fileRv = atoi(b.at(2).c_str());
            int comp = 0; //equal
            if(fileMj > Vmajor) {
                comp = 1; //opening file saved on newer version
            } else if(fileMj < Vmajor) {
                comp = -1; //opening old save file
            } else {
                if(fileMn > Vminor) {
                    comp = 1; //opening file saved on newer version
                } else if(fileMn < Vminor) {
                    comp = -1; //opening old save file
                } else {
                    if(fileRv > Vrevision) {
                        comp = 1; //opening file saved on newer version
                    } else if(fileRv < Vrevision) {
                        comp = -1; //opening old save file
                    } else {
                        //equal
                    }
                }
            }
            for(int toDel = 0; toDel < texs.size(); toDel++) {
                delete texs.at(toDel);
            }
            texs.clear();
            Texture* t = new Texture();
            texs.push_back(t);
            if(newFile) {
                currentTexture = 0;
            }
            for(int toDel = 0; toDel < palette.size(); toDel++) {
                delete palette.at(toDel);
            }
            palette.clear();
            if(newFile) {
                selectedColor = 0;
            }
            getline(fin, line);
            while(getline(fin, line)) {
                a.clear();
                a = split(line, ' ');
                if(a.size() > 0) {
                    if(a.at(0) == "Color") {
                        Color* c = new Color(atoi(a.at(1).c_str()), atoi(a.at(2).c_str()), atoi(a.at(3).c_str()));
                        palette.push_back(c);
                    }
                    if(a.at(0) == "PaletteSave1") {
                        if(a.size() > 1) {
                            lastPalDir = a.at(1);
                        } else {
                            lastPalDir = "";
                        }
                    }
                    if(a.at(0) == "PaletteSave2") {
                        if(a.size() > 1) {
                            lastPalName = a.at(1);
                        } else {
                            lastPalName = "";
                        }
                    }
                    if(a.at(0) == "FileSave1") {
                        if(a.size() > 1) {
                            lastSaveDir = a.at(1);
                        } else {
                            lastSaveDir = "";
                        }
                    }
                    if(a.at(0) == "FileSave2") {
                        if(a.size() > 1) {
                            lastSaveName = a.at(1);
                        } else {
                            lastSaveName = "";
                        }
                    }
                    if(a.at(0) == "Indexed") {
                        colorParams.at(7)->value = atoi(a.at(1).c_str());
                    }
                    if(a.at(0) == "Size") {
                        colorParams.at(8)->value = atoi(a.at(1).c_str());
                        texSizeX = colorParams.at(8)->value;
                        texSizeY = texSizeX;
                    }
                    if(a.at(0) == "Camera") {
                        nodeCX = atoi(a.at(1).c_str());
                        nodeCY = atoi(a.at(2).c_str());
                        nodeCSX = 0;
                        nodeCSY = 0;
                    }
                    if(a.at(0) == "Node") {
                        getline(fin, line);
                        a.clear();
                        a = split(line, ' ');
                        string name = "";
                        for(int i = 0; i < newEffects.size(); i++) {
                            if(newEffects.at(i)->fxname == a.at(1)) {
                                name = newEffects.at(i)->luafn;
                            }
                        }
                        if(name.size() > 0) {
                            nEffect* fx = new nEffect(name, a.at(1));
                            bool nodeFinished = false;
                            int param = 0;
                            while(!nodeFinished) {
                                getline(fin, line);
                                a.clear();
                                a = split(line, ' ');
                                if(a.at(0) == "}") {
                                    nodeFinished = true;
                                    texs.at(currentTexture)->fxs.push_back(fx);
                                }
                                if(a.at(0) == "Position") {
                                    fx->x = atof(a.at(1).c_str());
                                    fx->y = atof(a.at(2).c_str());
                                }
                                if(a.at(0) == "Ramp") {
                                    fx->params.at(param)->points.clear();
                                    bool rampDone = false;
                                    while(!rampDone) {
                                        getline(fin, line);
                                        a.clear();
                                        a = split(line, ' ');
                                        if(a.at(0) == "}") {
                                            rampDone = true;
                                            param++;
                                        }
                                        if(a.at(0) == "Point") {
                                            CPoint* cp = new CPoint();
                                            cp->a = atoi(a.at(1).c_str())/100.0;
                                            cp->r = atof(a.at(2).c_str());
                                            cp->g = atof(a.at(3).c_str());
                                            cp->b = atof(a.at(4).c_str());
                                            fx->params.at(param)->points.push_back(cp);
                                        }
                                    }
                                }
                                if(a.at(0) == "Parameter") {
                                    fx->params.at(param)->value = atoi(a.at(1).c_str());
                                    fx->params.at(param)->value2 = atoi(a.at(2).c_str());
                                    fx->params.at(param)->value3 = atoi(a.at(3).c_str());
                                    fx->params.at(param)->value4 = atoi(a.at(4).c_str());
                                    param++;
                                }
                                if(a.at(0) == "Connection") {
                                    fx->inputs.at(atoi(a.at(1).c_str()))->futureN = atoi(a.at(2).c_str());
                                    fx->inputs.at(atoi(a.at(1).c_str()))->futureS = atoi(a.at(3).c_str());
                                }
                            }
                        } else {
                            if(errorMessage.size() > 0) {
                                errorMessage += "\n";
                            }
                            errorMessage += "Unable to load \""+a.at(1)+"\"";
                            errorTimer = 160;
                            error = true;
                        }
                    }
                }
            }
            for(int i = 0; i < texs.at(currentTexture)->fxs.size(); i++) {
                nEffect* fx = texs.at(currentTexture)->fxs.at(i);
                for(int in = 0; in < fx->inputs.size(); in++) {
                    Socket* input = fx->inputs.at(in);
                    if(input->futureN != -1) {
                        input->s = texs.at(currentTexture)->fxs.at(input->futureN)->outputs.at(input->futureS);
                    }
                }
            }
            if(currentTexture >= texs.size()) {
                currentTexture = texs.size()-1;
            }
            if(selectedColor >= palette.size()) {
                selectedColor = palette.size()-1;
            }
            Color* rgb = palette.at(selectedColor);
            Color hsv = RGBtoHSV(rgb->r, rgb->g, rgb->b);
            colorParams.at(0)->value = hsv.r;
            colorParams.at(0)->value2 = hsv.g/100.0;
            colorParams.at(0)->value3 = hsv.b/100.0;
            colorParams.at(1)->value = rgb->r;
            colorParams.at(2)->value = rgb->g;
            colorParams.at(3)->value = rgb->b;
            colorParams.at(4)->value = hsv.r;
            colorParams.at(5)->value = hsv.g;
            colorParams.at(6)->value = hsv.b;
            paletteChanged();
            glGenTextures(1, &screenTexFinal2);
            glBindTexture(GL_TEXTURE_2D, screenTexFinal2);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glGenFramebuffers(1, &screenFboFinal2);
            glBindFramebuffer(GL_FRAMEBUFFER, screenFboFinal2);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexFinal2, 0);
            GLenum d2[] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, d2);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            for(int i = 0; i < texs.size(); i++) {
                Texture* t = texs.at(i);
                t->genTex();
            }
        } else {
            if(errorMessage.size() > 0) {
                errorMessage += "\n";
            }
            errorMessage += "Invalid file";
            errorTimer = 160;
            error = true;
        }
        fin.clear();
        if(error) {
            ::newFile();
        }
    }
}

void loadFile(string dir) {
    ifstream fin(dir);
    stringstream buffer;
    buffer << fin.rdbuf();
    loadStuff(buffer.str(), true);
}

string saveStuff() {
    ostringstream out;
    out << "Tilemancer " << Vmajor << "." << Vminor << "." << Vrevision << endl;
    out << endl;
    for(int i = 0; i < palette.size(); i++) {
        Color* c = palette.at(i);
        out << "Color " << c->r << " " << c->g << " " << c->b << endl;
    }
    out << "PaletteSave1 " << lastPalDir << endl;
    out << "PaletteSave2 " << lastPalName << endl;
    out << "FileSave1 " << lastSaveDir << endl;
    out << "FileSave2 " << lastSaveName << endl;
    out << "Indexed " << colorParams.at(7)->value << endl;
    out << "Size " << colorParams.at(8)->value << endl;
    out << "Camera " << nodeCX << " " << nodeCY << endl;
    Texture* t = texs.at(currentTexture);
    for(int i = 0; i < t->fxs.size(); i++) {
        nEffect* fx = t->fxs.at(i);
        out << "Node {" << endl;
        out << "Name " << fx->fxname << endl;
        out << "Position " << fx->x << " " << fx->y << endl;
        for(int p = 0; p < fx->params.size(); p++) {
            Parameter* param = fx->params.at(p);
            if(param->ID == 7) {
                out << "Ramp {" << endl;
                for(int p = 0; p < param->points.size(); p++) {
                    out << "Point " << param->points.at(p)->a*100 << " " << param->points.at(p)->r << " " << param->points.at(p)->g << " " << param->points.at(p)->b << endl;
                }
                out << "}" << endl;
            } else {
                out << "Parameter " << param->value << " " << param->value2 << " " << param->value3 << " " << param->value4 << endl;
            }
        }
        for(int in = 0; in < fx->inputs.size(); in++) {
            Socket* input = fx->inputs.at(in);
            if(input->s != NULL) {
                int nodeIndex = find(t->fxs.begin(), t->fxs.end(), input->s->parent) - t->fxs.begin();
                int socketIndex = find(input->s->parent->outputs.begin(), input->s->parent->outputs.end(), input->s) - input->s->parent->outputs.begin();
                out << "Connection " << in << " " << nodeIndex << " " << socketIndex << endl;
            }
        }
        out << "}" << endl;
    }
    return out.str();
}

void undo() {
    if(listUndo.size() > 0) {
        listRedo.push_back(saveStuff());
        loadStuff(listUndo.back(), false);
        listUndo.pop_back();
    }
}

void redo() {
    if(listRedo.size() > 0) {
        //cout << listRedo.size() << endl;
        listUndo.push_back(saveStuff());
        loadStuff(listRedo.back(), false);
        listRedo.pop_back();
    }
}

void saveUndo() {
    listUndo.push_back(saveStuff());
    listRedo.clear();
}

void saveFile(string dir) {
    /*if(dir.substr(dir.find_last_of(".") + 1) != "mancer") {
        dir += ".mancer";
    }*/
    ofstream out(dir.c_str());
    out << saveStuff();
}

void browserAction(string dir, string subDir, string parent) {
    bool exists = false;
    for(int i = 0; i < filenames.size(); i++) {
        if(!strcmp(filenames.at(i)->name.c_str(), subDir.c_str())) {
            exists = true;
        }
    }
    if(browserMode == 0) {
        if(exists) {
            if(OS == 0) {
                palImg = loadTexture2(dir);
            } else if(OS == 1) {
                palImg = loadTexture(dir);
            }
            loadPalette();
            browserOpen = false;
            fnUndo.clear();
            fnRedo.clear();
        }
    } else if(browserMode == 1) {
        if(!exists || overwrite) {
            exportPalette(dir);
            lastPalName = subDir;
            lastPalDir = parent;
            browserOpen = false;
            fnUndo.clear();
            fnRedo.clear();
        } else {
            overwrite = true;
        }
    } else if(browserMode == 2) {
        if(!exists || overwrite) {
            exportTexSingle(dir);
            currentSocket->lastTexName = subDir;
            currentSocket->lastTexDir = parent;
            browserOpen = false;
            fnUndo.clear();
            fnRedo.clear();
        } else {
            overwrite = true;
        }
    } else if(browserMode == 3) {
        if(!exists || overwrite) {
            //exportTexMulti(dir);
            lastTexName = subDir;
            lastTexDir = parent;
            browserOpen = false;
            fnUndo.clear();
            fnRedo.clear();
        } else {
            overwrite = true;
        }
    } else if(browserMode == 4) {
        if(exists) {
            listUndo.clear();
            listRedo.clear();
            loadFile(dir);
            browserOpen = false;
            fnUndo.clear();
            fnRedo.clear();
        }
    } else if(browserMode == 5) {
        if(!exists || overwrite) {
            lastSaveName = subDir;
            lastSaveDir = parent;
            saveFile(dir);
            browserOpen = false;
            fnUndo.clear();
            fnRedo.clear();
        } else {
            overwrite = true;
        }
    }
}

#ifdef _WIN32
       void openBrowser(string dir, int type, int mode) {
           overwrite = false;
           browserMode = mode;
           browserScroll = 0;
           if(type == 1) { //undo
               fnUndo.pop_back();
               fnRedo.push_back(currentDir);
           } else if(type == 2) { //redo
               fnRedo.pop_back();
               fnUndo.push_back(currentDir);
           } else {
               fnUndo.push_back(currentDir);
               fnRedo.clear();
           }
           selectedFile = -1;
           currentDir = dir;
           filenameB = "";
           if(browserMode == 1 && lastPalDir.size() > 0 && !browserOpen) {
               currentDir = lastPalDir;
               filenameB = lastPalName;
           }
           Texture* t = texs.at(currentTexture);
           if(browserMode == 2 && currentSocket->lastTexDir.size() > 0 && !browserOpen) {
               currentDir = currentSocket->lastTexDir;
               filenameB = currentSocket->lastTexName;
           }
           if(browserMode == 3 && lastTexDir.size() > 0 && !browserOpen) {
               currentDir = lastTexDir;
               filenameB = lastTexName;
           }
           if(browserMode == 5 && lastSaveDir.size() > 0 && !browserOpen) {
               currentDir = lastSaveDir;
               filenameB = lastSaveName;
           }
           for(int toDel = 0; toDel < filenames.size(); toDel++) {
               delete filenames.at(toDel);
           }
           filenames.clear();

           DIR* dirr;
           struct dirent* ent;
           vector<File*> temp;
           if((dirr = opendir(currentDir.c_str())) != NULL) {
               while((ent = readdir(dirr)) != NULL) {
                   if(ent->d_name[0] != '.') {
                       string fullDir = currentDir;
                       fullDir = fullDir.append("\\");
                       fullDir = fullDir.append(string(ent->d_name));
                       struct stat path_stat;
                       stat(fullDir.c_str(), &path_stat);
                       File* a = new File(string(ent->d_name), !S_ISREG(path_stat.st_mode));
                       if(a->folder) {
                           filenames.push_back(a);
                       } else {
                           bool compatible = true;
                           //SDL_RWops *rwop = SDL_RWFromFile(fullDir.c_str(), "rb");
                           /*compatible = compatible | IMG_isBMP(rwop);
                            compatible = compatible | IMG_isCUR(rwop);
                            compatible = compatible | IMG_isGIF(rwop);
                            compatible = compatible | IMG_isICO(rwop);
                            compatible = compatible | IMG_isJPG(rwop);
                            compatible = compatible | IMG_isLBM(rwop);
                            compatible = compatible | IMG_isPCX(rwop);
                            compatible = compatible | IMG_isPNG(rwop);
                            compatible = compatible | IMG_isPNM(rwop);
                            compatible = compatible | IMG_isTIF(rwop);
                            compatible = compatible | IMG_isWEBP(rwop);
                            compatible = compatible | IMG_isXCF(rwop);
                            compatible = compatible | IMG_isXPM(rwop);
                            compatible = compatible | IMG_isXV(rwop);*/
                           if(compatible) {
                               temp.push_back(a);
                           }
                       }
                   }
               }
               closedir(dirr);
           } else {
           }
           filenames.insert(filenames.end(), temp.begin(), temp.end());
           browserOpen = true;
       }

       void getBundle() {
           //nothing here
       }

       void setIcon() {
           unzFile data = unzOpen("data.lpf");
           unz_file_info info;
           Uint8* buffer;
           SDL_RWops* rw = NULL;
           SDL_Surface* ls = NULL;

           unzLocateFile(data, "icon.ico", NULL);
           unzOpenCurrentFile(data);
           unzGetCurrentFileInfo(data, &info, NULL, 0, NULL, 0, NULL, 0);
           buffer = (Uint8*)malloc(info.uncompressed_size);
           unzReadCurrentFile(data, buffer, info.uncompressed_size);
           rw = SDL_RWFromConstMem(buffer, info.uncompressed_size);
           ls = IMG_LoadICO_RW(rw);
           free(buffer);

           SDL_SetWindowIcon(window, ls);
       }

       void initGlew() {
           glewExperimental = GL_TRUE;
           glewInit();
       }

       void getHome() {
            const char *homeDir;
            homeDir = getenv("HOMEPATH");
            currentDir = homeDir;
       }

       void warpMouse(int x, int y) {
           SDL_WarpMouseInWindow(window, x, y);
       }
#elif __APPLE__
       void openBrowser(string dir, int type, int mode) {
           overwrite = false;
           browserMode = mode;
           browserScroll = 0;
           if(type == 1) { //undo
               fnUndo.pop_back();
               fnRedo.push_back(currentDir);
           } else if(type == 2) { //redo
               fnRedo.pop_back();
               fnUndo.push_back(currentDir);
           } else {
               fnUndo.push_back(currentDir);
               fnRedo.clear();
           }
           selectedFile = -1;
           currentDir = dir;
           filenameB = "";
           if(browserMode == 1 && lastPalDir.size() > 0 && !browserOpen) {
               currentDir = lastPalDir;
               filenameB = lastPalName;
           }
           Texture* t = texs.at(currentTexture);
           if(browserMode == 2 && currentSocket->lastTexDir.size() > 0 && !browserOpen) {
               currentDir = currentSocket->lastTexDir;
               filenameB = currentSocket->lastTexName;
           }
           if(browserMode == 3 && lastTexDir.size() > 0 && !browserOpen) {
               currentDir = lastTexDir;
               filenameB = lastTexName;
           }
           if(browserMode == 5 && lastSaveDir.size() > 0 && !browserOpen) {
               currentDir = lastSaveDir;
               filenameB = lastSaveName;
           }
           for(int toDel = 0; toDel < filenames.size(); toDel++) {
               delete filenames.at(toDel);
           }
           filenames.clear();
           int num_entries;
           struct dirent **entries = NULL;
           num_entries = scandir(currentDir.c_str(), &entries, NULL, NULL);
           vector<File*> temp;
           for(int i = 0; i < num_entries; i++) {
               if(entries[i]->d_name[0] != '.') {
                   string fullDir = currentDir;
                   fullDir = fullDir.append("/");
                   fullDir = fullDir.append(string(entries[i]->d_name));
                   struct stat path_stat;
                   stat(fullDir.c_str(), &path_stat);
                   File* a = new File(string(entries[i]->d_name), !S_ISREG(path_stat.st_mode));
                   if(a->folder) {
                       filenames.push_back(a);
                   } else {
                       bool compatible = true;
                       //SDL_RWops *rwop = SDL_RWFromFile(fullDir.c_str(), "rb");
                       /*compatible = compatible | IMG_isBMP(rwop);
                        compatible = compatible | IMG_isCUR(rwop);
                        compatible = compatible | IMG_isGIF(rwop);
                        compatible = compatible | IMG_isICO(rwop);
                        compatible = compatible | IMG_isJPG(rwop);
                        compatible = compatible | IMG_isLBM(rwop);
                        compatible = compatible | IMG_isPCX(rwop);
                        compatible = compatible | IMG_isPNG(rwop);
                        compatible = compatible | IMG_isPNM(rwop);
                        compatible = compatible | IMG_isTIF(rwop);
                        compatible = compatible | IMG_isWEBP(rwop);
                        compatible = compatible | IMG_isXCF(rwop);
                        compatible = compatible | IMG_isXPM(rwop);
                        compatible = compatible | IMG_isXV(rwop);*/
                       if(compatible) {
                           temp.push_back(a);
                       }
                   }
               }
           }
           filenames.insert(filenames.end(), temp.begin(), temp.end());
           for(int i = 0; i < num_entries; i++) {
               free(entries[i]);
           }
           free(entries);
           browserOpen = true;
       }

       void getBundle() {
           mainBundle = CFBundleGetMainBundle();
       }

       void setIcon() {
           //nothing here
       }

       void initGlew() {
           //nothing here
       }

       void getHome() {
            char* homeDir;
            homeDir = getenv("HOME");
            if(!homeDir) {
                struct passwd* pwd = getpwuid(getuid());
                if (pwd) {
                    homeDir = pwd->pw_dir;
                }
            }
            currentDir = homeDir;
       }

       void warpMouse(int x, int y) {
           SDL_WarpMouseGlobal(x, y);
       }
#elif __linux__
#else
#endif

void LoadStuff() {
    srand(time(NULL));
    font = TTF_OpenFont("KontrapunktLight.ttf", 50);

    light_program = glCreateProgram();
    light_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    light_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load Shader Sources
    glShaderSource(light_vertex_shader, 1, &light_vertex_shader_source, NULL);
    glShaderSource(light_fragment_shader, 1, &light_fragment_shader_source, NULL);

    // Compile The Shaders
    glCompileShader(light_vertex_shader);
    glCompileShader(light_fragment_shader);

    // Attach The Shader Objects To The Program Object
    glAttachShader(light_program, light_vertex_shader);
    glAttachShader(light_program, light_fragment_shader);

    // Link The Program Object
    glLinkProgram(light_program);

    // Use The Program Object Instead Of Fixed Function OpenGL
    glUseProgram(light_program);

    blur_program = glCreateProgram();
    blur_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    blur_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load Shader Sources
    glShaderSource(blur_vertex_shader, 1, &blur_vertex_shader_source, NULL);
    glShaderSource(blur_fragment_shader, 1, &blur_fragment_shader_source, NULL);

    // Compile The Shaders
    glCompileShader(blur_vertex_shader);
    glCompileShader(blur_fragment_shader);

    // Attach The Shader Objects To The Program Object
    glAttachShader(blur_program, blur_vertex_shader);
    glAttachShader(blur_program, blur_fragment_shader);

    // Link The Program Object
    glLinkProgram(blur_program);

    // Use The Program Object Instead Of Fixed Function OpenGL
    glUseProgram(blur_program);

    transition_program = glCreateProgram();
    transition_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    transition_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load Shader Sources
    glShaderSource(transition_vertex_shader, 1, &transition_vertex_shader_source, NULL);
    glShaderSource(transition_fragment_shader, 1, &transition_fragment_shader_source, NULL);

    // Compile The Shaders
    glCompileShader(transition_vertex_shader);
    glCompileShader(transition_fragment_shader);

    // Attach The Shader Objects To The Program Object
    glAttachShader(transition_program, transition_vertex_shader);
    glAttachShader(transition_program, transition_fragment_shader);

    // Link The Program Object
    glLinkProgram(transition_program);

    // Use The Program Object Instead Of Fixed Function OpenGL
    glUseProgram(transition_program);

    /*char* my_vertex_shader_source = (char*)"#version 130\nin vec4 position; uniform int frame; uniform vec2 frameSize; uniform vec2 off; uniform vec2 texSize; uniform mat4 model; uniform mat4 proj; in vec2 t; out vec2 txc; void main() { gl_Position = (proj*model) * position; int tx = int(mod(frame, 5)); int ty = frame/5; txc = vec2(((t.x+tx)*frameSize.x+off.x)/texSize.x, ((t.y+ty)*frameSize.y+off.y)/texSize.y); }";
    char* my_fragment_shader_source = (char*)"#version 130\nout vec4 fragColor; uniform sampler2D tex; uniform float alpha; in vec2 txc; void main() { fragColor = vec4(texture(tex, txc).xyz/texture(tex, txc).w, texture(tex, txc).w*alpha); }";*/

    my_program = glCreateProgram();
    my_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    my_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load Shader Sources
    glShaderSource(my_vertex_shader, 1, &my_vertex_shader_source, NULL);
    glShaderSource(my_fragment_shader, 1, &my_fragment_shader_source, NULL);

    // Compile The Shaders
    glCompileShader(my_vertex_shader);
    glCompileShader(my_fragment_shader);

    // Attach The Shader Objects To The Program Object
    glAttachShader(my_program, my_vertex_shader);
    glAttachShader(my_program, my_fragment_shader);

    // Link The Program Object
    glLinkProgram(my_program);

    // Use The Program Object Instead Of Fixed Function OpenGL
    glUseProgram(my_program);

    mp_tex = glGetUniformLocation(my_program, "tex");
    mp_texN = glGetUniformLocation(my_program, "texN");
    mp_frameSize = glGetUniformLocation(my_program, "frameSize");
    mp_texSize = glGetUniformLocation(my_program, "texSize");
    mp_off = glGetUniformLocation(my_program, "off");
    mp_frame = glGetUniformLocation(my_program, "frame");
    mp_depth = glGetUniformLocation(my_program, "depth");
    mp_alpha = glGetUniformLocation(my_program, "alpha");
    mp_strength = glGetUniformLocation(my_program, "strength");
    mp_model = glGetUniformLocation(my_program, "model");
    mp_flip = glGetUniformLocation(my_program, "flip");
    mp_proj =glGetUniformLocation(my_program, "proj");

    float texRect[] =
    {
        0.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 1.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 0.0, 0.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        0.0, 1.0, 0.0, 1.0
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texRect), texRect, GL_STATIC_DRAW);
    //glVertexAttribPointer(shaderPos, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(my_program, "position"));

    float texTri[] =
    {
        -float(0.5*sqrt(3)/3.0), -0.5, 0.0, 0.0,
        float(0.5*sqrt(3))-float(0.5*sqrt(3)/3.0), 0.0, 1.0, 1.0,
        -float(0.5*sqrt(3)/3.0), 0.5, 0.0, 1.0
    };
    glGenBuffers(1, &vboTri);
    glBindBuffer(GL_ARRAY_BUFFER, vboTri);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texTri), texTri, GL_STATIC_DRAW);

    glGenTextures(1, &screenTex);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &screenTexN);
    glBindTexture(GL_TEXTURE_2D, screenTexN);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &screenTexMisc);
    glBindTexture(GL_TEXTURE_2D, screenTexMisc);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &screenTexB);
    glBindTexture(GL_TEXTURE_2D, screenTexB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenFramebuffers(1, &screenFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, screenTexN, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, screenTexMisc, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, screenTexB, 0);
    GLenum d[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, d);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    logoImage = loadTexture("logo.png");
    titleImage = loadTexture("title.png");
    thanksImage = loadTexture("thanks.png");
    flashImg = loadTexture("flash.png");
    camOffset = 1.0;
    camOffsetMagnitude = 1.2;

    loadGen();

    logoTimer = logoTimerMax;
    getBundle();
}

int textW(string text, int x, int y, GLuint tex, bool alignRight) {
    int kerning[] =    {2, 2, 2, 2, 3,
        3, 2, 2, 6, 3,
        3, 3, 1, 3, 2,
        2, 2, 2, 3, 2,
        3, 2, 1, 3, 2,
        2, 2, 2, 2, 2,
        2, 3, 2, 2, 5,
        4, 3, 5, -1, 2,
        2, 2, 2, 2, 3,
        3, 2, 2, 1, 3,
        2, 2, 2, 2, 6,

        6, 5, 3, 3, 2,
        2, 6, 5, 5, 3,
        3, 6, 4, 6, 2,
        6, 6, 3, 4, 3,
        2, 1, 5, 4, 5,
        5, 4, 4, 6, 4,
        4};
    int kerningD[] = {3, 5, 3, 3, 3, 3, 3, 4, 3, 3, 2, 4};
    int w = -1;
    for(int i = 0; i < text.size(); i++) {
        if(text[i] >= 33 && text[i] <= 47) {
            w += 9-kerning[55+text[i]-33];
        } else if(text[i] >= 48 && text[i] <= 57) {
            w += 9-kerningD[text[i]-48];
        } else if(text[i] >= 58 && text[i] <= 64) {
            w += 9-kerning[70+text[i]-58];
        } else if(text[i] >= 91 && text[i] <= 95) {
            w += 9-kerning[77+text[i]-91];
        } else if(text[i] >= 123 && text[i] <= 126) {
            w += 9-kerning[82+text[i]-123];
        } else if(text[i] == 32) {
            w += 9-kerning[52];
        } else if((text[i] >= 65 && text[i] <= 90) || (text[i] >= 97 && text[i] <= 122)) {
            int f = text[i] - 'A';
            if(f >= 32) {
                f -= 6;
            }
            w += 9-kerning[f];
        } else {
            w += 9-kerning[0];
        }
    }
    int wd = w;
    return wd;
}

int textH(string text, int x, int y, GLuint tex, bool alignRight) {
    int h = 9;
    for(int i = 0; i < text.size(); i++) {
        if(text[i] == '\n') {
            h += 9;
        }
    }
    return h;
}

void adjustBrowserScroll() {
    int title = collH-4;
    int bSpace = 5;
    float tS = title/2-4;
    int fh = tS;
    int fw = -(screenW-barXRight-barX-bSpace*2);
    int fwNext = 0;
    for(int i = 0; i < filenames.size(); i++) {
        int n = textW(filenames.at(i)->name, barX+1+bSpace+fw+tS+8, title*2+bSpace*3+fh, fontImg, 0);
        if(n > fwNext) {
            fwNext = n;
        }
        fh += title;
        if(fh+title*2+title+bSpace*3 > title+bSpace*2+barY-1-bSpace*3-title-6 && i != filenames.size()-1) {
            fh = tS;
            fw += fwNext+tS*2+8*3;
            fwNext = 0;
        }
    }
    fw += fwNext+tS*2+8*3;
    if(browserScroll > fw) {
        browserScroll = fw;
    }
    if(browserScroll < 0) {
        browserScroll = 0;
    }
}

void adjustToolsScroll() {
    int maxH = -(barY)+barY5;
    if(toolsScroll > maxH+1) {
        toolsScroll = maxH+1;
    }
    if(toolsScroll < 0) {
        toolsScroll = 0;
    }
    int maxHPal = 0;
    int space = 1;
    float offset = 0;
    int w = barXRight-10+space-offset*2-7-7;
    float size = 12;
    int c = w/size;
    int leftspace = w-size*c;
    leftspace = 0;
    for(int p = 0; p < palette.size(); p++) {
        int x = p%c;
        int y = p/c;
        int offset2 = int(p==selectedColor)*2;
        if(y*(size)+size > maxHPal) {
            maxHPal = y*(size)+size;
        }
    }
    maxHPal -= barY3-2-5-8-5;
    if(palScroll > maxHPal) {
        palScroll = maxHPal;
    }
    if(palScroll < 0) {
        palScroll = 0;
    }
    int maxHTex = 0;
    int spaceT = 1;
    int wT = barXRight-2-7;
    float sizeT = texSizeX+1;
    int cT = wT/sizeT;
    int leftspaceT = wT-sizeT*cT;
    leftspaceT = 0;
    for(int p = 0; p < texs.size(); p++) {
        int x = p%cT;
        int y = p/cT;
        if(y*(sizeT)+sizeT+1 > maxHTex) {
            maxHTex = y*(sizeT)+sizeT+1;
        }
    }
    maxHTex -= screenH-barY-5-8-5-8-(14-8)/2-5;
    if(texScroll > maxHTex) {
        texScroll = maxHTex;
    }
    if(texScroll < 0) {
        texScroll = 0;
    }
}

void adjustLayersScroll() {
    int hTop = -(barY);
    int count = newEffects.size();
    hTop += collH*count;
    if(newEScroll > hTop+1) {
        newEScroll = hTop+1;
    }
    if(newEScroll < 0) {
        newEScroll = 0;
    }
}

void adjustWidths() {
    int space = 4;
    int s = 36;
    int s2 = (barXRight-7-space*4)-s;
    int paramSize = s2/2.0;
    int paramH = s/3.0;
    int offset = (paramH-7)/4.0;
    colorParams.at(1)->w = paramSize-offset*2;
    colorParams.at(2)->w = paramSize-offset*2;
    colorParams.at(3)->w = paramSize-offset*2;
    colorParams.at(4)->x = space+s+space*2+offset+paramSize;
    colorParams.at(5)->x = space+s+space*2+offset+paramSize;
    colorParams.at(6)->x = space+s+space*2+offset+paramSize;
    colorParams.at(4)->w = paramSize-offset*2;
    colorParams.at(5)->w = paramSize-offset*2;
    colorParams.at(6)->w = paramSize-offset*2;
    colorParams.at(7)->w = barXRight-7-space*2;
    colorParams.at(8)->w = barXRight-7-space*2;
}

void resizeWindow(int w, int h) {
    float by = screenH-barY;
    screenW = w/float(screenScale);
    screenH = h/float(screenScale);
    screenPW = pow(2, ceil(log(screenW)/log(2)));
    screenPH = pow(2, ceil(log(screenH)/log(2)));
    barY = screenH-by;
    glViewport(0, 0, screenW*screenScale, screenH*screenScale);
    proj = glm::ortho(0.0, (double)screenW*screenScale, (double)screenH*screenScale, 0.0, -1.0, 1.0);
    glGenTextures(1, &screenTex);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &screenTexN);
    glBindTexture(GL_TEXTURE_2D, screenTexN);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &screenTexMisc);
    glBindTexture(GL_TEXTURE_2D, screenTexMisc);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &screenTexB);
    glBindTexture(GL_TEXTURE_2D, screenTexB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenFramebuffers(1, &screenFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, screenTexN, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, screenTexMisc, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, screenTexB, 0);
    GLenum d[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, d);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if(barX < 128+7) {
        barX = 128+7;
    } else if(barX > screenW/3) {
        barX = screenW/3;
    }
    barY = screenH;
    if(barXRight < 128+7) {
        barXRight = 128+7;
    } else if(barXRight > screenW/3) {
        barXRight = screenW/3;
    }

    adjustWidths();
    adjustBrowserScroll();
    adjustLayersScroll();
    adjustToolsScroll();
}

Color::Color(int r, int g, int b) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = 255;
    disabled = false;
}

bool Color::equals(Color* c) {
    if(c->r == this->r &&
       c->g == this->g &&
       c->b == this->b) {
        return true;
    } else {
        return false;
    }
}

bPoint cBezier::getPoint(float t) {
    float x = pow(1-t, 3)*P0.x+3*pow(1-t, 2)*t*P1.x+3*(1-t)*pow(t, 2)*P2.x+pow(t, 3)*P3.x;
    float y = pow(1-t, 3)*P0.y+3*pow(1-t, 2)*t*P1.y+3*(1-t)*pow(t, 2)*P2.y+pow(t, 3)*P3.y;
    return bPoint(x, y);
}

bPoint cBezier::getTangent(float t) {
    float x = 3*pow(1-t, 2)*(P1.x-P0.x)+6*(1-t)*t*(P2.x-P1.x)+3*pow(t, 2)*(P3.x-P2.x);
    float y = 3*pow(1-t, 2)*(P1.y-P0.y)+6*(1-t)*t*(P2.y-P1.y)+3*pow(t, 2)*(P3.y-P2.y);
    float l = sqrt(x*x+y*y);
    x /= l;
    y /= l;
    //cout << x << " " << y << endl;
    return bPoint(x, y);
}

void cBezier::update() {
    float segs = 1.0/d;
    vector<float> data;
    for(int i = 0; i < d; i++) {
        bPoint a = getPoint(i*segs);
        bPoint b = getPoint((i+1)*segs);
        bPoint ta = getTangent(i*segs);
        bPoint tb = getTangent((i+1)*segs);
        bPoint pa = bPoint(ta.y, -ta.x);
        bPoint pb = bPoint(tb.y, -tb.x);
        float w = 1;
        data.push_back(a.x-pa.x*w);data.push_back(a.y-pa.y*w);data.push_back(0);data.push_back(0);
        data.push_back(b.x-pb.x*w);data.push_back(b.y-pb.y*w);data.push_back(0);data.push_back(0);
        data.push_back(b.x+pb.x*w);data.push_back(b.y+pb.y*w);data.push_back(0);data.push_back(0);

        data.push_back(a.x-pa.x*w);data.push_back(a.y-pa.y*w);data.push_back(0);data.push_back(0);
        data.push_back(b.x+pb.x*w);data.push_back(b.y+pb.y*w);data.push_back(0);data.push_back(0);
        data.push_back(a.x+pa.x*w);data.push_back(a.y+pa.y*w);data.push_back(0);data.push_back(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, bVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data.size(), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void update() { //update
    previewTimer++;
    blinkTimer++;
    if(blinkTimer > 64) {
        blinkTimer = 0;
    }
    if(errorTimer > 0) {
        errorTimer--;
    }
    if(draggingCam || draggingCam2) {
        float d = 4.0;
        float dx = (mouseX-mouseOX)/d;
        float dy = (mouseY-mouseOY)/d;
        mouseOX += dx;
        mouseOY += dy;
        nodeCSX = dx;
        nodeCSY = dy;
    }
    float cams = 1.5;
    nodeCX += nodeCSX;
    nodeCY += nodeCSY;
    nodeCSX /= cams;
    nodeCSY /= cams;
    if(logoTimer > 0) {
        logoTimer--;
    }
    rotTimer += 15;
    if(rotTimer >= 360) {
        rotTimer -= 360;
    }
    zoomA = zoom;
    if(doubleClickTimer < 100) {
        doubleClickTimer++;
    }
    if(typeTimer < 100) {
        typeTimer++;
        if(typeTimer == 20) {
            textType = textTypeTemp;
            textTypeFx = textTypeFxTemp;
            textTypeLayer = textTypeLayerTemp;
            if(textType != NULL) {
                textType->typing = "";
            }
        }
    }
    Texture* t = texs.at(currentTexture);
    if(!t->done) {
        t->doneTimer++;
    } else {
        t->doneTimer = 0;
    }
    if(draggingNode) {
        float d = 4.0;
        float dx = (mouseX-mouseOX)/d;
        float dy = (mouseY-mouseOY)/d;
        mouseOX += dx;
        mouseOY += dy;
        t->fxs.at(draggedNode)->sx = dx;
        t->fxs.at(draggedNode)->sy = dy;
        t->fxs.at(draggedNode)->r = dx;
    }
    for(int i = 0; i < t->fxs.size(); i++) {
        nEffect* fx = t->fxs.at(i);
        float d = 1.5;
        fx->x += fx->sx;
        fx->y += fx->sy;
        fx->sx /= d;
        fx->sy /= d;

        fx->r /= d;

        if(!fx->done) {
            fx->doneTimer++;
        } else {
            fx->doneTimer = 0;
        }
        if(fx->loaded) {//gentex2
            fx->side.join();
            if(fx->abort && fx->deleted) {
                texs.at(currentTexture)->fxs.erase(std::remove(texs.at(currentTexture)->fxs.begin(), texs.at(currentTexture)->fxs.end(), fx), texs.at(currentTexture)->fxs.end());
                delete fx;
            } else {
                fx->loading = false;
                fx->loaded = false;
                if(fx->abort) {
                    fx->abort = false;
                } else {
                    fx->done = true;
                    bool done = true;
                    for(int i2 = 0; i2 < t->fxs.size(); i2++) {
                        if(!t->fxs.at(i2)->done) {
                            done = false;
                        }
                    }
                    if(!done) {
                        t->genTex(false);
                    }
                }
                for(int out = 0; out < fx->outputs.size(); out++) {
                    Socket* output = fx->outputs.at(out);
                    //glDeleteTextures(1, &output->texture);
                    glGenTextures(1, &output->texture);
                    glBindTexture(GL_TEXTURE_2D, output->texture);
                    if(OS == 0) {
                        vector<GLubyte> bv;
                        for(int db = 0; db < output->texData.size(); db++) {
                            GLubyte c = fmax(0.0, fmin(255.0, output->texData.at(db)));
                            bv.push_back(c);
                        }
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, &bv[0]);
                    } else if(OS == 1) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texSizeX, texSizeY, 0, GL_RGBA, GL_FLOAT, &output->texData[0]);
                    }
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                }
            }
        }
        for(int in = 0; in < fx->inputs.size(); in++) {
            Socket* input = fx->inputs.at(in);
            if(input->s != NULL) {
                float xa = input->s->parent->x+input->s->parent->w;
                float ya = input->s->parent->y+input->s->y+4;
                xa -= input->s->parent->x+input->s->parent->w/2.0;
                ya -= input->s->parent->y+input->s->parent->h/2.0;
                float a = glm::radians(-input->s->parent->r);
                float xb = ya*sin(a) + xa*cos(a);
                float yb = ya*cos(a) - xa*sin(a);
                xb += input->s->parent->x+input->s->parent->w/2.0;
                yb += input->s->parent->y+input->s->parent->h/2.0;
                input->b->P0.x = xb+int(nodeCX);
                input->b->P0.y = yb+int(nodeCY);
                xa = input->parent->x;
                ya = input->parent->y+input->y+4;
                xa -= input->parent->x+input->parent->w/2.0;
                ya -= input->parent->y+input->parent->h/2.0;
                a = glm::radians(-input->parent->r);
                xb = ya*sin(a) + xa*cos(a);
                yb = ya*cos(a) - xa*sin(a);
                xb += input->parent->x+input->parent->w/2.0;
                yb += input->parent->y+input->parent->h/2.0;
                input->b->P3.x = xb+int(nodeCX);
                input->b->P3.y = yb+int(nodeCY);
                float dx = input->b->P3.x-input->b->P0.x;
                float offset = abs(dx)/2.0;
                input->b->P1.x = input->b->P0.x+offset;
                input->b->P1.y = input->b->P0.y;
                input->b->P2.x = input->b->P3.x-offset;
                input->b->P2.y = input->b->P3.y;
                input->b->update();
            }
        }
    }
    if(t->abort) {//gentex3
        bool stillLoading = false;
        for(int in = 0; in < t->fxs.size(); in++) {
            if(t->fxs.at(in)->loading) {
                stillLoading = true;
            }
        }
        if(!stillLoading) {
            for(int in = 0; in < t->fxs.size(); in++) {
                if(t->fxs.at(in)->undone) {
                    t->fxs.at(in)->clearTree();
                }
            }
            t->genTex(false);
            t->abort = false;
        }
    }
    if(draggingSocket) {
        float d = 4.0;
        float dx = (mouseX-mouseOX)/d;
        float dy = (mouseY-mouseOY)/d;
        mouseOX += dx;
        mouseOY += dy;
        nEffect* fx = t->fxs.at(draggedNode);
        Socket* output = fx->outputs.at(draggedSocket);
        float xa = output->parent->x+output->parent->w;
        float ya = output->parent->y+output->y+4;
        xa -= output->parent->x+output->parent->w/2.0;
        ya -= output->parent->y+output->parent->h/2.0;
        float a = glm::radians(-output->parent->r);
        float xb = ya*sin(a) + xa*cos(a);
        float yb = ya*cos(a) - xa*sin(a);
        xb += output->parent->x+output->parent->w/2.0;
        yb += output->parent->y+output->parent->h/2.0;
        output->b->P0.x = xb+int(nodeCX);
        output->b->P0.y = yb+int(nodeCY);
        output->snapped = false;
        output->px += dx;
        output->py += dy;
        output->b->P3.x = output->px;
        output->b->P3.y = output->py;
        float expand = 2;
        for(int i = 0; i < t->fxs.size(); i++) {
            nEffect* fx2 = t->fxs.at(i);
            if(i != draggedNode) {
                for(int in = 0; in < fx2->inputs.size(); in++) {
                    if(mouseX > fx2->x+nodeCX-4-expand && mouseX < fx2->x+nodeCX+4+expand && mouseY > fx2->y+nodeCY+fx2->inputs.at(in)->y-expand && mouseY < fx2->y+nodeCY+fx2->inputs.at(in)->y+8+expand/* && fx2->inputs.at(in)->s == NULL*/) {
                        output->b->P3.x = fx2->x+nodeCX;
                        output->b->P3.y = fx2->y+nodeCY+fx2->inputs.at(in)->y+4;
                        output->snapped = true;
                        output->s = fx2->inputs.at(in);
                    }
                }
            }
        }
        float ddx = output->b->P3.x-output->b->P0.x;
        float offset = abs(ddx)/2.0;
        output->b->P1.x = output->b->P0.x+offset;
        output->b->P1.y = output->b->P0.y;
        output->b->P2.x = output->b->P3.x-offset;
        output->b->P2.y = output->b->P3.y;
        output->b->update();
    }

    if(draggingSBar) {
        float d = 4.0;
        float dx = (mouseX-mouseOX)/d;
        float dy = (mouseY-mouseOY)/d;
        mouseOX += dx;
        mouseOY += dy;
        if(sBarDrag == 0) {
            layersScroll += dy*sRatio;
            adjustLayersScroll();
        } else if(sBarDrag == 1) {
            layersScrollH += dx*sRatio;
            adjustLayersScroll();
        } else if(sBarDrag == 2) {
            newEScroll += dy*sRatio;
            adjustLayersScroll();
        } else if(sBarDrag == 3) {
            palScroll += dy*sRatio;
            adjustToolsScroll();
        } else if(sBarDrag == 4) {
            toolsScroll += dy*sRatio;
            adjustToolsScroll();
        } else if(sBarDrag == 5) {
            texScroll += dy*sRatio;
            adjustToolsScroll();
        } else if(sBarDrag == 6) {
            browserScroll += dx*sRatio;
            adjustBrowserScroll();
        }
    }

    if(draggingUI) {
        if(UIdragType == 0) {
            float d = 4.0;
            float dx = (mouseX-mouseOX)/d;
            float dy = (mouseY-mouseOY)/d;
            mouseOX += dx;
            mouseOY += dy;
            barX += dx;
            if(barX < 128+7) {
                barX = 128+7;
            } else if(barX > screenW/3) {
                barX = screenW/3;
            }
            adjustWidths();
        } else if(UIdragType == 1) {
            float d = 4.0;
            float dx = (mouseX-mouseOX)/d;
            float dy = (mouseY-mouseOY)/d;
            mouseOX += dx;
            mouseOY += dy;
            barY += dy;
            barY = screenH;
        } else if(UIdragType == 2) {
            float d = 4.0;
            float dx = (mouseX-mouseOX)/d;
            float dy = (mouseY-mouseOY)/d;
            mouseOX += dx;
            mouseOY += dy;
            barXRight -= dx;
            if(barXRight < 128+7) {
                barXRight = 128+7;
            } else if(barXRight > screenW/3) {
                barXRight = screenW/3;
            }
            adjustWidths();
        }
        adjustBrowserScroll();
        adjustLayersScroll();
        adjustToolsScroll();
    }
    camX = 0;
    camY = 0;
    camRot += 1;
    float tr = 0.005;
    if(camOffset > 1.0) {
        camOffsetSpeed -= (camOffset-1.0)/1.5;
        if(camOffsetSpeed < tr && !camPositive) {
            camOffsetSpeed = 0.0;
            camOffset = 1.0;
        }
        camPositive = true;
    } else if(camOffset < 1.0) {
        camOffsetSpeed += (1.0-camOffset)/1.5;
        if(camOffsetSpeed > -tr && camPositive) {
            camOffsetSpeed = 0.0;
            camOffset = 1.0;
        }
        camPositive = false;
    }
    camOffset += camOffsetSpeed;
    camOffsetSpeed /= camOffsetMagnitude;
    int maxOffset = 16*6.0;
    camXFinal = camX+(sin(camRot*M_PI/180.0)*(camOffset-1.0)*maxOffset);
    camYFinal = camY+(cos(camRot*M_PI/180.0)*(camOffset-1.0)*maxOffset);
}

void renderGradient(int x, int y, int w, int h, Color* c, Color* c2) {
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(light_program);
    glUniform2f(glGetUniformLocation(light_program, "frameSize"), (float)w, (float)h);
    glUniform2f(glGetUniformLocation(light_program, "texSize"), (float)w, (float)h);
    glUniform1i(glGetUniformLocation(light_program, "frame"), 0);
    glUniform3f(glGetUniformLocation(light_program, "color"), (float)c->r/255.0, (float)c->g/255.0, (float)c->b/255.0);
    glUniform3f(glGetUniformLocation(light_program, "color2"), (float)c2->r/255.0, (float)c2->g/255.0, (float)c2->b/255.0);
    glUniformMatrix4fv(glGetUniformLocation(light_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniformMatrix4fv(glGetUniformLocation(light_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(light_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(light_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(light_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(light_program, "t"));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUseProgram(0);
}

void renderColor(int x, int y, int w, int h, Color* c, int cutoff = -1, int cutoff2 = -1, int cutoff3 = -1, int cutoff4 = -1) {
    if(x < cutoff && cutoff != -1) {
        w = x+w-cutoff;
        if(w < 0) {
            w = 0;
        }
        x = cutoff;
    }
    if(x+w > cutoff2 && cutoff2 != -1) {
        w = cutoff2-x;
        if(w < 0) {
            w = 0;
        }
    }
    if(y < cutoff3 && cutoff3 != -1) {
        h = y+h-cutoff3;
        if(h < 0) {
            h = 0;
        }
        y = cutoff3;
    }
    if(y+h > cutoff4 && cutoff4 != -1) {
        h = cutoff4-y;
        if(h < 0) {
            h = 0;
        }
    }
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(light_program);
    glUniform2f(glGetUniformLocation(light_program, "frameSize"), (float)w, (float)h);
    glUniform2f(glGetUniformLocation(light_program, "texSize"), (float)w, (float)h);
    glUniform1i(glGetUniformLocation(light_program, "frame"), 0);
    glUniform2f(glGetUniformLocation(light_program, "off"), (float)0, (float)0);
    glUniform3f(glGetUniformLocation(light_program, "color"), (float)c->r/255.0, (float)c->g/255.0, (float)c->b/255.0);
    glUniform3f(glGetUniformLocation(light_program, "color2"), (float)c->r/255.0, (float)c->g/255.0, (float)c->b/255.0);
    glUniformMatrix4fv(glGetUniformLocation(light_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniformMatrix4fv(glGetUniformLocation(light_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(light_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(light_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(light_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(light_program, "t"));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUseProgram(0);
}

void renderBezier(cBezier* b, GLuint tex) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glm::mat4 currentMatrix = model;
    glUseProgram(my_program);
    glUniform1i(mp_tex, 0);
    glUniform1i(mp_texN, 1);
    glUniform2f(mp_frameSize, (float)100, (float)100);
    glUniform2f(mp_texSize, (float)100, (float)100);
    glUniform2f(mp_off, (float)0, (float)0);
    glUniform1i(mp_frame, 0);
    glUniform1f(mp_depth, 0);
    glUniform1f(mp_alpha, 1);
    glUniform1f(mp_strength, 0);
    glUniformMatrix4fv(mp_model, 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniform2f(mp_flip, (float)0, (float)0);
    glUniformMatrix4fv(mp_proj, 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, b->bVbo);
    glVertexAttribPointer(glGetAttribLocation(my_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(my_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(my_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(my_program, "t"));
    glDrawArrays(GL_TRIANGLES, 0, 6*b->d);
    glUseProgram(0);
}

void renderGlyph(int frame, float x, float y, float w, float h, GLuint tex, int frameW, int frameH, float rot, float centerX, float centerY, float alpha, GLuint texN, float depth, float strength, bool flipX, bool flipY, int cutoff = -1, int cutoff2 = -1, int cutoff3 = -1, int cutoff4 = -1) {
    float offX = 0;
    float offY = 0;
    int o = cutoff-x;
    int o2 = x+w-cutoff2;
    int o3 = cutoff3-y;
    int o4 = y+h-cutoff4;
    bool disabled = false;
    if(cutoff != -1) {
        if(o > 0 && o < w) {
            int tx = frame%5;
            x = cutoff;
            float ratio = w/frameW;
            w = w-o;
            frameW = w/ratio;
            offX += o*(tx+1)/ratio;
        } else if(o >= w) {
            disabled = true;
        } else {

        }
    }
    if(cutoff2 != -1) {
        if(o2 > 0 && o2 < w) {
            int tx = frame%5;
            float ratio = w/frameW;
            w = w-o2;
            frameW = w/ratio;
            offX += o2*(tx)/ratio;
        } else if(o2 >= w) {
            disabled = true;
        } else {

        }
    }

    if(cutoff3 != -1) {
        if(o3 > 0 && o3 < h) {
            int ty = frame/5;
            y = cutoff3;
            float ratio = h/frameH;
            h = h-o3;
            frameH = h/ratio;
            offY += o3*(ty+1)/ratio;
        } else if(o3 >= h) {
            disabled = true;
        } else {

        }
    }
    if(cutoff4 != -1) {
        if(o4 > 0 && o4 < h) {
            int ty = frame/5;
            float ratio = h/frameH;
            h = h-o4;
            frameH = h/ratio;
            offY += o4*(ty)/ratio;
        } else if(o4 >= h) {
            disabled = true;
        } else {

        }
    }
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUniform1i(mp_frame, frame);
    glUniform2f(mp_frameSize, (float)frameW, (float)frameH);
    glUniform2f(mp_off, (float)offX, (float)offY);
    glUniformMatrix4fv(mp_model, 1, GL_FALSE, glm::value_ptr(currentMatrix));
    if(!disabled) {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void renderSprite2(int frame, float x, float y, float w, float h, GLuint tex, float frameW, float frameH, float rot, float centerX, float centerY, float alpha, GLuint texN, float depth, float strength, bool flipX, bool flipY, float offX, float offY, int cutoff, int cutoff2, int cutoff3, int cutoff4) {
    int tw, th;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, postImg);
    int o = cutoff-x;
    int o2 = x+w-cutoff2;
    int o3 = cutoff3-y;
    int o4 = y+h-cutoff4;
    bool disabled = false;
    if(cutoff != -1) {
        if(o > 0 && o < w) {
            int tx = frame%5;
            x = cutoff;
            float ratio = w/frameW;
            w = w-o;
            frameW = w/ratio;
            offX += o*(tx+1)/ratio;
        } else if(o >= w) {
            disabled = true;
        } else {

        }
    }
    if(cutoff2 != -1) {
        if(o2 > 0 && o2 < w) {
            int tx = frame%5;
            float ratio = w/frameW;
            w = w-o2;
            frameW = w/ratio;
            offX += o2*(tx)/ratio;
        } else if(o2 >= w) {
            disabled = true;
        } else {

        }
    }

    if(cutoff3 != -1) {
        if(o3 > 0 && o3 < h) {
            int ty = frame/5;
            y = cutoff3;
            float ratio = h/frameH;
            h = h-o3;
            frameH = h/ratio;
            offY += o3*(ty+1)/ratio;
        } else if(o3 >= h) {
            disabled = true;
        } else {

        }
    }
    if(cutoff4 != -1) {
        if(o4 > 0 && o4 < h) {
            int ty = frame/5;
            float ratio = h/frameH;
            h = h-o4;
            frameH = h/ratio;
            offY += o4*(ty)/ratio;
        } else if(o4 >= h) {
            disabled = true;
        } else {

        }
    }
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    if(rot != 0) {
        currentMatrix = glm::translate(currentMatrix, glm::vec3(centerX, centerY, 0.0));
        currentMatrix = glm::rotate(currentMatrix, glm::radians(rot), glm::vec3(0, 0, 1));
        currentMatrix = glm::translate(currentMatrix, glm::vec3(-centerX, -centerY, 0.0));
    }
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(transition_program);
    glUniform1i(glGetUniformLocation(transition_program, "tex"), 0);
    glUniform1i(glGetUniformLocation(transition_program, "texP"), 1);
    glUniform2f(glGetUniformLocation(transition_program, "frameSize"), (float)frameW, (float)frameH);
    glUniform2f(glGetUniformLocation(transition_program, "texSize"), (float)tw, (float)th);
    glUniform1f(glGetUniformLocation(transition_program, "rot"), -45.0*M_PI/180.0);
    glUniform2f(glGetUniformLocation(transition_program, "off"), (float)offX, (float)offY);
    glUniform1i(glGetUniformLocation(transition_program, "frame"), frame);
    glUniform1f(glGetUniformLocation(transition_program, "depth"), depth);
    glUniform1f(glGetUniformLocation(transition_program, "alpha"), alpha);
    glUniform1f(glGetUniformLocation(transition_program, "strength"), strength);
    glUniformMatrix4fv(glGetUniformLocation(transition_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniform2f(glGetUniformLocation(transition_program, "flip"), (float)flipX, (float)flipY);
    glUniformMatrix4fv(glGetUniformLocation(transition_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(transition_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(transition_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(transition_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(transition_program, "t"));
    if(!disabled) {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glUseProgram(0);
}

void renderSprite(int frame, float x, float y, float w, float h, GLuint tex, float frameW, float frameH, float rot, float centerX, float centerY, float alpha, GLuint texN, float depth, float strength, bool flipX, bool flipY, float offX, float offY, int cutoff, int cutoff2, int cutoff3, int cutoff4) {
    int tw, th;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
    glActiveTexture(GL_TEXTURE1);
    if(texN == 0) {
        glBindTexture(GL_TEXTURE_2D, noTex);
    } else {
        glBindTexture(GL_TEXTURE_2D, texN);
    }
    int o = cutoff-x;
    int o2 = x+w-cutoff2;
    int o3 = cutoff3-y;
    int o4 = y+h-cutoff4;
    bool disabled = false;
    if(cutoff != -1) {
        if(o > 0 && o < w) {
            int tx = frame%5;
            x = cutoff;
            float ratio = w/frameW;
            w = w-o;
            frameW = w/ratio;
            offX += o*(tx+1)/ratio;
        } else if(o >= w) {
            disabled = true;
        } else {

        }
    }
    if(cutoff2 != -1) {
        if(o2 > 0 && o2 < w) {
            int tx = frame%5;
            float ratio = w/frameW;
            w = w-o2;
            frameW = w/ratio;
            offX += o2*(tx)/ratio;
        } else if(o2 >= w) {
            disabled = true;
        } else {

        }
    }

    if(cutoff3 != -1) {
        if(o3 > 0 && o3 < h) {
            int ty = frame/5;
            y = cutoff3;
            float ratio = h/frameH;
            h = h-o3;
            frameH = h/ratio;
            offY += o3*(ty+1)/ratio;
        } else if(o3 >= h) {
            disabled = true;
        } else {

        }
    }
    if(cutoff4 != -1) {
        if(o4 > 0 && o4 < h) {
            int ty = frame/5;
            float ratio = h/frameH;
            h = h-o4;
            frameH = h/ratio;
            offY += o4*(ty)/ratio;
        } else if(o4 >= h) {
            disabled = true;
        } else {

        }
    }
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    if(rot != 0) {
        currentMatrix = glm::translate(currentMatrix, glm::vec3(centerX, centerY, 0.0));
        currentMatrix = glm::rotate(currentMatrix, glm::radians(rot), glm::vec3(0, 0, 1));
        currentMatrix = glm::translate(currentMatrix, glm::vec3(-centerX, -centerY, 0.0));
    }
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(my_program);
    glUniform1i(mp_tex, 0);
    glUniform1i(mp_texN, 1);
    glUniform2f(mp_frameSize, (float)frameW, (float)frameH);
    glUniform2f(mp_texSize, (float)tw, (float)th);
    glUniform2f(mp_off, (float)offX, (float)offY);
    glUniform1i(mp_frame, frame);
    glUniform1f(mp_depth, depth);
    glUniform1f(mp_alpha, alpha);
    glUniform1f(mp_strength, strength);
    glUniformMatrix4fv(mp_model, 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniform2f(mp_flip, (float)flipX, (float)flipY);
    glUniformMatrix4fv(mp_proj, 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(my_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(my_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(my_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(my_program, "t"));
    if(!disabled) {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glUseProgram(0);
}

void renderNumber(int number, int x, int y, GLuint tex, bool percent, bool alignRight, int cutoff = 0) {
    int kerning[] = {3, 5, 3, 3, 3, 3, 3, 4, 3, 3, 2, 4};
    bool negative = false;
    if(number < 0) {
        negative = true;
        number *= -1;
    }
    int numDigits = 1;
    if(number > 0) {
        numDigits = log10(number)+1;
    }
    int f = number;
    int w = -1;
    vector<int> digits;
    for(int d = 0; d < numDigits; d++) {
        digits.push_back(f%10);
        w += 9-kerning[f%10];
        f /= 10;
    }
    if(percent) {
        w += 9-kerning[10];
    }
    int k = 0;
    for(int d = digits.size()-1; d >= 0; d--) {
        renderSprite(digits.at(d)+55, x+9*(digits.size()-1-d)-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, 0, 0, cutoff);
        k += kerning[digits.at(d)];
    }
    if(percent) {
        renderSprite(53, x+9*digits.size()-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, 0, 0, cutoff);
    }
    if(negative) {
        renderSprite(54, x+9*-1+kerning[11]-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, 0, 0, cutoff);
    }
}

int renderText(string text, int x, int y, GLuint tex, bool alignRight, int cutoff = -1, int cutoff2 = -1, int cutoff3 = -1, int cutoff4 = -1, float alpha = 1.0) {
    int kerning[] =    {2, 2, 2, 2, 3,
                        3, 2, 2, 6, 3,
                        3, 3, 1, 3, 2,
                        2, 2, 2, 3, 2,
                        3, 2, 1, 3, 2,
                        2, 2, 2, 2, 2,
                        2, 3, 2, 2, 5,
                        4, 3, 5, -1, 2,
                        2, 2, 2, 2, 3,
                        3, 2, 2, 1, 3,
                        2, 2, 2, 2, 6,

                        6, 5, 3, 3, 2,
                        2, 6, 5, 5, 3,
                        3, 6, 4, 6, 2,
                        6, 6, 3, 4, 3,
                        2, 1, 5, 4, 5,
                        5, 4, 4, 6, 4,
                        4};
    int kerningD[] = {3, 5, 3, 3, 3, 3, 3, 4, 3, 3, 2, 4};
    int w = -1;
    for(int i = 0; i < text.size(); i++) {
        if(text[i] >= 33 && text[i] <= 47) {
            w += 9-kerning[55+text[i]-33];
        } else if(text[i] >= 48 && text[i] <= 57) {
            w += 9-kerningD[text[i]-48];
        } else if(text[i] >= 58 && text[i] <= 64) {
            w += 9-kerning[70+text[i]-58];
        } else if(text[i] >= 91 && text[i] <= 95) {
            w += 9-kerning[77+text[i]-91];
        } else if(text[i] >= 123 && text[i] <= 126) {
            w += 9-kerning[82+text[i]-123];
        } else if(text[i] == 32) {
            w += 9-kerning[52];
        } else if((text[i] >= 65 && text[i] <= 90) || (text[i] >= 97 && text[i] <= 122)) {
            int f = text[i] - 'A';
            if(f >= 32) {
                f -= 6;
            }
            w += 9-kerning[f];
        } else {
            w += 9-kerning[0];
        }
    }
    int wd = w;
    if(!alignRight) {
        w = 0;
    }
    int k = 0;

    int tw, th;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
    glUseProgram(my_program);
    glUniform1i(mp_tex, 0);
    glUniform1i(mp_texN, 1);
    glUniform2f(mp_texSize, (float)tw, (float)th);
    glUniform1f(mp_depth, 0.0);
    glUniform1f(mp_strength, 0.0);
    glUniform1f(mp_alpha, alpha);
    glUniform2f(mp_flip, (float)0, (float)0);
    glUniformMatrix4fv(mp_proj, 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(my_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(my_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(my_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(my_program, "t"));

    int posX = 0;
    for(int i = 0; i < text.size(); i++) {
        if(text[i] == '\n') {
            y += 9;
            k = 0;
            posX = 0;
        } else {
            if(text[i] >= 33 && text[i] <= 47) {
                renderGlyph(55+text[i]-33+10, x+posX-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
                k += kerning[55+text[i]-33];
            } else if(text[i] >= 48 && text[i] <= 57) {
                renderGlyph(55+text[i]-48, x+posX-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
                k += kerningD[text[i]-48];
            } else if(text[i] >= 58 && text[i] <= 64) {
                renderGlyph(70+text[i]-58+10, x+posX-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
                k += kerning[70+text[i]-58];
            } else if(text[i] >= 91 && text[i] <= 95) {
                renderGlyph(77+text[i]-91+10, x+posX-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
                k += kerning[77+text[i]-91];
            } else if(text[i] >= 123 && text[i] <= 126) {
                renderGlyph(82+text[i]-123+10, x+posX-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
                k += kerning[82+text[i]-123];
            } else if(text[i] == 32) {
                renderGlyph(52, x+posX-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
                k += kerning[52];
            } else if((text[i] >= 65 && text[i] <= 90) || (text[i] >= 97 && text[i] <= 122)) {
                int f = text[i] - 'A';
                if(f >= 32) {
                    f -= 6;
                }
                renderGlyph(f, x+posX-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
                k += kerning[f];
            } else {
                //renderGlyph(f, x+9*i-k-w, y, 9, 9, tex, 9, 9, 0, 0, 0, 1, 0, 0, 0.0, false, false);
                k += kerning[0];
            }
            posX += 9;
        }
    }

    glUseProgram(0);

    return wd;
}

void renderScreen() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, screenTexN);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, screenTexMisc);
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(0, screenH*screenScale, 0.0));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(screenW, -screenH, 1.0));
    glUseProgram(light_program);
    glUniform1i(glGetUniformLocation(light_program, "tex"), 0);
    glUniform1i(glGetUniformLocation(light_program, "texN"), 1);
    glUniform1i(glGetUniformLocation(light_program, "texMisc"), 2);
    glUniform2f(glGetUniformLocation(light_program, "frameSize"), 1.0, 1.0);
    glUniform2f(glGetUniformLocation(light_program, "resolution"), (float)screenW, (float)screenH);
    glUniform1f(glGetUniformLocation(light_program, "timex"), 0);
    glUniform1f(glGetUniformLocation(light_program, "timey"), 0);
    glUniform1f(glGetUniformLocation(light_program, "tileSize"), 16);
    glUniform2f(glGetUniformLocation(light_program, "texSize"), 1.0, 1.0);
    glUniform1i(glGetUniformLocation(light_program, "frame"), 0);
    glUniformMatrix4fv(glGetUniformLocation(light_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniformMatrix4fv(glGetUniformLocation(light_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(light_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(light_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(light_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(light_program, "t"));
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glUniform3f(glGetUniformLocation(light_program, "lightPos"), 0, 0, 2.0);
    glUniform1f(glGetUniformLocation(light_program, "lightRadius"), 0.0);
    glUniform4f(glGetUniformLocation(light_program, "lightColor"), 0, 0, 0, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(0);
}

void renderRotIcon(int x, int y, int w, int h, GLuint tex, int frame) {
    renderSprite(frame, x, y, w, h, tex, w, h, rotTimer, w/2.0, h/2.0, 1, 0, 0, 0.0, false, false);
}

void renderIcon(int x, int y, int w, int h, GLuint tex, int frame, int offX = 0, int offY = 0) {
    renderSprite(frame, x, y, w, h, tex, w, h, 0, 0, 0, 1, 0, 0, 0.0, false, false, offX*(frame+1), offY*(frame+1));
}

void renderUI(int x, int y, int w, int h, GLuint tex, int cutoff = -1, int cutoff2 = -1, int cutoff3 = -1, int cutoff4 = -1) {
    if(w > 0 && h > 0) {
        int tw, th;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
        glUseProgram(my_program);
        glUniform1i(mp_tex, 0);
        glUniform1i(mp_texN, 1);
        glUniform2f(mp_texSize, (float)tw, (float)th);
        glUniform1f(mp_depth, 0.0);
        glUniform1f(mp_strength, 0.0);
        glUniform2f(mp_flip, (float)0, (float)0);
        glUniformMatrix4fv(mp_proj, 1, GL_FALSE, glm::value_ptr(proj));
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(glGetAttribLocation(my_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
        glEnableVertexAttribArray(glGetAttribLocation(my_program, "position"));
        glVertexAttribPointer(glGetAttribLocation(my_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
        glEnableVertexAttribArray(glGetAttribLocation(my_program, "t"));

        renderGlyph(0, x, y, 2, 2, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(1, x+2, y, w-4, 2, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(2, x+w-2, y, 2, 2, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(3, x, y+2, 2, h-4, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(4, x+2, y+2, w-4, h-4, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(5, x+w-2, y+2, 2, h-4, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(6, x, y+h-2, 2, 2, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(7, x+2, y+h-2, w-4, 2, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
        renderGlyph(8, x+w-2, y+h-2, 2, 2, tex, 2, 2, 0, 0, 0, 1, 0, 0, 0.0, false, false, cutoff, cutoff2, cutoff3, cutoff4);
    }
}

void renderWheel(int x, int y, int w, int h, float v) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, palImgReal);
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(blur_program);
    glUniform2f(glGetUniformLocation(blur_program, "frameSize"), 16, 16);
    glUniform2f(glGetUniformLocation(blur_program, "texSize"), 16, 16);
    glUniform1i(glGetUniformLocation(blur_program, "tex"), 0);
    v /= 50.0;
    if(v > 1.0) {
        v = 1.0;
    } else if(v < 0.0) {
        v = 0.0;
    }
    glUniform1i(glGetUniformLocation(blur_program, "tri"), false);
    glUniform1i(glGetUniformLocation(blur_program, "full"), false);
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(blur_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(blur_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "t"));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUseProgram(0);

}


void renderWheelTri(int x, int y, int w, int h, float v) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, palImgReal);
    float r = 0.5*sqrt(3)*2*w/3.0;
    float d = r*2/w;
    float a = 0.4;
    x += w/2.0;
    y += h/2.0;
    w /= d;
    h /= d;
    w *= a*2;
    h *= a*2;
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    currentMatrix = glm::rotate(currentMatrix, glm::radians(float(v)), glm::vec3(0, 0, 1));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(blur_program);
    glUniform2f(glGetUniformLocation(blur_program, "frameSize"), 16, 16);
    glUniform2f(glGetUniformLocation(blur_program, "texSize"), 16, 16);
    glUniform1i(glGetUniformLocation(blur_program, "tex"), 0);
    glUniform1i(glGetUniformLocation(blur_program, "tri"), true);
    glUniform1i(glGetUniformLocation(blur_program, "full"), false);
    glUniform1f(glGetUniformLocation(blur_program, "v"), v);
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vboTri);
    glVertexAttribPointer(glGetAttribLocation(blur_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(blur_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "t"));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);

}

void renderWheelF(int x, int y, int w, int h, float v) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, palImg);
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(blur_program);
    glUniform2f(glGetUniformLocation(blur_program, "frameSize"), 16, 16);
    glUniform2f(glGetUniformLocation(blur_program, "texSize"), 16, 16);
    glUniform1i(glGetUniformLocation(blur_program, "tex"), 0);
    v /= 50.0;
    if(v > 1.0) {
        v = 1.0;
    } else if(v < 0.0) {
        v = 0.0;
    }
    glUniform1i(glGetUniformLocation(blur_program, "tri"), false);
    glUniform1i(glGetUniformLocation(blur_program, "full"), true);
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(blur_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(blur_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "t"));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUseProgram(0);

}


void renderWheelTriF(int x, int y, int w, int h, float v) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, palImg);
    float r = 0.5*sqrt(3)*2*w/3.0;
    float d = r*2/w;
    float a = 0.4;
    x += w/2.0;
    y += h/2.0;
    w /= d;
    h /= d;
    w *= a*2;
    h *= a*2;
    glm::mat4 currentMatrix = model;
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, 0.0));
    currentMatrix = glm::rotate(currentMatrix, glm::radians(float(v)), glm::vec3(0, 0, 1));
    currentMatrix = glm::scale(currentMatrix, glm::vec3(w, h, 1.0));
    glUseProgram(blur_program);
    glUniform2f(glGetUniformLocation(blur_program, "frameSize"), 16, 16);
    glUniform2f(glGetUniformLocation(blur_program, "texSize"), 16, 16);
    glUniform1i(glGetUniformLocation(blur_program, "tex"), 0);
    glUniform1i(glGetUniformLocation(blur_program, "tri"), true);
    glUniform1i(glGetUniformLocation(blur_program, "full"), true);
    glUniform1f(glGetUniformLocation(blur_program, "v"), v);
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "model"), 1, GL_FALSE, glm::value_ptr(currentMatrix));
    glUniformMatrix4fv(glGetUniformLocation(blur_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vboTri);
    glVertexAttribPointer(glGetAttribLocation(blur_program, "position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(blur_program, "t"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(blur_program, "t"));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);

}

void renderGL() {
    int a;
    a = barX;
    int barX = a;
    a = barY;
    int barY = a;
    a = barXRight;
    int barXRight = a;
    a = barY2;
    int barY2 = a;
    a = barY3;
    int barY3 = a;
    a = barY4;
    int barY4 = a;
    a = barY5;
    int barY5 = a;
    float sDepth = 0.2;
    glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);
    glClear(GL_COLOR_BUFFER_BIT);

    model = glm::translate(glm::mat4(1.0), glm::vec3(-camXFinal, -camYFinal, 0.0));
    model = glm::translate(model, glm::vec3(0, screenH*(screenScale-1.0), 0.0));

    renderUI(0, 0, screenW, screenH, effectImg);
    int x = int(nodeCX)%16;
    int y = int(nodeCY)%16;
    renderSprite(0, x-16, y-16, screenW+32, screenH+32, gridImg, screenW+32, screenH+32, 0, 0, 0, 1.0, 0, 0, 0, 0, 0);

    Texture* t = texs.at(currentTexture);

    int h = -layersScroll;
    int maxW = -layersScrollH;

    int scrollW = 6;

    //file browser
    if(browserOpen) {
        renderUI(barX+1, 0, screenW-barX-barXRight-2, barY-1, effectImg5);
        int title = collH-4;
        int bSpace = 5;
        float tS = title/2-4;
        renderUI(barX+1+bSpace*4+8*3, bSpace, screenW-barX-barXRight-2-bSpace*6-8*4, title, effectImg6);
        int ln = textW(currentDir, barX+1+bSpace*4+8*3+tS, bSpace+tS, fontImg, 0);
        if(ln > screenW-barX-barXRight-2-bSpace*6-8*4-tS*2) {
            renderText(currentDir, screenW-barXRight-1-bSpace*2-8-tS, bSpace+tS, fontImg, 1);
        } else {
            renderText(currentDir, barX+1+bSpace*4+8*3+tS, bSpace+tS, fontImg, 0);
        }
        renderUI(barX+1+bSpace, title+bSpace*2, screenW-barX-barXRight-2-bSpace*3-8, title, effectImg6);
        if(overwrite) {
            renderText(filenameB, barX+1+bSpace+tS, bSpace*2+tS+title, fontImg2, 0);
        } else {
            renderText(filenameB, barX+1+bSpace+tS, bSpace*2+tS+title, fontImg, 0);
        }
        renderUI(barX+1+bSpace, title*2+bSpace*3, screenW-barX-barXRight-2-bSpace*2, barY-1-bSpace*4-title*2, effectImg6);
        int fh = tS;
        int fw = tS-browserScroll;
        int fwNext = 0;
        for(int i = 0; i < filenames.size(); i++) {
            int n = textW(filenames.at(i)->name, barX+1+bSpace+fw+tS+8, title*2+bSpace*3+fh, fontImg, 0);
            if(barX+1+bSpace+fw-tS+n+8*2+tS > barX && barX+1+bSpace+fw-tS < screenW-barXRight) {
                if(i == selectedFile) {
                    renderUI(barX+1+bSpace+fw-tS, title*2+bSpace*3+fh-tS, n+8*2+tS, title, effectImg5);
                }
                renderIcon(barX+1+bSpace+fw, title*2+bSpace*3+fh, 8, 8, iconImg9, filenames.at(i)->folder);
                renderText(filenames.at(i)->name, barX+1+bSpace+fw+tS+8, title*2+bSpace*3+fh, fontImg, 0);
            }

            if(n > fwNext) {
                fwNext = n;
            }
            fh += title;
            if(fh+title*2+title+bSpace*3 > title+bSpace*2+barY-1-bSpace*3-title-6 && i != filenames.size()-1) {
                fh = tS;
                fw += fwNext+tS*2+8*3;
                fwNext = 0;
            }
        }
        fw += fwNext+tS*2+8*3;
        int scrollH = fw+browserScroll;
        if(scrollH < screenW-barX-barXRight-bSpace*2) {
            scrollH = screenW-barX-barXRight-bSpace*2;
        }
        renderUI(barX+bSpace+1+int(int(browserScroll)*(screenW-barX-barXRight-bSpace*2)/float(scrollH)), barY-bSpace-scrollW-1, int((screenW-barX-barXRight-bSpace*2)*(screenW-barX-barXRight-bSpace*2)/float(scrollH)), scrollW, effectImg14);


        renderUI(barX, 0, bSpace, barY-1, effectImg);
        renderUI(screenW-barXRight-bSpace, 0, bSpace, barY-1, effectImg);
        renderUI(barX+1, 0, bSpace, barY-1, effectImg5);
        renderUI(barX+1, 0, bSpace*4+8*3, title+bSpace, effectImg5);
        renderUI(screenW-barXRight-bSpace-1, 0, bSpace, barY-1, effectImg5);
        renderUI(screenW-barXRight-bSpace*2-1-8, 0, bSpace*2+8, title*2+bSpace*2, effectImg5);
        bool leftActive = (fnUndo.size()>1);
        bool rightActive = (fnRedo.size()>0);
        bool upActive = (currentDir.size()!=1);
        renderIcon(barX+1+bSpace, bSpace+tS, 8, 8, iconImg10, 0+leftActive);
        renderIcon(barX+1+bSpace+8+bSpace, bSpace+tS, 8, 8, iconImg10, 2+rightActive);
        renderIcon(barX+1+bSpace+8*2+bSpace*2, bSpace+tS, 8, 8, iconImg10, 4+upActive);
        renderIcon(screenW-barXRight-8-bSpace-1, bSpace+tS, 8, 8, iconImg10, 6);
        int impexp = 0;
        if(browserMode == 1 || browserMode == 2) {
            impexp = 1;
        } else if(browserMode == 3) {
            impexp = 2;
        } else if(browserMode == 4) {
            impexp = 4;
        } else if(browserMode == 5) {
            impexp = 3;
        }
        renderIcon(screenW-barXRight-8-bSpace-1, bSpace*2+title+tS, 8, 8, iconImg7, impexp);
    } else {
        //render nodes
        model = glm::translate(glm::mat4(1.0), glm::vec3(-camXFinal, -camYFinal, 0.0));
        model = glm::translate(model, glm::vec3(0, screenH*(screenScale-1.0), 0.0));
        if(draggingSocket) {
            nEffect* fx = t->fxs.at(draggedNode);
            Socket* out = fx->outputs.at(draggedSocket);
            renderBezier(out->b, bezierFill);
        }
        for(int i = 0; i < t->fxs.size(); i++) {
            nEffect* fx = t->fxs.at(i);
            for(int in = 0; in < fx->inputs.size(); in++) {
                if(fx->inputs.at(in)->s != NULL) {
                    if(fx->inputs.at(in)->infloop) {
                        renderBezier(fx->inputs.at(in)->b, bezierFillError);
                    } else {
                        renderBezier(fx->inputs.at(in)->b, bezierFill);
                    }
                }
            }
        }
        for(int i = 0; i < t->fxs.size(); i++) {
            nEffect* fx = t->fxs.at(i);
            if(fx->x+fx->w+4+int(nodeCX) > barX && fx->x-4+int(nodeCX) < screenW-barXRight && fx->y+fx->h+4+int(nodeCY) > 0 && fx->y-4+int(nodeCY) < screenH) {
                model = glm::translate(model, glm::vec3(fx->x+int(nodeCX)+fx->w/2.0, fx->y+int(nodeCY)+fx->h/2.0, 0.0));
                model = glm::rotate(model, glm::radians(fx->r), glm::vec3(0, 0, 1));
                model = glm::translate(model, glm::vec3(-(fx->x+int(nodeCX)+fx->w/2.0), -(fx->y+int(nodeCY)+fx->h/2.0), 0.0));

                renderUI(fx->x+int(nodeCX), fx->y+int(nodeCY), fx->w, fx->h, effectImg4);
                renderUI(fx->x+int(nodeCX), fx->y+int(nodeCY), fx->w, 9+8, effectImg16);
                renderText(fx->name, fx->x+int(nodeCX)+4, fx->y+int(nodeCY)+4, fontImg, false);
                renderIcon(fx->x+fx->w+int(nodeCX)-8-4, fx->y+int(nodeCY)+4, 8, 8, iconImg3, 1);
                renderIcon(fx->x+fx->w+int(nodeCX)-8-4-8-4, fx->y+int(nodeCY)+4, 8, 8, iconImg3, 2);
                for(int p = 0; p < fx->texts.size(); p++) {
                    renderText(fx->texts.at(p)->name, fx->x+int(nodeCX)+7, fx->y+fx->texts.at(p)->y+int(nodeCY), fontImg, false);
                }
                for(int p = 0; p < fx->params.size(); p++) {
                    fx->params.at(p)->render(fx->x+int(nodeCX), fx->y+int(nodeCY));
                }
                for(int out = 0; out < fx->outputs.size(); out++) {
                    int size = 8;
                    int size2 = 64;
                    renderUI(fx->x+fx->w/2.0+int(nodeCX)-int(size2/2.0), fx->y+int(nodeCY)+fx->outputs.at(out)->y+size/2.0-size2/2.0, size2, size2, effectImg);
                    if(fx->doneTimer < 3) {
                        renderSprite(0, int(fx->x+fx->w/2.0+int(nodeCX)-int(texSizeX/2.0)), int(fx->y+int(nodeCY)+fx->outputs.at(out)->y+size/2.0-texSizeY/2.0), texSizeX, texSizeY, fx->outputs.at(out)->texture, texSizeX, texSizeY, 0, 0, 0, 1, 0, 0, 0, 0, 0);
                    } else {
                        renderRotIcon(int(fx->x+fx->w/2.0+int(nodeCX)-4), int(fx->y+int(nodeCY)+fx->outputs.at(out)->y+size/2.0-4), 8, 8, iconImg8, 0);
                    }
                    renderIcon(fx->x+fx->w+int(nodeCX)-size/2.0, fx->y+int(nodeCY)+fx->outputs.at(out)->y, size, size, iconImg6, 1);
                }
                for(int in = 0; in < fx->inputs.size(); in++) {
                    int size = 8;
                    renderIcon(fx->x+int(nodeCX)-size/2.0, fx->y+int(nodeCY)+fx->inputs.at(in)->y, size, size, iconImg6, 1);
                }

                model = glm::translate(glm::mat4(1.0), glm::vec3(-camXFinal, -camYFinal, 0.0));
                model = glm::translate(model, glm::vec3(0, screenH*(screenScale-1.0), 0.0));
            }
        }
    }

    renderUI(0, 0, barX, barY, effectImg8);

    if(view == 0) {
        int newEScrolli = newEScroll;
        for(int i = 0; i < newEffects.size(); i++) {
            nEffect* fx = newEffects.at(i);
            if(draggingFX && draggedLayer == -1 && i == newEffects.size()-1) {

            } else {
                renderUI(0, collH*i-newEScrolli, barX-scrollW-1, collH, effectImg2);
                renderText(fx->name, 5, 5+collH*i-newEScrolli, fontImg, 0, -1, -1, -1, barY-1);
                if(fx->isPreset) {
                    renderIcon(barX-scrollW-1-8-4-2, 5+collH*i-newEScrolli, 8, 8, iconImg10, 7);
                }
            }
        }
        int count = newEffects.size();
        if(draggingFX && draggedLayer == -1) {
            count--;
        }
        int scrollH = collH*count;
        if(scrollH < barY) {
            scrollH = barY;
        }
        renderUI(barX-scrollW, 0, scrollW, barY-1, effectImg8);
        renderUI(barX-scrollW-1, 0, scrollW, barY+1, effectImg15);
        renderUI(barX-scrollW, int(int(newEScroll)*(barY-2)/float(scrollH)), scrollW, int((barY)*(barY-2)/float(scrollH))+1, effectImg14);
    }

    //right bar background (including lines)
    int toolsScrolli = toolsScroll;
    int palScrolli = palScroll;
    renderUI(screenW-int(barXRight), -toolsScrolli, int(barXRight), barY2, effectImg3);
    renderUI(screenW-int(barXRight), barY2-toolsScrolli, int(barXRight), 3+8+3+(14*1)+4, effectImg3);
    renderUI(screenW-int(barXRight), barY2-toolsScrolli+3+8+3+(14*1)+4, int(barXRight), screenH-(barY2-toolsScrolli+3+8+3+(14*1)+4)+2, effectImg3);

    renderText("Palette", screenW-barXRight+4, 3-toolsScrolli, fontImg, 0);
    renderUI(screenW-barXRight+4, 2+12-toolsScrolli, barXRight-8-7, barY3, effectImg);
    renderUI(screenW-barXRight+5, 3+12-toolsScrolli, barXRight-10-7, barY3-2-5-8-5, effectImg5);
    int space = 1;
    float offset = 0;
    int w = barXRight-10+space-offset*2-7-7;
    float size = 12;
    int c = w/size;
    int leftspace = w-size*c;
    leftspace = 0;
    int maxHPal = 0;
    for(int p = 0; p < palette.size(); p++) {
        int x = p%c;
        int y = p/c;
        int offset2 = int(p==selectedColor)*2;
        renderUI(leftspace+offset-1+x*(size)+screenW-barXRight+5, offset-1+3+y*(size)+12-toolsScrolli-palScrolli, int((x+1)*(size))-int(x*(size))-space+2, int((y+1)*(size))-int(y*(size))-space+2, effectImg11, -1, -1, 3+12-toolsScrolli, barY3-2-5-8-5+3+12-toolsScrolli);
        renderColor(leftspace+offset+offset2+x*(size)+screenW-barXRight+5, offset+offset2+3+y*(size)+12-toolsScrolli-palScrolli, size-space-offset2*2, size-space-offset2*2, palette.at(p), -1, -1, 3+12-toolsScrolli, barY3-2-5-8-5+3+12-toolsScrolli);
        if(y*(size)+size > maxHPal) {
            maxHPal = y*(size)+size;
        }
    }

    int scrollH = maxHPal;
    if(scrollH < barY3-5-8-5) {
        scrollH = barY3-5-8-5;
    }
    renderUI(screenW-4-8-scrollW, 2+12-toolsScrolli+1, scrollW, barY3-5-8-5-2, effectImg5);
    renderUI(screenW-4-8-scrollW-1, 2+12-toolsScrolli+1, scrollW, barY3-5-8-5+2, effectImg10);
    renderUI(screenW-4-8-scrollW, 2+12-toolsScrolli+int(int(palScroll)*(barY3-5-8-5)/float(scrollH))+1, scrollW, int((barY3-5-8-5)*(barY3-5-8-5-3)/float(scrollH))+1, effectImg14);

    renderIcon(screenW-5-8-5-7, 1+12-toolsScrolli+barY3-8-5, 8, 8, iconImg7, 1);
    renderIcon(screenW-5-8-5-5-8-7, 1+12-toolsScrolli+barY3-8-5, 8, 8, iconImg7, 0);
    renderIcon(screenW-5-8-5-5-8-5-8-7, 1+12-toolsScrolli+barY3-8-5, 8, 8, iconImg3, 1);
    renderIcon(screenW-5-8-5-5-8-5-8-5-8-7, 1+12-toolsScrolli+barY3-8-5, 8, 8, iconImg3, 2);
    renderIcon(screenW-5-8-5-5-8-5-8-5-8-5-8-7, 1+12-toolsScrolli+barY3-8-5, 8, 8, iconImg3, 0);
    Parameter* red = colorParams.at(1);
    Parameter* blue = colorParams.at(3);
    Parameter* hue = colorParams.at(4);
    int ss = 36;
    int paramH = ss/3.0;
    int off = (paramH-7)/4.0;
    renderUI(screenW-barXRight+red->x-off, barY3+2+red->y-off+12-toolsScrolli, red->w+off*2, (blue->h+blue->y)-red->y+off*2, effectImg6);
    renderUI(screenW-barXRight+hue->x-off, barY3+2+red->y-off+12-toolsScrolli, red->w+off*2, (blue->h+blue->y)-red->y+off*2, effectImg6);
    renderText("Tile Properties", screenW-barXRight+4, barY2+3-toolsScrolli, fontImg, 0);
    
    renderText("Node Preview", screenW-barXRight+4, barY2+3+3+8+3+(14*1)+4-toolsScrolli, fontImg, 0);
    renderUI(screenW-barXRight+4, barY2+3+8+3+(14*1)+4-toolsScrolli+2+12, barXRight-8-7, 96, effectImg);
    renderUI(screenW-barXRight+5, barY2+3+8+3+(14*1)+4-toolsScrolli+3+12, barXRight-10-7, 96-2, effectImg5);
    
    if(preview != NULL && !browserOpen) {
        int tileW = texSizeX;
        int tileH = texSizeY;
        while(tileW < barXRight-10-7) {
            tileW += texSizeX;
        }
        while(tileH < 96-2) {
            tileH += texSizeY;
        }
        float d = 0.3;
        int xOff = -previewTimer*d;
        while(xOff < -tileW) {
            xOff += tileW;
        }
        int yOff = -previewTimer/2*d;
        while(yOff < -tileH) {
            yOff += tileH;
        }
        if(preview->parent->doneTimer < 3) {
            renderSprite(0, screenW-barXRight+5+xOff, barY2+3+8+3+(14*1)+4-toolsScrolli+3+12+yOff, tileW*2, tileH*2, preview->texture, tileW*2, tileH*2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, screenW-barXRight+5, screenW-barXRight+5+barXRight-10-7, barY2+3+8+3+(14*1)+4-toolsScrolli+3+12, barY2+3+8+3+(14*1)+4-toolsScrolli+3+12+96-2);
        } else {
            renderRotIcon(screenW-barXRight+5+(barXRight-8-7)/2-4, barY2+3+8+3+(14*1)+4-toolsScrolli+3+12+(96-2)/2-4, 8, 8, iconImg8, 0);
        }
    } else {
        string txt = "Hover on a node";
        int txtW = textW(txt, 0, 0, fontImg, 0);
        renderText(txt, screenW-barXRight+4+(barXRight-8-7-txtW)/2, barY2+3+8+3+(14*1)+4-toolsScrolli+2+12+48-4, fontImg, 0);
    }
    
    for(int p = 0; p < colorParams.size(); p++) {
        colorParams.at(p)->render(screenW-barXRight, barY3+2+12-toolsScrolli);
    }

    scrollH = barY5;
    if(scrollH < barY) {
        scrollH = barY;
    }
    renderUI(screenW-scrollW, 0, scrollW, barY-1, effectImg8);
    renderUI(screenW-scrollW-1, 0, scrollW, barY+1, effectImg15);
    renderUI(screenW-scrollW, int(int(toolsScroll)*(barY-2)/float(scrollH)), scrollW, int((barY)*(barY-2)/float(scrollH))+1, effectImg14);

    if(!browserOpen) {

        int d3 = barX-(screenW-5-8-barXRight);
        if(d3 > 0 && d3 < 8) {
            renderIcon(barX, 5, screenW-5-barXRight-barX, 8, iconImg9, 2, d3);
            renderIcon(barX, 5+8+5, screenW-5-barXRight-barX, 8, iconImg9, 3, d3);
        } else if(d3 >= 8) {

        } else {
            renderIcon(screenW-5-8-barXRight, 5, 8, 8, iconImg9, 2);
            renderIcon(screenW-5-8-barXRight, 5+8+5, 8, 8, iconImg9, 3);
        }
        //renderIcon(screenW-5-8-barXRight, 5, 8, 8, iconImg9, 2);
        int d2 = barX-(screenW-5-8-5-8-barXRight);
        if(d2 > 0 && d2 < 8) {
            renderIcon(barX, 5, screenW-5-8-5-barXRight-barX, 8, iconImg9, 1, d2);
        } else if(d2 >= 8) {

        } else {
            renderIcon(screenW-5-8-5-8-barXRight, 5, 8, 8, iconImg9, 1);
        }
        int d1 = barX-(screenW-5-8-5-8-5-8-barXRight);
        if(d1 > 0 && d1 < 8) {
            renderIcon(barX, 5, screenW-5-8-5-8-5-barXRight-barX, 8, iconImg9, 0, d1);
        } else if(d1 >= 8) {

        } else {
            renderIcon(screenW-5-8-5-8-5-8-barXRight, 5, 8, 8, iconImg9, 0);
        }
        renderText(toolTip, barX+4, 4, fontImg, false, -1, screenW-barXRight, -1, -1);
        int errorH = textH(errorMessage, 0, 0, fontImg2, false);
        float errorA = fmin(1.0, errorTimer/30.0);
        renderText(errorMessage, barX+4, screenH-errorH-4, fontImg2, false, -1, screenW-barXRight, -1, -1, errorA);
        if(logoTimer > 0) {
            float alpha = fmin(1.0, logoTimer/float(logoTimerMax-120));
            renderSprite(0, int((screenW-barX-barXRight)/2.0+barX-128/2.0), int(barY/2.0-79/2.0), 128, 79, logoImage, 128, 79, 0, 0, 0, alpha, 0, sDepth, 0.0, false, false, 0, 0, barX, screenW-barXRight, -1, barY);
        }
    }

    int cursorType = 0;
    bool noChange = false;
    for(int i = 0; i < newEffects.size(); i++) {
        if(mouseX > 2 && mouseX <= barX-2 && mouseY > 2+(collH-2)*i-newEScroll && mouseY <= 2+(collH-2)*i-newEScroll+collH-4 && mouseY < barY) {
            noChange = true;
        }
    }
    if(mouseX > 0 && mouseX < screenW && mouseY > barY && mouseY < screenH) {
        Texture* t = texs.at(currentTexture);
        int h = -layersScroll;
    }
    if(mouseX > barX && mouseX < barX+UIdragRange && mouseY < barY && !noChange) {
        cursorType = 1;
    }
    if(mouseX > screenW-barXRight-UIdragRange && mouseX < screenW-barXRight && mouseY < barY && !noChange) {
        cursorType = 1;
    }
    if(draggingUI && UIdragType == 0) {
        cursorType = 1;
    }
    if(draggingUI && UIdragType == 2) {
        cursorType = 1;
    }
    if(cursorType == 1) {
        renderIcon(mouseX-4, mouseY-1, 8, 8, iconImg11, cursorType);
    } else if(cursorType == 2) {
        renderIcon(mouseX-1, mouseY-4, 8, 8, iconImg11, cursorType);
    } else {
        renderIcon(mouseX, mouseY, 8, 8, iconImg11, cursorType);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    model = glm::mat4(1.0);
    renderSprite(0, 0, screenH*screenScale, screenW*screenScale, -screenH*screenScale, screenTex, screenW, screenH, 0, 0, 0, 1, 0, sDepth, 0.0, false, false);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    model = glm::mat4(1.0);

    bool renderfps = false;

    if(renderfps) {
        int numDigits = 1;
        if(fps > 0) {
            numDigits = log10(fps)+1;
        }
        int f = fps;
        int kerning[] = {6, 4, 6, 6, 6, 6, 6, 5, 6, 6};
        float width = 0;
        for(int d = 0; d < numDigits; d++) {
            width += kerning[f%10]*16/16.0;
            f /= 10;
        }
        f = fps;
        int pos = 0;
        for(int d = 0; d < numDigits; d++) {
            pos -= kerning[f%10]*16/16.0;
            renderSprite(f%10, (width+pos)*screenScale, 0, 16*screenScale, 16*screenScale, digitsImg, 16, 16, 0, 0, 0, 1, 0, 0, 0.0, false, false);
            f /= 10;
        }
    }
}

float sign(float x1, float y1, float x2, float y2, float x3, float y3) {
    return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}

bool insideTri(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3) {
    bool b1, b2, b3;
    b1 = sign(px, py, x1, y1, x2, y2) < 0.0f;
    b2 = sign(px, py, x2, y2, x3, y3) < 0.0f;
    b3 = sign(px, py, x3, y3, x1, y1) < 0.0f;
    if((b1 == b2) && (b2 == b3)) {
        return true;
    } else {
        return false;
    }
}

float dot(float x1, float y1, float x2, float y2) {
    return x1*x2+y1*y2;
}

double getLuminance(double R, double G, double B) {
    double R1;
    double G1;
    double B1;
    //cout << "l" << R << " " << G << " " << B << endl;
    if(R <= 0.03928) {
        R1 = R/12.92;
    } else {
        R1 = pow(((R+0.055)/1.055), 2.4);
    }
    if(G <= 0.03928) {
        G1 = G/12.92;
    } else {
        G1 = pow(((G+0.055)/1.055), 2.4);
    }
    if(B <= 0.03928) {
        B1 = B/12.92;
    } else {
        B1 = pow(((B+0.055)/1.055), 2.4);
    }
    double L = 0.2126*R1+0.7152*G1+0.0722*B1;
    return L;
}

double getContrast(float y1, float y2) {
    if(y1 < y2) {
        float y3 = y2;
        y2 = y1;
        y1 = y3;
    }
    return (y1+0.05)/(y2+0.05);
}

double getContrast(Color a, Color b) {
    float y1 = getLuminance(a.r, a.g, a.b);
    float y2 = getLuminance(b.r, b.g, b.b);
    if(y1 < y2) {
        float y3 = y2;
        y2 = y1;
        y1 = y3;
    }
    return (y1+0.05)/(y2+0.05);
}

void genGrad(Color c1, Color c2) {
    int shades = colorParams.at(9)->value;
    int middleS = shades/2.0;
    Color ahsv = RGBtoHSV(c1.r, c1.g, c1.b);
    Color bhsv = RGBtoHSV(c2.r, c2.g, c2.b);
    int AH = ahsv.r;
    int BH = bhsv.r;
    if(BH <= AH-180) {
        BH += 360;
    }
    int Hshift = (BH-AH)/float(middleS);
    int Sshift = (bhsv.g-ahsv.g)/float(middleS);
    int Vshift = (bhsv.b-ahsv.b)/float(middleS);
    for(int i = 1; i < middleS; i++) {
        int currentH = ahsv.r+i*Hshift;
        float CH = i*Hshift/float(BH-AH);
        //CH = sqrt(CH);
        CH *= (BH-AH);
        currentH = ahsv.r+CH;
        while(currentH > 360) {
            currentH -= 360;
        }
        while(currentH < 0) {
            currentH += 360;
        }
        int currentS = ahsv.g+i*Sshift;
        float CS = i*Sshift/float(bhsv.g-ahsv.g);
        //cout << CS << endl;
        //CS = 1.0-CS;
        CS = CS*CS*CS;
        //CS = 1.0-CS;
        CS *= (bhsv.g-ahsv.g);
        currentS = ahsv.g+CS;
        int currentV = ahsv.b+i*Vshift;
        Color finalhsv = Color(currentH, currentS, currentV);
        Color finalrgb = HSVtoRGB(finalhsv.r, finalhsv.g, finalhsv.b);
        Color* final = new Color(finalrgb.r, finalrgb.g, finalrgb.b);
        palette.push_back(final);
    }
    Color* cf = new Color(c2.r, c2.g, c2.b);
    palette.push_back(cf);
}

float curve(float x) {
    return x*sqrt(x);
}

void baseToBright(Color base, Color target, int s, int s2) {
    int shades = s;
    int middleS = s2;
    Color ahsv = RGBtoHSV(target.r, target.g, target.b);
    Color bhsv = RGBtoHSV(base.r, base.g, base.b);
    int AH = ahsv.r;
    int BH = bhsv.r;
    if(AH <= BH-180) {
        AH += 360;
    }
    vector<Color*> palN;
    int Hshift = (BH-AH)*(colorParams.at(8)->value/100.0)/float(middleS);
    int Sshift = (ahsv.g-bhsv.g)/float(middleS);
    int Vshift = (ahsv.b-bhsv.b)/float(middleS);
    Color prevColor = Color(base.r, base.g, base.b);

    double lB = getLuminance(base.r, base.g, base.b);
    double lT = getLuminance(target.r, target.g, target.b);
    double offset = 0.05;

    double a = (lT+offset)/(lB+offset);
    double r = pow(a, 1.0/float(middleS));

    for(int i = 0; i < middleS; i++) {
        float index = (i+1)/float(middleS);
        float curv = index;
        curv = curve(curv);
        //cout << index << endl;
        //index = curve(index);
        //cout << index << endl;
        index *= (middleS);
        curv *= (middleS);
        int currentH = bhsv.r-curv*Hshift;
        int currentS = bhsv.g+index*Sshift;
        int currentV = bhsv.b+index*Vshift;
        while(currentH > 360) {
            currentH -= 360;
        }
        while(currentH < 0) {
            currentH += 360;
        }
        float CS = currentS/float(bhsv.g);
        if(bhsv.g == 0) {
            CS = 0;
        }
        CS = sqrt(CS);
        CS = CS*0.8+0.2;
        //currentS = CS*float(bhsv.g);
        currentS = max(0, currentS);
        currentS = min(100, currentS);
        currentV = max(0, currentV);
        currentV = min(100, currentV);
        Color finalhsv = Color(currentH, currentS, currentV);
        Color finalrgb = HSVtoRGB(finalhsv.r, finalhsv.g, finalhsv.b);
        double contrast = getContrast(prevColor, finalrgb);
        //cout << "before " << contrast;

        int V = 100;
        finalhsv = Color(currentH, currentS, V);
        finalrgb = HSVtoRGB(finalhsv.r, finalhsv.g, finalhsv.b);
        contrast = getContrast(prevColor, finalrgb);
        while(contrast > r) {
            V--;
            finalhsv = Color(currentH, currentS, V);
            finalrgb = HSVtoRGB(finalhsv.r, finalhsv.g, finalhsv.b);
            contrast = getContrast(prevColor, finalrgb);
        }
        //cout << " after " << contrast << " r " << r << endl;

        Color* final = new Color(finalrgb.r, finalrgb.g, finalrgb.b);
        palN.push_back(final);
        prevColor = Color(finalrgb.r, finalrgb.g, finalrgb.b);
        //prevContrast = contrast;
    }
    for(int i = palN.size()-1; i >= 0; i--) {
        palette.push_back(palN.at(i));
    }
}

void baseToDark(Color base, Color target, int s, int s2) {
    int shades = s-1;
    int middleS = s2;
    Color ahsv = RGBtoHSV(target.r, target.g, target.b);
    Color bhsv = RGBtoHSV(base.r, base.g, base.b);
    int AH = ahsv.r;
    int BH = bhsv.r;
    if(BH <= AH-180) {
        BH += 360;
    }
    int Hshift = (AH-BH)*(colorParams.at(8)->value/100.0)/float(shades-middleS);
    int Sshift = (ahsv.g-bhsv.g)/float(shades-middleS);
    int Vshift = (ahsv.b-bhsv.b)/float(shades-middleS);
    Color prevColor = Color(base.r, base.g, base.b);

    double lB = getLuminance(base.r, base.g, base.b);
    double lT = getLuminance(target.r, target.g, target.b);
    double offset = 0.05;

    double a = (lB+offset)/(lT+offset);
    double r = pow(a, 1.0/float(shades-middleS));

    for(int i = middleS+1; i < shades+1; i++) {
        float index = (i-middleS)/float(shades-(middleS));
        float curv = index;
        curv = curve(curv);
        //index = curve(index);
        //cout << index << endl;
        index *= (shades-(middleS));
        curv *= (shades-(middleS));
        int currentH = bhsv.r+curv*Hshift;
        int currentS = bhsv.g+index*Sshift;
        int currentV = bhsv.b+index*Vshift;
        while(currentH > 360) {
            currentH -= 360;
        }
        while(currentH < 0) {
            currentH += 360;
        }
        float CS = (currentS-bhsv.g)/float(100-bhsv.g);
        if(100-bhsv.g == 0) {
            CS = 0;
        }
        CS = CS*CS*CS;
        CS = CS*0.5;
        CS = 0;
        //currentS = CS*float(100-bhsv.g)+bhsv.g;
        currentS = max(0, currentS);
        currentS = min(100, currentS);
        currentV = max(0, currentV);
        currentV = min(100, currentV);
        Color finalhsv = Color(currentH, currentS, currentV);
        Color finalrgb = HSVtoRGB(finalhsv.r, finalhsv.g, finalhsv.b);
        double contrast = getContrast(prevColor, finalrgb);
        //cout << "before " << contrast;

        int V = 0;
        finalhsv = Color(currentH, currentS, V);
        finalrgb = HSVtoRGB(finalhsv.r, finalhsv.g, finalhsv.b);
        contrast = getContrast(prevColor, finalrgb);
        while(contrast > r) {
            V++;
            finalhsv = Color(currentH, currentS, V);
            finalrgb = HSVtoRGB(finalhsv.r, finalhsv.g, finalhsv.b);
            contrast = getContrast(prevColor, finalrgb);
        }
        //cout << " after " << contrast << " r " << r << endl;

        Color* final = new Color(finalrgb.r, finalrgb.g, finalrgb.b);
        palette.push_back(final);
        prevColor = Color(finalrgb.r, finalrgb.g, finalrgb.b);
    }
}

void genShades(Color c, Color b, Color d, int shades, int shadesM) {
    baseToBright(c, b, shades, shadesM);
    Color* a = new Color(c.r, c.g, c.b);
    palette.push_back(a);
    //palette.push_back(a);
    baseToDark(c, d, shades, shadesM);
}

void generatePalette() {
    for(int toDel = 0; toDel < palette.size(); toDel++) {
        delete palette.at(toDel);
    }
    palette.clear();

    int globalSat = (rand()%72)+18;
    globalSat = colorParams.at(8)->value;
    Color bright = Color(255, 243, 232);
    Color dark = Color(31, 25, 46);

    int count = 5;

    int angleInit = rand()%360;
    int angleOffset = 360/count;
    int maxVar = angleOffset/2.0;
    angleOffset += (rand()%(maxVar*2))-maxVar;

    for(int i = 0; i < count+1; i++) {
        bool invalid = true;
        Color nextrgb = Color(0, 0, 0);
        while(invalid) {
            invalid = false;
            int nextAngle = angleInit+angleOffset*i;
            while(nextAngle >= 360) {
                nextAngle -= 360;
            }
            while(nextAngle < 0) {
                nextAngle += 360;
            }
            Color next = Color(nextAngle, (rand()%3)-1+globalSat, 65);
            if(i == count) {
                next.g = 10;
            }

            double lB = getLuminance(bright.r, bright.g, bright.b);
            double lT = getLuminance(dark.r, dark.g, dark.b);
            double offset = 0.05;

            double aa = (lB+offset)/(lT+offset);
            double r = pow(aa, 1.0/2);

            double l = (lT+offset)*r-offset;

            nextrgb = HSVtoRGB(next.r, next.g, 100);
            double luminance = getLuminance(nextrgb.r, nextrgb.g, nextrgb.b);

            int V = 100;
            while(luminance > l) {
                V--;
                nextrgb = HSVtoRGB(next.r, next.g, V);
                luminance = getLuminance(nextrgb.r, nextrgb.g, nextrgb.b);
            }
        }

        if(i != 0) {
            int ramp = rand()%i;
            int side = rand()%2;
            int offset = rand()%int(colorParams.at(9)->value/4+1);
            int offset2 = offset;
            if(side) {
                offset2 = colorParams.at(9)->value-1-offset;
            }
            int final = ramp*colorParams.at(9)->value+offset2;
            //Color* finalC = palette.at(final);
            if(side) {
                //dark = Color(finalC->r, finalC->g, finalC->b);
            } else {
                //bright = Color(finalC->r, finalC->g, finalC->b);
            }
        }

        genShades(nextrgb, bright, dark, colorParams.at(9)->value, (colorParams.at(9)->value)/2.0);
    }

    float treshold = 40;
    for(int i = 0; i < count+1; i++) {
        int ramp = i;
        for(int s = 1; s < 2; s++) {
            int side = s;
            int offset = 1;
            int offset2 = offset;
            if(side) {
                offset2 = colorParams.at(9)->value-1-offset;
            }
            int final = ramp*colorParams.at(9)->value+offset2;
            Color* finalC = palette.at(final);
            Color* color2 = NULL;
            float dist = 10000;
            for(int i2 = 0; i2 < count+1; i2++) {
                if(i2 != i) {
                    int ramp2 = i2;
                    int final2 = ramp2*colorParams.at(9)->value+offset2;
                    Color* finalC2 = palette.at(final2);
                    if(finalC2->r != -1 && finalC->r != -1) {
                        float dr = finalC2->r-finalC->r;
                        float dg = finalC2->g-finalC->g;
                        float db = finalC2->b-finalC->b;
                        float dt = sqrt(dr*dr+dg*dg+db*db);
                        if(dt < treshold) {
                            if(dt < dist) {
                                dist = dt;
                                color2 = finalC2;
                            }
                        }
                    }
                }
            }
            if(color2 != NULL) {
                if(side) {
                    dark = Color(color2->r, color2->g, color2->b);
                } else {
                    bright = Color(color2->r, color2->g, color2->b);
                }
                Color* next = palette.at(ramp*colorParams.at(9)->value+colorParams.at(9)->value/2.0);
                genShades(*next, bright, dark, colorParams.at(9)->value-1, (colorParams.at(9)->value)/2.0-(!side));
                for(int f = 0; f < colorParams.at(9)->value; f++) {
                    Color* rampC = palette.at(ramp*colorParams.at(9)->value+f);
                    rampC->r = -1;
                    rampC->g = -1;
                    rampC->b = -1;
                }
            }
        }
    }

    float removeTresh = 15;
    for(int i = 0; i < palette.size(); i++) {
        Color* finalC = palette.at(i);
        for(int j = 0; j < palette.size(); j++) {
            if(i != j) {
                Color* finalC2 = palette.at(j);
                float dr = finalC2->r-finalC->r;
                float dg = finalC2->g-finalC->g;
                float db = finalC2->b-finalC->b;
                float dt = sqrt(dr*dr+dg*dg+db*db);
                if(dt < removeTresh) {
                    finalC2->r = -1;
                    finalC2->g = -1;
                    finalC2->b = -1;
                }
            }
        }
    }

    for(int i = 0; i < palette.size(); i++) {
        Color* finalC = palette.at(i);
        if(finalC->r == -1) {
            delete finalC;
            palette.erase(std::remove(palette.begin(), palette.end(), finalC), palette.end());
            i--;
        }
    }
    paletteChanged();
}

void Parameter::render(int ex, int ey) { //renderparam
    if(ID == 0) {
        renderWheel(ex+x, ey+y, w, h, 0);
        renderWheelTri(ex+x, ey+y, w, h, value);

        float centerX = x+w/2.0;
        float centerY = y+h/2.0;
        float len = w/2.0*0.8;
        float len2 = w/2.0*0.9;
        float x1 = cos((value)*M_PI/180.0)*len+centerX; // 1, 1
        float y1 = sin((value)*M_PI/180.0)*len+centerY;
        float xf = cos((value)*M_PI/180.0)*len2+centerX; // 1, 1
        float yf = sin((value)*M_PI/180.0)*len2+centerY;
        float x2 = cos((value+120.0)*M_PI/180.0)*len+centerX; // 0, 1
        float y2 = sin((value+120.0)*M_PI/180.0)*len+centerY;
        float x3 = cos((value+240.0)*M_PI/180.0)*len+centerX; // 0, 0
        float y3 = sin((value+240.0)*M_PI/180.0)*len+centerY;

        float v1x = x1-x2;
        float v1y = y1-y2;
        float l1 = sqrt(v1x*v1x+v1y*v1y);
        v1x /= l1;
        v1y /= l1;
        l1 *= value2*value3;
        v1x *= l1;
        v1y *= l1;
        float v2x = x2-x3;
        float v2y = y2-y3;
        float l2 = sqrt(v2x*v2x+v2y*v2y);
        v2x /= l2;
        v2y /= l2;
        l2 *= value3;
        v2x *= l2;
        v2y *= l2;

        renderIcon(ex+x3-4+v1x+v2x, ey+y3-4+v1y+v2y, 8, 8, iconImg2, 0);
        renderIcon(ex+xf-4, ey+yf-4, 8, 8, iconImg2, 0);
    } else if(ID == 4) {
        renderWheelF(ex+x, ey+y, w, h, 0);
        renderWheelTriF(ex+x, ey+y, w, h, value);

        float centerX = x+w/2.0;
        float centerY = y+h/2.0;
        float len = w/2.0*0.8;
        float len2 = w/2.0*0.9;
        float x1 = cos((value)*M_PI/180.0)*len+centerX; // 1, 1
        float y1 = sin((value)*M_PI/180.0)*len+centerY;
        float xf = cos((value)*M_PI/180.0)*len2+centerX; // 1, 1
        float yf = sin((value)*M_PI/180.0)*len2+centerY;
        float x2 = cos((value+120.0)*M_PI/180.0)*len+centerX; // 0, 1
        float y2 = sin((value+120.0)*M_PI/180.0)*len+centerY;
        float x3 = cos((value+240.0)*M_PI/180.0)*len+centerX; // 0, 0
        float y3 = sin((value+240.0)*M_PI/180.0)*len+centerY;

        float v1x = x1-x2;
        float v1y = y1-y2;
        float l1 = sqrt(v1x*v1x+v1y*v1y);
        v1x /= l1;
        v1y /= l1;
        l1 *= value2*value3;
        v1x *= l1;
        v1y *= l1;
        float v2x = x2-x3;
        float v2y = y2-y3;
        float l2 = sqrt(v2x*v2x+v2y*v2y);
        v2x /= l2;
        v2y /= l2;
        l2 *= value3;
        v2x *= l2;
        v2y *= l2;

        renderIcon(ex+x3-4+v1x+v2x, ey+y3-4+v1y+v2y, 8, 8, iconImg2, 0);
        renderIcon(ex+xf-4, ey+yf-4, 8, 8, iconImg2, 0);
    } else if(ID == 1) {
        renderUI(ex+x, ey+y, w, h, effectImg6);
        float space = h/2-4;
        renderText(name, ex+x+space, ey+y+space, fontImg, 0);
        if(s == NULL || s->s == NULL) {
            if(textType == this) {
                if(blinkTimer <= 32) {
                    renderText(typing+"_", ex+x+w-space, ey+y+space, fontImg, true);
                } else {
                    renderText(typing, ex+x+w-space-5, ey+y+space, fontImg, true);
                }
            } else {
                renderNumber(value, ex+x+w-space, ey+y+space, fontImg, false, true);
            }
        }
    } else if(ID == 5) { //unused
        renderUI(ex+x, ey+y, w, h, effectImg6);
        float space = h/2-4;
        renderText(name, ex+x+space, ey+y+space, fontImg, 0);
        renderNumber(pow(2, value), ex+x+w-space, ey+y+space, fontImg, false, true);
    } else if(ID == 2) {
        renderUI(ex+x, ey+y, w, h, effectImg6);
        float space = h/2-4;
        renderText(name, ex+x+space, ey+y+space, fontImg, 0);
        if(s == NULL || s->s == NULL) {
            if(textType == this) {
                if(blinkTimer <= 32) {
                    renderText(typing+"_", ex+x+w-space, ey+y+space, fontImg, true);
                } else {
                    renderText(typing, ex+x+w-space-5, ey+y+space, fontImg, true);
                }
            } else {
                renderNumber(value, ex+x+w-space, ey+y+space, fontImg, true, true);
            }
        }
    } else if(ID == 3) {
        renderUI(ex+x, ey+y, w, h, effectImg6);
        float space = h/2-4;
        renderText(name, ex+x+space, ey+y+space, fontImg, 0);
        if(value == 0) {
            renderText("NO", ex+x+w-space, ey+y+space, fontImg, true);
        } else {
            renderText("YES", ex+x+w-space, ey+y+space, fontImg, true);
        }
    } else if(ID == 7) {
        for(int i = 0; i < points.size(); i++) {
            float a1 = 0.0;
            for(int pp = 0; pp < points.size(); pp++) {
                if(points.at(pp)->a < points.at(i)->a && pp != i) {
                    a1 = points.at(i)->a;
                }
            }
            float a2 = 1.0;
            for(int pp = 0; pp < points.size(); pp++) {
                if(points.at(pp)->a > points.at(i)->a && points.at(pp)->a < a2 && pp != i) {
                    a2 = points.at(pp)->a;
                }
            }
            Color* final = getPalColor(points.at(i)->r, points.at(i)->g, points.at(i)->b);
            renderColor(ex+x+4+int(a1*100), ey+y+3, int(a2*100)-int(a1*100), 4, final);
        }
        for(int i = 0; i < points.size(); i++) {
            renderIcon(ex+x+points.at(i)->a*100, ey+y+1, 8, 8, iconImg12, (i == selectedPoint));
        }
    } else if(ID == 8) {
        renderIcon(ex+x, ey+y, 8, 8, iconImg13, 0);
    } else if(ID == 9) {
        renderIcon(ex+x, ey+y, 8, 8, iconImg13, 1);
    }
}

bPoint::bPoint() {
}

bPoint::bPoint(float x, float y) {
    this->x = x;
    this->y = y;
}

cBezier::cBezier() {
}

void cBezier::create() {
    glGenBuffers(1, &bVbo);
    update();
}

CPoint::CPoint() {
}

Parameter::Parameter(int ID, string name, float x, float y, float w, float h, int value, int value2, int value3, string tt) {
    this->index = 0;
    this->ref = NULL;
    this->s = NULL;
    this->ID = ID;
    this->name = name;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->tt = tt;
    this->resetValue = value;
    this->value = value;
    this->value2 = value2;
    this->value3 = value3;
    this->value4 = 0;
    this->selectedPoint = 0;
    this->dragging = false;
    this->dragging2 = false;
    if(ID == 0) {
        float H = value;
        float S = value2;
        float V = value3;
        Color* final = getPalColor(H, S, V);
        for(int col = 0; col < palette.size(); col++) {
            Color* color = palette.at(col);
            if(final->equals(color)) {
                value4 = col;
            }
        }
    } else if(ID == 7) {
        CPoint* a = new CPoint();
        a->a = 0.0;
        a->r = 0;
        a->g = 0;
        a->b = 1;
        points.push_back(a);
    }
}

void Parameter::mouseDown(int mx, int my, int ex, int ey, int layer, nEffect* fx) {
    mx -= ex;
    my -= ey;
    if(ID == 0 || ID == 4) {
        float centerX = x+w/2.0;
        float centerY = y+h/2.0;
        float dx = mx-centerX;
        float dy = my-centerY;
        float dist = sqrt(dx*dx+dy*dy);
        if(dist < w/2.0 && dist > w/2.0*0.8) {
            saveUndo();
            dragging = true;
            value = atan2(-dy, -dx)/M_PI*180.0+180.0;
            if(value == 360) {
                value = 0;
            }
            if(value < 0 || value > 360) {
                value = 0;
            }
        }
        float len = w/2.0*0.8;
        float x1 = cos((value)*M_PI/180.0)*len+centerX; // 1, 1
        float y1 = sin((value)*M_PI/180.0)*len+centerY;
        float x2 = cos((value+120.0)*M_PI/180.0)*len+centerX; // 0, 1
        float y2 = sin((value+120.0)*M_PI/180.0)*len+centerY;
        float x3 = cos((value+240.0)*M_PI/180.0)*len+centerX; // 0, 0
        float y3 = sin((value+240.0)*M_PI/180.0)*len+centerY;
        dragging2 = insideTri(mx, my, x1, y1, x2, y2, x3, y3);
        if(dragging2) {
            saveUndo();
            float oldX = mx-x3;
            float oldY = my-y3;
            float angle = 30.0-value;
            float newX = (oldX*sin((120-angle)*M_PI/180.0)+oldY*sin((120-90-angle)*M_PI/180.0))/sin(120*M_PI/180.0);
            float newY = (oldX*sin(angle*M_PI/180.0)+oldY*sin((90+angle)*M_PI/180.0))/sin(120*M_PI/180.0);

            float v1x = x1-x2;
            float v1y = y1-y2;
            float l1 = sqrt(v1x*v1x+v1y*v1y);
            float v2x = x2-x3;
            float v2y = y2-y3;
            float l2 = sqrt(v2x*v2x+v2y*v2y);

            newX /= l1;
            newY /= l2;

            value2 = newX/newY;
            value3 = newY;
            if(value2 < 0.0) {
                value2 = 0.0;
            } else if(value2 > 1.0) {
                value2 = 1.0;
            }
            if(value3 < 0.0) {
                value3 = 0.0;
            } else if(value3 > 1.0) {
                value3 = 1.0;
            }
            float H = value;
            float S = value2;
            float V = value3;
            Color* final = getPalColor(H, S, V);
            for(int col = 0; col < palette.size(); col++) {
                Color* color = palette.at(col);
                if(final->equals(color)) {
                    value4 = col;
                }
            }
        }
        if((dragging || dragging2) && layer == -1) {
            Color hsv = Color(value, value2*100, value3*100);
            Color rgb = HSVtoRGB(hsv.r, hsv.g, hsv.b);
            beforePaletteChange();
            colorParams.at(1)->value = rgb.r;
            colorParams.at(2)->value = rgb.g;
            colorParams.at(3)->value = rgb.b;
            colorParams.at(4)->value = value;
            colorParams.at(5)->value = value2*100;
            colorParams.at(6)->value = value3*100;
            if(selectedColor == -1) {
                rampA.r = rgb.r;
                rampA.g = rgb.g;
                rampA.b = rgb.b;
            } else if(selectedColor == -2) {
                rampB.r = rgb.r;
                rampB.g = rgb.g;
                rampB.b = rgb.b;
            } else {
                palette.at(selectedColor)->r = rgb.r;
                palette.at(selectedColor)->g = rgb.g;
                palette.at(selectedColor)->b = rgb.b;
            }
            paletteChanged();
        }
        if(ID == 0) {
            p->points.at(p->selectedPoint)->r = value;
            p->points.at(p->selectedPoint)->g = value2;
            p->points.at(p->selectedPoint)->b = value3;
            p->points.at(p->selectedPoint)->i = value4;
            updateGrad(p);
            if(layer != -1) {
                Texture* t = texs.at(currentTexture);
                fx->undone = true;
                t->genTex();
            }
        }
    } else if(ID == 7) {
        for(int i = 0; i < points.size(); i++) {
            if(mx > x+points.at(i)->a*100 && mx < x+points.at(i)->a*100+8 && my > y+1 && my < y+1+8) {
                selectedPoint = i;
                p->value = points.at(selectedPoint)->r;
                p->value2 = points.at(selectedPoint)->g;
                p->value3 = points.at(selectedPoint)->b;
                p->value4 = points.at(selectedPoint)->i;
                dragging = true;
            }
        }
    } else if(ID == 8) {
        if(mx > x && mx < x+8 && my > y && my < y+8) {
            CPoint* a = new CPoint();
            float a1 = p->points.at(p->selectedPoint)->a;
            float a2 = 1.0;
            for(int pp = 0; pp < p->points.size(); pp++) {
                if(p->points.at(pp)->a > a1 && p->points.at(pp)->a < a2) {
                    a2 = p->points.at(pp)->a;
                }
            }
            a->a = (a1+a2)/2.0;
            a->r = 0;
            a->g = 0;
            a->b = 1;
            p->points.push_back(a);
            p->selectedPoint = p->points.size()-1;
            p->p->value = p->points.at(p->selectedPoint)->r;
            p->p->value2 = p->points.at(p->selectedPoint)->g;
            p->p->value3 = p->points.at(p->selectedPoint)->b;
            p->p->value4 = p->points.at(p->selectedPoint)->i;
            updateGrad(p);
            if(layer != -1) {
                Texture* t = texs.at(currentTexture);
                fx->undone = true;
                t->genTex();
            }
        }
    } else if(ID == 9) {
        if(mx > x && mx < x+8 && my > y && my < y+8) {
            if(p->points.size() > 1) {
                delete p->points.at(p->selectedPoint);
                p->points.erase(p->points.begin() + p->selectedPoint);
                if(p->selectedPoint >= p->points.size()) {
                    p->selectedPoint = p->points.size()-1;
                    p->p->value = p->points.at(p->selectedPoint)->r;
                    p->p->value2 = p->points.at(p->selectedPoint)->g;
                    p->p->value3 = p->points.at(p->selectedPoint)->b;
                    p->p->value4 = p->points.at(p->selectedPoint)->i;
                }
                updateGrad(p);
                if(layer != -1) {
                    Texture* t = texs.at(currentTexture);
                    fx->undone = true;
                    t->genTex();
                }
            }
        }
    } else if(ID == 1 || ID == 2 || ID == 3 || ID == 5 || ID == 6) {
        if(mx > x && mx < x+w && my > y && my < y+h) {
            if(doubleClickTimer <= 20) {
                value = resetValue;
                if(layer == -1) {
                    if(name == "Size") {
                        texSizeX = value;
                        texSizeY = value;
                        glGenTextures(1, &screenTexFinal2);
                        glBindTexture(GL_TEXTURE_2D, screenTexFinal2);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glGenFramebuffers(1, &screenFboFinal2);
                        glBindFramebuffer(GL_FRAMEBUFFER, screenFboFinal2);
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexFinal2, 0);
                        GLenum d2[] = {GL_COLOR_ATTACHMENT0};
                        glDrawBuffers(1, d2);
                        glBindFramebuffer(GL_FRAMEBUFFER, 0);
                        for(int i = 0; i < texs.size(); i++) {
                            Texture* t = texs.at(i);
                            for(int in = 0; in < t->fxs.size(); in++) {
                                t->fxs.at(in)->undone = true;
                            }
                            t->genTex();
                        }
                    }
                    if(name == "R" || name == "G" || name == "B") {
                        Color rgb = Color(colorParams.at(1)->value, colorParams.at(2)->value, colorParams.at(3)->value);
                        Color hsv = RGBtoHSV(rgb.r, rgb.g, rgb.b);
                        beforePaletteChange();
                        colorParams.at(0)->value = hsv.r;
                        colorParams.at(0)->value2 = hsv.g/100.0;
                        colorParams.at(0)->value3 = hsv.b/100.0;
                        colorParams.at(4)->value = hsv.r;
                        colorParams.at(5)->value = hsv.g;
                        colorParams.at(6)->value = hsv.b;
                        if(selectedColor == -1) {
                            rampA.r = rgb.r;
                            rampA.g = rgb.g;
                            rampA.b = rgb.b;
                        } else if(selectedColor == -2) {
                            rampB.r = rgb.r;
                            rampB.g = rgb.g;
                            rampB.b = rgb.b;
                        } else {
                            palette.at(selectedColor)->r = rgb.r;
                            palette.at(selectedColor)->g = rgb.g;
                            palette.at(selectedColor)->b = rgb.b;
                        }
                        paletteChanged();
                    }
                    if(name == "H" || name == "S" || name == "V") {
                        int h = colorParams.at(4)->value;
                        if(h == 360) {
                            h = 0;
                        }
                        Color hsv = Color(h, colorParams.at(5)->value, colorParams.at(6)->value);
                        Color rgb = HSVtoRGB(hsv.r, hsv.g, hsv.b);
                        beforePaletteChange();
                        colorParams.at(0)->value = hsv.r;
                        colorParams.at(0)->value2 = hsv.g/100.0;
                        colorParams.at(0)->value3 = hsv.b/100.0;
                        colorParams.at(1)->value = rgb.r;
                        colorParams.at(2)->value = rgb.g;
                        colorParams.at(3)->value = rgb.b;
                        if(selectedColor == -1) {
                            rampA.r = rgb.r;
                            rampA.g = rgb.g;
                            rampA.b = rgb.b;
                        } else if(selectedColor == -2) {
                            rampB.r = rgb.r;
                            rampB.g = rgb.g;
                            rampB.b = rgb.b;
                        } else {
                            palette.at(selectedColor)->r = rgb.r;
                            palette.at(selectedColor)->g = rgb.g;
                            palette.at(selectedColor)->b = rgb.b;
                        }
                        paletteChanged();
                    }
                }
                if(layer != -1) {
                    Texture* t = texs.at(currentTexture);
                    fx->undone = true;
                    t->genTex();
                }
            } else {
                saveUndo();
            }
            doubleClickTimer = 0;
            textTypeTemp = this;
            textTypeFxTemp = fx;
            textTypeLayerTemp = layer;
            dragging = true;
            draggingParam = true;
            oldmX = mx;
            oldmY = my;
            initmX = mx;
            initmY = my;
            if(name == "Rows") {
                RoC = false;
            } else if(name == "Cols") {
                RoC = true;
            }
        }
    }
}

void updateDrag(int layer, nEffect* fx, Parameter* param) {
    if(layer != -1) {
        Texture* t = texs.at(currentTexture);
        fx->undone = true;
        t->genTex();
    } else if(layer == -1) {
        if(param->name == "Size") {
            texSizeX = param->value;
            texSizeY = param->value;
            glGenTextures(1, &screenTexFinal2);
            glBindTexture(GL_TEXTURE_2D, screenTexFinal2);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glGenFramebuffers(1, &screenFboFinal2);
            glBindFramebuffer(GL_FRAMEBUFFER, screenFboFinal2);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexFinal2, 0);
            GLenum d2[] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, d2);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            for(int i = 0; i < texs.size(); i++) {
                Texture* t = texs.at(i);
                for(int in = 0; in < t->fxs.size(); in++) {
                    t->fxs.at(in)->undone = true;
                }
                t->genTex();
            }
        }
        if(param->name == "R" || param->name == "G" || param->name == "B") {
            Color rgb = Color(colorParams.at(1)->value, colorParams.at(2)->value, colorParams.at(3)->value);
            Color hsv = RGBtoHSV(rgb.r, rgb.g, rgb.b);
            beforePaletteChange();
            colorParams.at(0)->value = hsv.r;
            colorParams.at(0)->value2 = hsv.g/100.0;
            colorParams.at(0)->value3 = hsv.b/100.0;
            colorParams.at(4)->value = hsv.r;
            colorParams.at(5)->value = hsv.g;
            colorParams.at(6)->value = hsv.b;
            if(selectedColor == -1) {
                rampA.r = rgb.r;
                rampA.g = rgb.g;
                rampA.b = rgb.b;
            } else if(selectedColor == -2) {
                rampB.r = rgb.r;
                rampB.g = rgb.g;
                rampB.b = rgb.b;
            } else {
                palette.at(selectedColor)->r = rgb.r;
                palette.at(selectedColor)->g = rgb.g;
                palette.at(selectedColor)->b = rgb.b;
            }
            paletteChanged();
        }
        if(param->name == "H" || param->name == "S" || param->name == "V") {
            int h = colorParams.at(4)->value;
            if(h == 360) {
                h = 0;
            }
            Color hsv = Color(h, colorParams.at(5)->value, colorParams.at(6)->value);
            Color rgb = HSVtoRGB(hsv.r, hsv.g, hsv.b);
            beforePaletteChange();
            colorParams.at(0)->value = hsv.r;
            colorParams.at(0)->value2 = hsv.g/100.0;
            colorParams.at(0)->value3 = hsv.b/100.0;
            colorParams.at(1)->value = rgb.r;
            colorParams.at(2)->value = rgb.g;
            colorParams.at(3)->value = rgb.b;
            if(selectedColor == -1) {
                rampA.r = rgb.r;
                rampA.g = rgb.g;
                rampA.b = rgb.b;
            } else if(selectedColor == -2) {
                rampB.r = rgb.r;
                rampB.g = rgb.g;
                rampB.b = rgb.b;
            } else {
                palette.at(selectedColor)->r = rgb.r;
                palette.at(selectedColor)->g = rgb.g;
                palette.at(selectedColor)->b = rgb.b;
            }
            paletteChanged();
        }
    }
}

void Parameter::mouseMove(int mx, int my, int ex, int ey, int layer, nEffect* fx) {
    mx -= ex;
    my -= ey;
    if(ID == 0 || ID == 4) {
        if(dragging) {
            float dx = mx-(x+w/2.0);
            float dy = my-(y+h/2.0);
            value = atan2(-dy, -dx)/M_PI*180.0+180.0;
            if(value == 360) {
                value = 0;
            }
            if(value < 0 || value > 360) {
                value = 0;
            }
        }
        if(dragging2) {
            float centerX = x+w/2.0;
            float centerY = y+h/2.0;
            float len = w/2.0*0.8;
            float x1 = cos((value)*M_PI/180.0)*len+centerX; // 1, 1
            float y1 = sin((value)*M_PI/180.0)*len+centerY;
            float x2 = cos((value+120.0)*M_PI/180.0)*len+centerX; // 0, 1
            float y2 = sin((value+120.0)*M_PI/180.0)*len+centerY;
            float x3 = cos((value+240.0)*M_PI/180.0)*len+centerX; // 0, 0
            float y3 = sin((value+240.0)*M_PI/180.0)*len+centerY;

            float oldX = mx-x3;
            float oldY = my-y3;
            float angle = 30.0-value;
            float newX = (oldX*sin((120-angle)*M_PI/180.0)+oldY*sin((120-90-angle)*M_PI/180.0))/sin(120*M_PI/180.0);
            float newY = (oldX*sin(angle*M_PI/180.0)+oldY*sin((90+angle)*M_PI/180.0))/sin(120*M_PI/180.0);

            float v1x = x1-x2;
            float v1y = y1-y2;
            float l1 = sqrt(v1x*v1x+v1y*v1y);
            float v2x = x2-x3;
            float v2y = y2-y3;
            float l2 = sqrt(v2x*v2x+v2y*v2y);

            newX /= l1;
            newY /= l2;

            value2 = newX/newY;
            value3 = newY;
            if(value2 < 0.0) {
                value2 = 0.0;
            } else if(value2 > 1.0) {
                value2 = 1.0;
            }
            if(value3 < 0.0) {
                value3 = 0.0;
            } else if(value3 > 1.0) {
                value3 = 1.0;
            }
            float H = value;
            float S = value2;
            float V = value3;
            Color* final = getPalColor(H, S, V);
            for(int col = 0; col < palette.size(); col++) {
                Color* color = palette.at(col);
                if(final->equals(color)) {
                    value4 = col;
                }
            }
        }
        if((dragging || dragging2) && layer == -1) {
            Color hsv = Color(value, value2*100, value3*100);
            Color rgb = HSVtoRGB(hsv.r, hsv.g, hsv.b);
            beforePaletteChange();
            colorParams.at(1)->value = rgb.r;
            colorParams.at(2)->value = rgb.g;
            colorParams.at(3)->value = rgb.b;
            colorParams.at(4)->value = value;
            colorParams.at(5)->value = value2*100;
            colorParams.at(6)->value = value3*100;
            if(selectedColor == -1) {
                rampA.r = rgb.r;
                rampA.g = rgb.g;
                rampA.b = rgb.b;
            } else if(selectedColor == -2) {
                rampB.r = rgb.r;
                rampB.g = rgb.g;
                rampB.b = rgb.b;
            } else {
                palette.at(selectedColor)->r = rgb.r;
                palette.at(selectedColor)->g = rgb.g;
                palette.at(selectedColor)->b = rgb.b;
            }
            paletteChanged();
        }
        if(ID == 0 && (dragging || dragging2)) {
            p->points.at(p->selectedPoint)->r = value;
            p->points.at(p->selectedPoint)->g = value2;
            p->points.at(p->selectedPoint)->b = value3;
            p->points.at(p->selectedPoint)->i = value4;
            updateGrad(p);
            if(layer != -1) {
                Texture* t = texs.at(currentTexture);
                fx->undone = true;
                t->genTex();
            }
        }
    } else if(ID == 7) {
        if(dragging) {
            points.at(selectedPoint)->a = (mx-4)/100.0;
            if(points.at(selectedPoint)->a > 1.0) {
                points.at(selectedPoint)->a = 1.0;
            } else if(points.at(selectedPoint)->a < 0.0) {
                points.at(selectedPoint)->a = 0.0;
            }
            updateGrad(this);
            if(layer != -1) {
                Texture* t = texs.at(currentTexture);
                fx->undone = true;
                t->genTex();
            }
        }
    } else if(ID == 1 || ID == 2 || ID == 3 || ID == 5 || ID == 6) {
        if(dragging) {
            float dist = 3;
            if(ID != 6) {
                if(s == NULL || s->s == NULL) {
                    if(textType != this || (ID != 1 && ID != 2)) {
                        if(my-oldmY < -dist) {//vertical drag
                            while(my-oldmY < -dist) {
                                oldmY -= dist;
                                if(value < value3 || value3 == -1) {
                                    value++;
                                }
                            }
                            updateDrag(layer, fx, this);
                        } else if(my-oldmY > dist) {
                            while(my-oldmY > dist) {
                                oldmY += dist;
                                if(value > value2 || value2 == -1) {
                                    value--;
                                }
                            }
                            updateDrag(layer, fx, this);
                        }
                        
                        if(mx-oldmX < -dist) {//horizontal drag
                            while(mx-oldmX < -dist) {
                                oldmX -= dist;
                                if(value > value2 || value2 == -1) {
                                    value--;
                                }
                            }
                            updateDrag(layer, fx, this);
                        } else if(mx-oldmX > dist) {
                            while(mx-oldmX > dist) {
                                oldmX += dist;
                                if(value < value3 || value3 == -1) {
                                    value++;
                                }
                            }
                            updateDrag(layer, fx, this);
                        }
                    }
                }
            }
        }
    }
}

void Parameter::mouseUp(int mx, int my, int ex, int ey, int layer, nEffect* fx) {
    mx -= ex;
    my -= ey;
    dragging = false;
    dragging2 = false;
    draggingParam = false;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

int main(int argc, char* args[]) {
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 2);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	    SDL_Rect r;
	    SDL_GetDisplayBounds(0, &r);
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);
        displayW = current.w;
        displayH = current.h;
	    screenW = displayW/2.4;
        screenH = displayH/2.4;
        screenPW = pow(2, ceil(log(screenW)/log(2)));;
        screenPH = pow(2, ceil(log(screenH)/log(2)));;

		window = SDL_CreateWindow( "Tilemancer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW*screenScale, screenH*screenScale, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );

        SDL_SetWindowMinimumSize(window, (128+7)*3*screenScale, 256*screenScale);

        SDL_ShowCursor(SDL_FALSE);

		if(window == NULL) {
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		} else {
            setIcon();
		    gContext = SDL_GL_CreateContext(window);
            cout << "Created OpenGL " << glGetString(GL_VERSION) << " context" << endl;
            initGL();
            Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
            TTF_Init();
            initGlew();
            LoadStuff();
		}
	}
	bool quit = false;
	SDL_Event e;
	while(!quit) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
            if(e.type == SDL_WINDOWEVENT) {
                if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    resizeWindow(e.window.data1, e.window.data2);
                }
            }
            if(e.type == SDL_MOUSEBUTTONDOWN) {
                if(textType != NULL) {
                    textType->value = atoi(textType->typing.c_str());
                    if(textType->value > textType->value3 && textType->value3 != -1) {
                        textType->value = textType->value3;
                    }
                    if(textType->value < textType->value2 && textType->value2 != -1) {
                        textType->value = textType->value2;
                    }
                    updateDrag(textTypeLayer, textTypeFx, textType);
                }
                textType = NULL;
                typeTimer = 0;
                if(e.button.button == SDL_BUTTON_LEFT) {
                    int x = e.button.x/screenScale;
                    int y = e.button.y/screenScale;
                    if(view == 0) {
                        if(browserOpen) {
                            int title = collH-4;
                            int bSpace = 5;
                            float tS = title/2-4;
                            if(x > barX+bSpace && x < screenW-barXRight-bSpace) {
                                int fh = tS;
                                int fw = tS-browserScroll;
                                int fwNext = 0;
                                for(int i = 0; i < filenames.size(); i++) {
                                    int n = textW(filenames.at(i)->name, barX+1+bSpace+fw+tS+8, title*2+bSpace*3+fh, fontImg, 0);
                                    if(x > barX+1+bSpace+fw-tS && x < barX+1+bSpace+fw-tS+n+8*2+tS && y > title*2+bSpace*3+fh-tS && y < title*2+bSpace*3+fh-tS+title) {
                                        if(doubleClickTimer <= 20 && selectedFile == i) {
                                            string fullDir = currentDir;
                                            if(fullDir.size() != 1) {
                                                if(OS == 0) {
                                                    fullDir = fullDir.append("\\");
                                                } else if(OS == 1) {
                                                    fullDir = fullDir.append("/");
                                                }
                                            }
                                            fullDir = fullDir.append(string(filenames.at(i)->name));
                                            if(filenames.at(i)->folder) {
                                                openBrowser(fullDir, 0, browserMode);
                                            } else {
                                                browserAction(fullDir, filenames.at(i)->name, currentDir);
                                            }
                                        } else {
                                            if(selectedFile != i) {
                                                overwrite = false;
                                            }
                                            selectedFile = i;
                                            filenameB = filenames.at(i)->name;
                                        }
                                        doubleClickTimer = 0;
                                    }
                                    if(n > fwNext) {
                                        fwNext = n;
                                    }
                                    fh += title;
                                    if(fh+title*2+title+bSpace*3 > title+bSpace*2+barY-1-bSpace*3-title-6 && i != filenames.size()-1) {
                                        fh = tS;
                                        fw += fwNext+tS*2+8*3;
                                        fwNext = 0;
                                    }
                                }
                                fw += fwNext+tS*2+8*3;
                                int scrollH = fw+browserScroll;
                                if(scrollH < screenW-barX-barXRight-bSpace*2) {
                                    scrollH = screenW-barX-barXRight-bSpace*2;
                                }
                                if(x > barX+bSpace+1+int(int(browserScroll)*(screenW-barX-barXRight-bSpace*2)/float(scrollH)) && x < barX+bSpace+1+int(int(browserScroll)*(screenW-barX-barXRight-bSpace*2)/float(scrollH))+int((screenW-barX-barXRight-bSpace*2)*(screenW-barX-barXRight-bSpace*2)/float(scrollH)) && y > barY-bSpace-6-1 && y < barY-bSpace-1) {
                                    draggingSBar = true;
                                    mouseOX = x;
                                    mouseOY = y;
                                    mouseX = x;
                                    mouseY = y;
                                    sBarDrag = 6;
                                    sRatio = scrollH/float(screenW-barX-barXRight-bSpace*2);
                                }
                            }
                            if(x > barX+1+bSpace && x < barX+1+bSpace+8 && y > bSpace+tS && y < bSpace+tS+8) {
                                //left
                                if(fnUndo.size() > 1) {
                                    openBrowser(fnUndo.back(), 1, browserMode);
                                }
                            }
                            if(x > barX+1+bSpace+8+bSpace && x < barX+1+bSpace+8+8+bSpace && y > bSpace+tS && y < bSpace+tS+8) {
                                //right
                                if(fnRedo.size() > 0) {
                                    openBrowser(fnRedo.back(), 2, browserMode);
                                }
                            }
                            if(x > barX+1+bSpace+8*2+bSpace*2 && x < barX+1+bSpace+8+8*2+bSpace*2 && y > bSpace+tS && y < bSpace+tS+8) {
                                //up
                                string newDir = currentDir;
                                newDir.erase(newDir.rfind('/'));
                                if(newDir.size() < 1) {
                                    if(OS == 0) {
                                        newDir = "\\";
                                    } else if(OS == 1) {
                                        newDir = "/";
                                    }
                                }
                                openBrowser(newDir, 0, browserMode);
                            }
                            if(x > screenW-barXRight-8-bSpace-1 && x < screenW-barXRight-bSpace-1 && y > bSpace+tS && y < bSpace+tS+8) {
                                browserOpen = false;
                                fnUndo.clear();
                                fnRedo.clear();
                            }
                            if(x > screenW-barXRight-8-bSpace-1 && x < screenW-barXRight-bSpace-1 && y > bSpace*2+title+tS && y < bSpace*2+title+tS+8) {
                                //action
                                string fullDir = currentDir;
                                if(fullDir.size() != 1) {
                                    if(OS == 0) {
                                        fullDir = fullDir.append("\\");
                                    } else if(OS == 1) {
                                        fullDir = fullDir.append("/");
                                    }
                                }
                                fullDir = fullDir.append(filenameB);
                                bool folder = false;
                                for(int i = 0; i < filenames.size(); i++) {
                                    if(!strcmp(filenames.at(i)->name.c_str(), filenameB.c_str()) && filenames.at(i)->folder) {
                                        openBrowser(fullDir, 0, browserMode);
                                        folder = true;
                                    }
                                }
                                if(!folder) {
                                    browserAction(fullDir, filenameB, currentDir);
                                }
                            }
                        } else {
                            if(x > barX && x < screenW-barXRight) {
                                if(camMoving) {
                                    draggingCam = true;
                                    mouseOX = x;
                                    mouseOY = y;
                                    mouseX = x;
                                    mouseY = y;
                                } else {
                                    for(int n = texs.at(currentTexture)->fxs.size()-1; n >= 0; n--) {
                                        nEffect* fx = texs.at(currentTexture)->fxs.at(n);
                                        bool breaking = false;
                                        bool noMove = false;
                                        for(int in = 0; in < fx->inputs.size(); in++) {//next step
                                            if(x > fx->x+nodeCX-4 && x < fx->x+nodeCX+4 && y > fx->y+nodeCY+fx->inputs.at(in)->y && y < fx->y+nodeCY+fx->inputs.at(in)->y+8) {
                                                if(fx->inputs.at(in)->s != NULL) {
                                                    draggingSocket = true;
                                                    draggingNode = false;
                                                    saveUndo();
                                                    fx->inputs.at(in)->s->px = x;
                                                    fx->inputs.at(in)->s->py = y;
                                                    for(int j = 0; j < texs.at(currentTexture)->fxs.size(); j++) {
                                                        if(texs.at(currentTexture)->fxs.at(j) == fx->inputs.at(in)->s->parent) {
                                                            draggedNode = j;
                                                            for(int out = 0; out < texs.at(currentTexture)->fxs.at(j)->outputs.size(); out++) {
                                                                if(texs.at(currentTexture)->fxs.at(j)->outputs.at(out) == fx->inputs.at(in)->s) {
                                                                    draggedSocket = out;
                                                                }
                                                            }
                                                        }
                                                    }
                                                    mouseOX = x;
                                                    mouseOY = y;
                                                    mouseX = x;
                                                    mouseY = y;
                                                    fx->undone = true;
                                                    fx->inputs.at(in)->s = NULL;
                                                }
                                                noMove = true;
                                                breaking = true;
                                            }
                                        }
                                        for(int out = 0; out < fx->outputs.size(); out++) {
                                            if(x > fx->x+nodeCX+fx->w-4 && x < fx->x+nodeCX+fx->w+4 && y > fx->y+nodeCY+fx->outputs.at(out)->y && y < fx->y+nodeCY+fx->outputs.at(out)->y+8) {
                                                draggingSocket = true;
                                                saveUndo();
                                                fx->outputs.at(out)->px = x;
                                                fx->outputs.at(out)->py = y;
                                                draggedNode = n;
                                                draggedSocket = out;
                                                mouseOX = x;
                                                mouseOY = y;
                                                mouseX = x;
                                                mouseY = y;
                                                noMove = true;
                                                breaking = true;
                                            }
                                        }
                                        if(x > fx->x+nodeCX && x < fx->x+fx->w+nodeCX && y > fx->y+nodeCY && y < fx->y+fx->h+nodeCY) {
                                            if(x > fx->x+fx->w+nodeCX-8-4 && x < fx->x+fx->w+nodeCX-4 && y > fx->y+nodeCY+4 && y < fx->y+nodeCY+4+8) {
                                                noMove = true;
                                                //delete Node
                                                toolTip = "";
                                                saveUndo();
                                                for(int n2 = texs.at(currentTexture)->fxs.size()-1; n2 >= 0; n2--) {
                                                    nEffect* fx2 = texs.at(currentTexture)->fxs.at(n2);
                                                    for(int in = 0; in < fx2->inputs.size(); in++) {
                                                        if(fx2->inputs.at(in)->s != NULL && fx2->inputs.at(in)->s->parent == fx) {
                                                            fx2->inputs.at(in)->s = NULL;
                                                            fx2->undone = true;
                                                            texs.at(currentTexture)->genTex();
                                                        }
                                                    }
                                                }
                                                if(fx->loading) {
                                                    fx->abort = true;
                                                    fx->deleted = true;
                                                } else {
                                                    texs.at(currentTexture)->fxs.erase(std::remove(texs.at(currentTexture)->fxs.begin(), texs.at(currentTexture)->fxs.end(), fx), texs.at(currentTexture)->fxs.end());
                                                    delete fx;
                                                }
                                            }
                                            if(x > fx->x+fx->w+nodeCX-8-4-8-4 && x < fx->x+fx->w+nodeCX-4-8-4 && y > fx->y+nodeCY+4 && y < fx->y+nodeCY+4+8) {
                                                noMove = true;
                                                //duplicate Node
                                                saveUndo();
                                                nEffect* d = new nEffect(fx->luafn, fx->fxname);
                                                d->x = fx->x+8;
                                                d->y = fx->y+8;
                                                for(int pa = 0; pa < d->params.size(); pa++) {
                                                    d->params.at(pa)->value = fx->params.at(pa)->value;
                                                    d->params.at(pa)->points.clear();
                                                    for(int po = 0; po < fx->params.at(pa)->points.size(); po++) {
                                                        CPoint* cp = new CPoint();
                                                        cp->a = fx->params.at(pa)->points.at(po)->a;
                                                        cp->r = fx->params.at(pa)->points.at(po)->r;
                                                        cp->g = fx->params.at(pa)->points.at(po)->g;
                                                        cp->b = fx->params.at(pa)->points.at(po)->b;
                                                        d->params.at(pa)->points.push_back(cp);
                                                    }
                                                }
                                                texs.at(currentTexture)->fxs.push_back(d);
                                                texs.at(currentTexture)->genTex();
                                            }
                                            for(int out = 0; out < fx->outputs.size(); out++) {
                                                if(x > int(fx->x+fx->w/2.0+nodeCX-int(texSizeX/2.0)) && x < int(fx->x+fx->w/2.0+nodeCX+int(texSizeX/2.0)) && y > int(fx->y+nodeCY+fx->outputs.at(out)->y+4-texSizeY/2.0) && y < int(fx->y+nodeCY+fx->outputs.at(out)->y+4+texSizeY/2.0)) {
                                                    //export tex
                                                    if(fx->doneTimer < 3) {
                                                        if(!browserOpen) {
                                                            currentSocket = fx->outputs.at(out);
                                                            openBrowser(currentDir, 0, 2);
                                                        }
                                                    }
                                                    noMove = true;
                                                }
                                            }
                                            for(int p = 0; p < fx->params.size(); p++) {
                                                Parameter* pa = fx->params.at(p);
                                                if(x > fx->x+nodeCX+pa->x && x < fx->x+nodeCX+pa->x+pa->w && y > fx->y+nodeCY+pa->y && y < fx->y+nodeCY+pa->y+pa->h) {
                                                    pa->mouseDown(x, y, fx->x+nodeCX, fx->y+nodeCY, 0, fx);
                                                    noMove = true;
                                                }
                                            }
                                            if(!noMove) {
                                                draggingNode = true;
                                                draggedNode = n;
                                                mouseOX = x;
                                                mouseOY = y;
                                                mouseX = x;
                                                mouseY = y;
                                            }
                                            breaking = true;
                                        }
                                        if(breaking) {
                                            break;
                                        }
                                    }
                                }
                            }
                            if(x > barX) {
                                if(x > screenW-barXRight-5-8 && x < screenW-barXRight-5 && y > 5 && y < 5+8) {
                                    //save file
                                    if(!browserOpen) {
                                        openBrowser(currentDir, 0, 5);
                                    }
                                }
                                if(x > screenW-barXRight-5-8-5-8 && x < screenW-barXRight-5-5-8 && y > 5 && y < 5+8) {
                                    //load file
                                    if(!browserOpen) {
                                        openBrowser(currentDir, 0, 4);
                                    }
                                }
                                if(x > screenW-barXRight-5-8-5-8-5-8 && x < screenW-barXRight-5-5-8-5-8 && y > 5 && y < 5+8) {
                                    //new file
                                    newFile();
                                }
                            }
                        }
                        if(y > barY) {
                            int spaceT = 1;
                            int wT = barXRight-2-7;
                            float sizeT = texSizeX+1;
                            int cT = wT/sizeT;
                            int leftspaceT = wT-sizeT*cT;
                            int maxHTex = 0;
                            leftspaceT = 0;
                            for(int p = 0; p < texs.size(); p++) {
                                int xt = p%cT;
                                int yt = p/cT;
                                if(yt*(sizeT)+sizeT > maxHTex) {
                                    maxHTex = yt*(sizeT)+sizeT;
                                }
                                if(y > barY && y < screenH-5-8-5-(collH-4)-5) {
                                    if(x > screenW-barXRight+1+xt*(sizeT) && x < screenW-barXRight+1+xt*(sizeT)+sizeT && y > barY+1+yt*(sizeT)-texScroll && y < barY+1+yt*(sizeT)+sizeT) {
                                        currentTexture = p;
                                    }
                                }
                            }
                            int scrollW = 6;
                            int scrollH = maxHTex;
                            if(scrollH < screenH-barY-5-8-5-8-(14-8)/2-5) {
                                scrollH = screenH-barY-5-8-5-8-(14-8)/2-5;
                            }
                            if(x > screenW-scrollW && x < screenW && y > barY+int(int(texScroll)*(screenH-barY-5-8-5-8-(14-8)/2-5)/float(scrollH)) && y < barY+int(int(texScroll)*(screenH-barY-5-8-5-8-(14-8)/2-5)/float(scrollH))+int((screenH-barY-5-8-5-8-(14-8)/2-5)*(screenH-barY-5-8-5-8-(14-8)/2-5)/float(scrollH))) {
                                draggingSBar = true;
                                mouseOX = x;
                                mouseOY = y;
                                mouseX = x;
                                mouseY = y;
                                sBarDrag = 5;
                                sRatio = scrollH/float(screenH-barY-5-8-5-8-(14-8)/2-5);
                            }
                        }
                        if(y < barY) {
                            for(int i = 0; i < newEffects.size(); i++) {
                                nEffect* fx = newEffects.at(i);
                                if(x > 0 && x <= barX-2-7 && y > collH*i-newEScroll && y <= collH*i-newEScroll+collH && y < barY) {
                                    //new node
                                    saveUndo();
                                    if(fx->isPreset) {
                                        fx->presetError = false;
                                        fx->presetFxs.clear();
                                        luaL_dofile(fx->L, fx->luafn.c_str());
                                        lua_settop(fx->L, 0);
                                        lua_getglobal(fx->L, "apply");
                                        lua_pcall(fx->L, 0, 0, 0);
                                        if(!fx->presetError) {
                                            for(int i = 0; i < fx->presetFxs.size(); i++) {
                                                texs.at(currentTexture)->fxs.push_back(fx->presetFxs.at(i));
                                            }
                                        }
                                    } else {
                                        texs.at(currentTexture)->fxs.push_back(new nEffect(fx->luafn, fx->fxname));
                                    }
                                    texs.at(currentTexture)->genTex();
                                }
                            }
                            int toolsScrolli = toolsScroll;
                            int space = 1;
                            float offset = 0;
                            int w = barXRight-10+space-offset*2-7-7;
                            float size = 12;
                            int c = w/size;
                            int leftspace = w-size*c;
                            leftspace = 0;
                            int maxHPal = 0;
                            for(int p = 0; p < palette.size(); p++) {
                                int Cx = p%c;
                                int Cy = p/c;
                                int colorX = leftspace+offset+Cx*(size)+screenW-barXRight+5;
                                int colorY = offset+3+Cy*(size)+12-toolsScrolli-palScroll;
                                if(Cy*(size)+size > maxHPal) {
                                    maxHPal = Cy*(size)+size;
                                }
                                if(y > 3+12-toolsScroll && y < 3+12-toolsScroll+barY3-2-5-8-5) {
                                    if(x > colorX && x < colorX+size && y > colorY && y < colorY+size) {
                                        selectedColor = p;
                                        Color* rgb = palette.at(selectedColor);
                                        Color hsv = RGBtoHSV(rgb->r, rgb->g, rgb->b);
                                        colorParams.at(0)->value = hsv.r;
                                        colorParams.at(0)->value2 = hsv.g/100.0;
                                        colorParams.at(0)->value3 = hsv.b/100.0;
                                        colorParams.at(1)->value = rgb->r;
                                        colorParams.at(2)->value = rgb->g;
                                        colorParams.at(3)->value = rgb->b;
                                        colorParams.at(4)->value = hsv.r;
                                        colorParams.at(5)->value = hsv.g;
                                        colorParams.at(6)->value = hsv.b;
                                    }
                                }
                            }
                            int scrollW = 6;
                            int scrollH = maxHPal;
                            if(scrollH < barY3-5-8-5) {
                                scrollH = barY3-5-8-5;
                            }
                            if(x > screenW-4-8-scrollW && x < screenW-4-8 && y > 2+12-toolsScrolli+int(int(palScroll)*(barY3-5-8-5)/float(scrollH))+1 && y < 2+12-toolsScrolli+int(int(palScroll)*(barY3-5-8-5)/float(scrollH))+1+int((barY3-5-8-5)*(barY3-5-8-5-3)/float(scrollH))+1) {
                                draggingSBar = true;
                                mouseOX = x;
                                mouseOY = y;
                                mouseX = x;
                                mouseY = y;
                                sBarDrag = 3;
                                sRatio = scrollH/float(barY3-5-8-5-3);
                            }

                            scrollH = barY5;
                            if(scrollH < barY) {
                                scrollH = barY;
                            }
                            if(x > screenW-scrollW && x < screenW && y > int(int(toolsScroll)*(barY-2)/float(scrollH)) && y < int(int(toolsScroll)*(barY-2)/float(scrollH))+int((barY)*(barY-2)/float(scrollH))+1) {
                                draggingSBar = true;
                                mouseOX = x;
                                mouseOY = y;
                                mouseX = x;
                                mouseY = y;
                                sBarDrag = 4;
                                sRatio = scrollH/float(barY);
                            }
                            if(x > screenW-5-8-5-5-8-5-8-5-8-5-8-7 && x < screenW-5-5-5-8-5-8-5-8-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                                saveUndo();
                                Color* c = new Color(0, 0, 0);
                                palette.push_back(c);
                                selectedColor = palette.size()-1;
                                Color* rgb = palette.at(selectedColor);
                                Color hsv = RGBtoHSV(rgb->r, rgb->g, rgb->b);
                                colorParams.at(0)->value = hsv.r;
                                colorParams.at(0)->value2 = hsv.g/100.0;
                                colorParams.at(0)->value3 = hsv.b/100.0;
                                colorParams.at(1)->value = rgb->r;
                                colorParams.at(2)->value = rgb->g;
                                colorParams.at(3)->value = rgb->b;
                                colorParams.at(4)->value = hsv.r;
                                colorParams.at(5)->value = hsv.g;
                                colorParams.at(6)->value = hsv.b;

                                for(int i = 0; i < texs.size(); i++) {
                                    Texture* t = texs.at(i);
                                    bool updateTex = false;
                                    //UPDATE COLORS
                                }

                                paletteChanged();
                            }
                            if(x > screenW-5-8-5-5-8-5-8-5-8-7 && x < screenW-5-5-5-8-5-8-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                                saveUndo();
                                Color* rgb = palette.at(selectedColor);
                                Color* c = new Color(rgb->r, rgb->g, rgb->b);
                                palette.insert(palette.begin()+selectedColor, c);
                                selectedColor++;
                                Color hsv = RGBtoHSV(rgb->r, rgb->g, rgb->b);
                                colorParams.at(0)->value = hsv.r;
                                colorParams.at(0)->value2 = hsv.g/100.0;
                                colorParams.at(0)->value3 = hsv.b/100.0;
                                colorParams.at(1)->value = rgb->r;
                                colorParams.at(2)->value = rgb->g;
                                colorParams.at(3)->value = rgb->b;
                                colorParams.at(4)->value = hsv.r;
                                colorParams.at(5)->value = hsv.g;
                                colorParams.at(6)->value = hsv.b;

                                for(int i = 0; i < texs.size(); i++) {
                                    Texture* t = texs.at(i);
                                    bool updateTex = false;
                                    //UPDATECOLORS
                                }

                                paletteChanged();
                            }
                            if(x > screenW-5-8-5-5-8-5-8-7 && x < screenW-5-5-5-8-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                                saveUndo();
                                Color* c = palette.at(selectedColor);
                                if(palette.size() > 1) {
                                    delete c;
                                    palette.erase(std::remove(palette.begin(), palette.end(), c), palette.end());
                                    if(selectedColor >= palette.size()) {
                                        selectedColor = palette.size()-1;
                                    }
                                } else {
                                    c->r = 0;
                                    c->g = 0;
                                    c->b = 0;
                                }
                                Color* rgb = palette.at(selectedColor);
                                Color hsv = RGBtoHSV(rgb->r, rgb->g, rgb->b);
                                colorParams.at(0)->value = hsv.r;
                                colorParams.at(0)->value2 = hsv.g/100.0;
                                colorParams.at(0)->value3 = hsv.b/100.0;
                                colorParams.at(1)->value = rgb->r;
                                colorParams.at(2)->value = rgb->g;
                                colorParams.at(3)->value = rgb->b;
                                colorParams.at(4)->value = hsv.r;
                                colorParams.at(5)->value = hsv.g;
                                colorParams.at(6)->value = hsv.b;

                                for(int i = 0; i < texs.size(); i++) {
                                    Texture* t = texs.at(i);
                                    bool updateTex = false;
                                    //UPDATECOLORS
                                }

                                paletteChanged();
                            }
                            if(x > screenW-5-8-5-5-8-7 && x < screenW-5-5-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                                //import
                                if(!browserOpen) {
                                    openBrowser(currentDir, 0, 0);
                                }
                            }
                            if(x > screenW-5-8-5-7 && x < screenW-5-5-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                                //export
                                if(!browserOpen) {
                                    openBrowser(currentDir, 0, 1);
                                }
                            }
                            for(int i = 0; i < colorParams.size(); i++) {
                                colorParams.at(i)->mouseDown(x, y, screenW-barXRight, barY3+2+12-toolsScrolli, -1, NULL);
                            }
                        }
                        int scrollW = 6;
                        int count = newEffects.size();
                        if(draggingFX && draggedLayer == -1) {
                            count--;
                        }
                        int scrollH = collH*count;
                        if(scrollH < barY) {
                            scrollH = barY;
                        }
                        if(x > barX-scrollW && x < barX && y > int(int(newEScroll)*(barY-2)/float(scrollH)) && y < int(int(newEScroll)*(barY-2)/float(scrollH))+int((barY)*(barY-2)/float(scrollH))+1) {
                            draggingSBar = true;
                            mouseOX = x;
                            mouseOY = y;
                            mouseX = x;
                            mouseY = y;
                            sBarDrag = 2;
                            sRatio = scrollH/float(barY);
                        }
                        if(x > 0 && x < screenW && y > barY && y < screenH) {
                            Texture* t = texs.at(currentTexture);
                            int h = -layersScroll;
                            int iconsize = 8;
                            int iconoffset = 5;
                            int maxW = -layersScrollH;
                            int scrollW = 6;
                            int scrollH = h+layersScroll+collH;
                            if(scrollH < screenH-(barY)) {
                                scrollH = screenH-(barY);
                            }
                            int scrollH2 = maxW+layersScrollH;
                            if(scrollH2 < screenW-barX-barXRight-2-scrollW) {
                                scrollH2 = screenW-barX-barXRight-2-scrollW;
                            }
                            if(x > screenW-barXRight-scrollW-1 && x < screenW-barXRight-scrollW-1+scrollW && y > barY+int(int(layersScroll)*(screenH-(barY)-scrollW)/float(scrollH)) && y < barY+int(int(layersScroll)*(screenH-(barY)-scrollW)/float(scrollH))+int((screenH-(barY)-scrollW)*(screenH-(barY))/float(scrollH))+1) {
                                draggingSBar = true;
                                mouseOX = x;
                                mouseOY = y;
                                mouseX = x;
                                mouseY = y;
                                sBarDrag = 0;
                                sRatio = scrollH/float(screenH-(barY)-scrollW);
                            }
                            if(x > barX+1+int(int(layersScrollH)*(screenW-barX-barXRight-2-scrollW)/float(scrollH2)) && x < barX+1+int(int(layersScrollH)*(screenW-barX-barXRight-2-scrollW)/float(scrollH2))+int((screenW-barX-barXRight-2-scrollW)*(screenW-barX-barXRight-2-scrollW)/float(scrollH2)) && y > screenH-scrollW && y < screenH) {
                                draggingSBar = true;
                                mouseOX = x;
                                mouseOY = y;
                                mouseX = x;
                                mouseY = y;
                                sBarDrag = 1;
                                sRatio = scrollH2/float(screenW-barX-barXRight-2-scrollW);
                            }
                            if(y > barY+h+iconoffset && y <= barY+h+iconoffset+iconsize && x > iconoffset && x <= iconoffset+iconsize) {
                            }
                        }
                        if(x > barX && x < barX+UIdragRange && y < barY && !draggingFX && !draggingSBar) {
                            draggingUI = true;
                            draggingSocket = false;
                            draggingNode = false;
                            mouseOX = x;
                            mouseOY = y;
                            mouseX = x;
                            mouseY = y;
                            UIdragType = 0;

                        }
                        if(x > screenW-barXRight-UIdragRange && x < screenW-barXRight && y < barY && !draggingFX && !draggingSBar) {
                            draggingUI = true;
                            draggingSocket = false;
                            draggingNode = false;
                            mouseOX = x;
                            mouseOY = y;
                            mouseX = x;
                            mouseY = y;
                            UIdragType = 2;
                        }
                    }
                }
                if(e.button.button == SDL_BUTTON_RIGHT) {
                }
                if(e.button.button == SDL_BUTTON_MIDDLE) {
                    int x = e.button.x/screenScale;
                    int y = e.button.y/screenScale;
                    if(x > barX && x < screenW-barXRight) {
                        draggingCam2 = true;
                        mouseOX = x;
                        mouseOY = y;
                        mouseX = x;
                        mouseY = y;
                    }
                }
            }
            if(e.type == SDL_MOUSEBUTTONUP) {
                if(e.button.button == SDL_BUTTON_MIDDLE) {
                    draggingCam2 = false;
                }
                if(e.button.button == SDL_BUTTON_LEFT) {
                    int x = e.button.x/screenScale;
                    int y = e.button.y/screenScale;
                    textTypeTemp = NULL;
                    textTypeFxTemp = NULL;
                    textTypeLayerTemp = -2;
                    Texture* t = texs.at(currentTexture);
                    if(view == 0) {
                        for(int n = texs.at(currentTexture)->fxs.size()-1; n >= 0; n--) {
                            nEffect* fx = texs.at(currentTexture)->fxs.at(n);
                            for(int p = 0; p < fx->params.size(); p++) {
                                Parameter* pa = fx->params.at(p);
                                pa->mouseUp(x, y, fx->x+nodeCX, fx->y+nodeCY, 0, fx);
                            }
                        }
                        if(draggingSocket) {
                            nEffect* fx = t->fxs.at(draggedNode);
                            Socket* s = fx->outputs.at(draggedSocket);
                            if(s->snapped) {
                                s->s->s = s;
                                s->s->parent->undone = true;
                            }
                            s->s = NULL;
                            draggingSocket = false;
                            t->genTex();
                        }
                        draggingCam = false;
                        draggingNode = false;
                        draggingUI = false;
                        draggingSBar = false;
                        int toolsScrolli = toolsScroll;
                        for(int i = 0; i < colorParams.size(); i++) {
                            colorParams.at(i)->mouseUp(x, y, screenW-barXRight, barY3+2+12-toolsScrolli, -1, NULL);
                        }
                        draggingLayer = false;
                        draggingFX = false;
                        int h = -layersScroll;
                    }
                }
            }
            if(e.type == SDL_MOUSEMOTION) {
                int x = e.motion.x/screenScale;
                int y = e.motion.y/screenScale;
                mouseX = x;
                mouseY = y;
                textTypeTemp = NULL;
                textTypeFxTemp = NULL;
                textTypeLayerTemp = -2;
                preview = NULL;
                bool ttSet = false;
                for(int i = 0; i < newEffects.size(); i++) {
                    nEffect* fx = newEffects.at(i);
                    int newEScrolli = newEScroll;
                    if(x > 0 && x <= barX-2-7 && y > collH*i-newEScroll && y <= collH*i-newEScroll+collH && y < barY) {
                        toolTip = newEffects.at(i)->desc+"\n\n\"";
                        if(newEffects.at(i)->isPreset) {
                            toolTip += "Presets/"+fx->fxname+"\"";
                        } else {
                            toolTip += "Nodes/"+fx->fxname+"\"";
                        }
                        ttSet = true;
                    }
                }
                if(x > 0 && x < screenW && y > barY && y < screenH) {
                    Texture* t = texs.at(currentTexture);
                    int h = -layersScroll;
                    int iconsize = 8;
                    int iconoffset = 5;
                    if(y > barY+h+iconoffset && y <= barY+h+iconoffset+iconsize && x > iconoffset && x <= iconoffset+iconsize) {
                        toolTip = "New layer";
                        ttSet = true;
                    }
                }
                if(!browserOpen) {
                    if(x > barX) {
                        if(x > screenW-barXRight-5-8 && x < screenW-barXRight-5 && y > 5+8+5 && y < 5+8+5+8) {
                            //info
                            toolTip = "Tilemancer "+to_string(Vmajor)+"."+to_string(Vminor)+"."+to_string(Vrevision);
                            toolTip += "\nDeveloped by @ledgamedev";
                            ttSet = true;
                        }
                        if(x > screenW-barXRight-5-8 && x < screenW-barXRight-5 && y > 5 && y < 5+8) {
                            //save file
                            toolTip = "Save file";
                            ttSet = true;
                        }
                        if(x > screenW-barXRight-5-8-5-8 && x < screenW-barXRight-5-5-8 && y > 5 && y < 5+8) {
                            //load file
                            toolTip = "Load file";
                            ttSet = true;
                        }
                        if(x > screenW-barXRight-5-8-5-8-5-8 && x < screenW-barXRight-5-5-8-5-8 && y > 5 && y < 5+8) {
                            //new file
                            toolTip = "New file";
                            ttSet = true;
                        }
                    }
                }
                if(x > barX && x < screenW-barXRight) {
                    for(int n = texs.at(currentTexture)->fxs.size()-1; n >= 0; n--) {
                        nEffect* fx = texs.at(currentTexture)->fxs.at(n);
                        bool breaking = false;
                        if(x > fx->x+nodeCX && x < fx->x+fx->w+nodeCX && y > fx->y+nodeCY && y < fx->y+fx->h+nodeCY) {
                            for(int p = 0; p < fx->params.size(); p++) {
                                Parameter* pa = fx->params.at(p);
                                if(x > fx->x+nodeCX+pa->x && x < fx->x+nodeCX+pa->x+pa->w && y > fx->y+nodeCY+pa->y && y < fx->y+nodeCY+pa->y+pa->h) {
                                    toolTip = pa->tt;
                                    ttSet = true;
                                }
                            }
                            if(x > fx->x+fx->w+nodeCX-8-4 && x < fx->x+fx->w+nodeCX-4 && y > fx->y+nodeCY+4 && y < fx->y+nodeCY+4+8) {
                                toolTip = "Delete node";
                                ttSet = true;
                            }
                            if(x > fx->x+fx->w+nodeCX-8-4-8-4 && x < fx->x+fx->w+nodeCX-4-8-4 && y > fx->y+nodeCY+4 && y < fx->y+nodeCY+4+8) {
                                toolTip = "Duplicate node";
                                ttSet = true;
                            }
                            for(int out = 0; out < fx->outputs.size(); out++) {
                                if(x > int(fx->x+fx->w/2.0+nodeCX-int(texSizeX/2.0)) && x < int(fx->x+fx->w/2.0+nodeCX+int(texSizeX/2.0)) && y > int(fx->y+nodeCY+fx->outputs.at(out)->y+4-texSizeY/2.0) && y < int(fx->y+nodeCY+fx->outputs.at(out)->y+4+texSizeY/2.0)) {
                                    toolTip = "Export tile";
                                    ttSet = true;
                                    preview = fx->outputs.at(out);
                                }
                            }
                            breaking = true;
                        }
                        for(int in = 0; in < fx->inputs.size(); in++) {
                            if(x > fx->x+nodeCX-4 && x < fx->x+nodeCX+4 && y > fx->y+nodeCY+fx->inputs.at(in)->y && y < fx->y+nodeCY+fx->inputs.at(in)->y+8) {
                                breaking = true;
                            }
                        }
                        for(int out = 0; out < fx->outputs.size(); out++) {
                            if(x > fx->x+nodeCX+fx->w-4 && x < fx->x+nodeCX+fx->w+4 && y > fx->y+nodeCY+fx->outputs.at(out)->y && y < fx->y+nodeCY+fx->outputs.at(out)->y+8) {
                                breaking = true;
                            }
                        }
                        if(breaking) {
                            break;
                        }
                    }
                }
                if(y < barY) {
                    int toolsScrolli = toolsScroll;
                    if(x > screenW-5-8-5-5-8-5-8-5-8-5-8-7 && x < screenW-5-5-5-8-5-8-5-8-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                        toolTip = "New color";
                        ttSet = true;
                    }
                    if(x > screenW-5-8-5-5-8-5-8-5-8-7 && x < screenW-5-5-5-8-5-8-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                        toolTip = "Duplicate color";
                        ttSet = true;
                    }
                    if(x > screenW-5-8-5-5-8-5-8-7 && x < screenW-5-5-5-8-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                        toolTip = "Delete color";
                        ttSet = true;
                    }
                    if(x > screenW-5-8-5-5-8-7 && x < screenW-5-5-5-8-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                        //import
                        toolTip = "Load palette";
                        ttSet = true;
                    }
                    if(x > screenW-5-8-5-7 && x < screenW-5-5-7 && y > 1+12-toolsScrolli+barY3-8-5 && y < 1+12-toolsScrolli+barY3-5) {
                        //export
                        toolTip = "Save palette";
                        ttSet = true;
                    }
                    /*renderColor(screenW-barXRight+5+offsetA, barY2+9+3+3+offsetA-toolsScrolli, size-offsetA*2, size-offsetA*2, &rampA);
                     renderColor(screenW-(size-offsetB)-5, barY2+9+3+3+offsetB-toolsScrolli, size-offsetB*2, size-offsetB*2, &rampB);*/
                    for(int i = 0; i < colorParams.size(); i++) {
                        Parameter* pr = colorParams.at(i);
                        if(x-(screenW-barXRight) > pr->x && x-(screenW-barXRight) <= pr->x+pr->w && y-(barY3+2+12-toolsScrolli) > pr->y && y-(barY3+2+12-toolsScrolli) <= pr->y+pr->h) {
                            toolTip = pr->tt;
                            ttSet = true;
                        }
                    }
                }
                if(!ttSet) {
                    toolTip = "";
                }

                int ox = 0;
                int oy = 0;
                if(draggingParam) {
                    int mx = 0;
                    int my = 0;
                    SDL_GetGlobalMouseState(&mx, &my);
                    int sx = 0;
                    int sy = 0;
                    int sw = 0;
                    int sh = 0;
                    SDL_GetWindowPosition(window, &sx, &sy);
                    if(OS == 0) {
                        mx += sx;
                        my += sy;
                    }
                    SDL_GetWindowSize(window, &sw, &sh);
                    while(mx < sx) {
                        mx+=sw;
                        ox+=sw/screenScale;
                    }
                    while(mx > sx+sw) {
                        mx-=sw;
                        ox-=sw/screenScale;
                    }
                    while(my < sy) {
                        my+=sh;
                        oy+=sh/screenScale;
                    }
                    while(my > sy+sh) {
                        my-=sh;
                        oy-=sh/screenScale;
                    }
                    if(OS == 0) {
                        warpMouse(mx-sx, my-sy);
                    } else if(OS == 1) {
                        warpMouse(mx, my);
                    }
                    x += ox;
                    y += oy;
                }

                for(int n = texs.at(currentTexture)->fxs.size()-1; n >= 0; n--) {
                    nEffect* fx = texs.at(currentTexture)->fxs.at(n);
                    for(int p = 0; p < fx->params.size(); p++) {
                        Parameter* pa = fx->params.at(p);
                        pa->oldmX+=ox;
                        pa->oldmY+=oy;
                        pa->mouseMove(x, y, fx->x+nodeCX, fx->y+nodeCY, 0, fx);
                    }
                }

                if(view == 0) {
                    int toolsScrolli = toolsScroll;
                    for(int i = 0; i < colorParams.size(); i++) {
                        colorParams.at(i)->oldmX+=ox;
                        colorParams.at(i)->oldmY+=oy;
                        colorParams.at(i)->mouseMove(x, y, screenW-barXRight, barY3+2+12-toolsScrolli, -1, NULL);
                    }
                }
            }
            if(e.type == SDL_FINGERDOWN) {
                mouseSX = -1;
                mouseSY = -1;
                scrollSet = false;
            }
            if(e.type == SDL_MULTIGESTURE) {
                mouseSDX = e.tfinger.x*screenH;
                mouseSDY = e.tfinger.y*screenH;
                if(!scrollSet && mouseSX != -1 && mouseSY != -1) {
                    float distX = mouseSDX-mouseSX;
                    float distY = mouseSDY-mouseSY;
                    float dist = sqrt(distX*distX+distY*distY);
                    //if(dist > 2) {
                        if(abs(mouseSDY-mouseSY) >= abs(mouseSDX-mouseSX)) {
                            scrollDir = true;
                            scrollSet = true;
                        } else {
                            scrollDir = false;
                            scrollSet = true;
                        }
                    //}
                }
                if(view == 0) {
                    if(scrollSet) {
                        if(!browserOpen) {
                            if(mouseSX != -1 && mouseSY != -1 && mouseX > barX && mouseX < screenW-barXRight) {
                                nodeCX += mouseSDX-mouseSX;
                                nodeCY += mouseSDY-mouseSY;
                            }
                        }
                        bool adjust = false;
                        if(mouseSX != -1 && mouseSY != -1 && mouseY > barY && mouseX < screenW-barXRight && scrollDir) {
                            layersScroll -= mouseSDY-mouseSY;
                            adjust = true;
                        }
                        if(mouseSX != -1 && mouseSY != -1 && mouseY > barY && mouseX > barX && mouseX < screenW-barXRight && !scrollDir) {
                            layersScrollH -= mouseSDX-mouseSX;
                            adjust = true;
                        }
                        if(mouseSX != -1 && mouseSY != -1 && mouseY < barY && mouseX < barX) {
                            newEScroll -= mouseSDY-mouseSY;
                            adjust = true;
                        }
                        if(adjust) {
                            adjustLayersScroll();
                        }

                        bool adjust2 = false;
                        bool scrollingPal = false;
                        if(mouseSX != -1 && mouseSY != -1 && mouseX > screenW-barXRight+5 && mouseX < screenW-5 && mouseY < barY && mouseY > 3+12-toolsScroll && mouseY < 3+12-toolsScroll+barY3-2-5-8-5) {
                            palScroll -= mouseSDY-mouseSY;
                            adjust2 = true;
                            scrollingPal = true;
                        }
                        if(mouseSX != -1 && mouseSY != -1 && mouseX > screenW-barXRight && mouseY < barY && !scrollingPal) {
                            toolsScroll -= mouseSDY-mouseSY;
                            adjust2 = true;
                        }
                        if(mouseSX != -1 && mouseSY != -1 && mouseX > screenW-barXRight && mouseY > barY && mouseY < screenH-5-8-5-(collH-4)-5) {
                            texScroll -= mouseSDY-mouseSY;
                            adjust2 = true;
                        }
                        if(adjust2) {
                            adjustToolsScroll();
                        }

                        bool adjust3 = false;
                        int title = collH-4;
                        int bSpace = 5;
                        if(mouseSX != -1 && mouseSY != -1 && mouseX > barX+bSpace && mouseX < screenW-barXRight-bSpace && mouseY > bSpace*2+title && mouseY < barY-bSpace && browserOpen) {
                            browserScroll -= mouseSDX-mouseSX;
                            adjust3 = true;
                        }
                        if(adjust3) {
                            adjustBrowserScroll();
                        }
                    }
                }
                mouseSX = e.tfinger.x*screenH;
                mouseSY = e.tfinger.y*screenH;
            }
            if(e.type == SDL_MOUSEWHEEL) {
                if(!browserOpen) {
                    if(e.wheel.y < 0) {
                        if(mouseX > barX && mouseX < screenW-barXRight && mouseY > 0 && mouseY < barY) {
                            if(zoom < 2) {
                                zoom++;
                            }
                        }
                    } else if(e.wheel.y > 0) {
                        if(mouseX > barX && mouseX < screenW-barXRight && mouseY > 0 && mouseY < barY) {
                            if(zoom > 1) {
                                zoom--;
                            }
                        }
                    }
                }
            }
            if(e.type == SDL_TEXTINPUT) {
                if(browserOpen) {
                    filenameB.append(e.text.text);
                    overwrite = false;
                }
                if(textType != NULL && (textType->ID == 1 || textType->ID == 2)) {
                    if(textType->typing.length() < 10) {
                        textType->typing.append(e.text.text);
                    }
                }
            }
            if(e.type == SDL_KEYUP) {
                if(e.key.keysym.sym == SDLK_LSHIFT) {
                    camMoving = false;
                }
            }
            if(e.type == SDL_KEYDOWN) {
                if(e.key.keysym.sym == SDLK_LSHIFT) {
                    camMoving = true;
                }
                if(e.key.keysym.sym == SDLK_BACKSPACE) {
                    if(filenameB.size() > 0) {
                        filenameB.pop_back();
                    }
                    overwrite = false;
                    if(textType != NULL && (textType->ID == 1 || textType->ID == 2)) {
                        if(textType->typing.size() > 0) {
                            textType->typing.pop_back();
                        }
                    }
                }
                if(e.key.keysym.sym == SDLK_z && SDL_GetModState() & (KMOD_GUI | KMOD_CTRL)) {
                    undo();
                }
                if(e.key.keysym.sym == SDLK_y && SDL_GetModState() & (KMOD_GUI | KMOD_CTRL)) {
                    redo();
                }
                if(e.key.keysym.sym == SDLK_n && SDL_GetModState() & (KMOD_GUI | KMOD_CTRL)) {
                    newFile();
                }
                if(e.key.keysym.sym == SDLK_s && SDL_GetModState() & (KMOD_GUI | KMOD_CTRL)) {
                    if(!browserOpen) {
                        openBrowser(currentDir, 0, 5);
                    }
                }
                if(e.key.keysym.sym == SDLK_o && SDL_GetModState() & (KMOD_GUI | KMOD_CTRL)) {
                    if(!browserOpen) {
                        openBrowser(currentDir, 0, 4);
                    }
                }
                /*if((e.key.keysym.unicode >= 33 && e.key.keysym.unicode <= 95) || e.key.keysym.unicode >= 97 && e.key.keysym.unicode <= 126) || e.key.keysym.unicode == 32) {
                }*/
                if(e.key.keysym.sym == SDLK_RETURN) {
                    if(browserOpen) {
                        string fullDir = currentDir;
                        if(fullDir.size() != 1) {
                            fullDir = fullDir.append("/");
                        }
                        fullDir = fullDir.append(filenameB);
                        bool folder = false;
                        for(int i = 0; i < filenames.size(); i++) {
                            if(!strcmp(filenames.at(i)->name.c_str(), filenameB.c_str()) && filenames.at(i)->folder) {
                                openBrowser(fullDir, 0, browserMode);
                                folder = true;
                            }
                        }
                        if(!folder) {
                            browserAction(fullDir, filenameB, currentDir);
                        }
                    }
                    if(textType != NULL) {
                        textType->value = atoi(textType->typing.c_str());
                        if(textType->value > textType->value3 && textType->value3 != -1) {
                            textType->value = textType->value3;
                        }
                        if(textType->value < textType->value2 && textType->value2 != -1) {
                            textType->value = textType->value2;
                        }
                        updateDrag(textTypeLayer, textTypeFx, textType);
                    }
                    textType = NULL;
                }
                if(e.key.keysym.sym == SDLK_LEFT) {
                    if(SDL_GetModState() & (KMOD_GUI | KMOD_CTRL)) {
                        //swap with left side
                    }
                    currentTexture--;
                    if(currentTexture < 0) {
                        currentTexture = 0;
                    }
                }
                if(e.key.keysym.sym == SDLK_RIGHT) {
                    if(SDL_GetModState() & (KMOD_GUI | KMOD_CTRL)) {
                        //swap with right side
                    }
                    currentTexture++;
                    if(currentTexture >= texs.size()) {
                        currentTexture = texs.size()-1;
                    }
                }
                if(e.key.keysym.sym == SDLK_ESCAPE) {
                    if(browserOpen) {
                        browserOpen = false;
                        fnUndo.clear();
                        fnRedo.clear();
                    }
                }
            }
            if(e.type == SDL_KEYUP) {
            }
        }
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);
        double newTime = 0;
        if(gameStarted) {
            newTime = SDL_GetTicks();
        } else {
            newTime = SDL_GetTicks();
            currentTime = SDL_GetTicks();
        }
        gameStarted = true;
        double frameTime = (newTime - currentTime)/1000.0;
        currentTime = newTime;
        accumulator += frameTime;
        if(fpsTimer == 0) {
            if(frameTime != 0) {
                fps = 1.0/frameTime;
            } else {
                fps = 0;
            }
            fpsTimer = 0;
        } else {
            fpsTimer--;
        }
        while(accumulator >= dt) {
            update();
            accumulator -= dt;
        }
        renderGL();
        SDL_GL_SwapWindow(window);

        double newTime2 = 0;
        if(gameStarted) {
            newTime2 = SDL_GetTicks();
        } else {
            newTime2 = SDL_GetTicks();
            lastTime = SDL_GetTicks();
        }
        double frameTime2 = (newTime2 - lastTime)/1000.0;
        double delayTime = dt-frameTime2;
        if(delayTime > 0) {
            SDL_Delay(delayTime*1000);
        }
        lastTime = SDL_GetTicks();
	}
    lua_close(L);
	SDL_DestroyWindow( window );
	SDL_Quit();
	return 0;
}
