#ifndef ROBORALLY_MAIN
#define ROBORALLY_MAIN

#include "window.h"
#include "sound_WAVEPCM.h"
#include "shadertexture.h"
#include "shaderlight.h"
#pragma comment (lib, "D3DLib.lib")

using namespace D3DLIB;

bool Initialize();
void Run();
void Shutdown();

void DrawInfo();
void DrawView();
void Movement();
void ScrollMove(unsigned char DIK, float &directionSpeedVar, float &outputVar,
				float frameTime, bool positive, bool allowNegatives);

D3DLIB::Window win;

Sound_WAVEPCM* sound1;
IDirectSoundBuffer8* buffer1;

Bitmap* image1;
Bitmap* cursor;
Model* earth;
Model* moon;

Shader_TEXTURE* shade_tex;
Shader_LIGHT* shade_light;

Texture* texhill;
Texture* texgrass;
Texture* texteapot;
Texture* texearth;
Texture* texstars;
Texture* texmoon;
Texture* texmouse;
Texture* texmouseclick;

float frameTime = 0.0f;
float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;
float movementX = 0.0f;
float movementZ = 0.0f;

float rotUSpeed = 0.0f;
float rotDSpeed = 0.0f;
float moveL = 0.0f;
float moveR = 0.0f;
float moveUP = 0.0f;
float moveD = 0.0f;

bool mousePressed = false;
bool mouseEnabled = false;
int originalX = 0;
int originalY = 0;
float rotXo = 0.0f;
float rotYo = 0.0f;
int mouseX = 0;
int mouseY = 0;

#endif