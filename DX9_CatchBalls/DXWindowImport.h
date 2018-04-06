#pragma once

//Header files
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <XInput.h>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

//libraries
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")

//Program values
extern const string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
extern BOOL gameover;

//Direct3D objects
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 backbuffer;

//Direct3D functions
BOOL Direct3D_Init(HWND hwnd, int width, int height, BOOL fullscreen);
VOID Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(string filename);
VOID DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);

//Direct input objects
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;
extern XINPUT_GAMEPAD controllers[4];

//DirectInput functions
BOOL DirectInput_Init(HWND);
VOID DirectInput_Update();
VOID DirectInput_Shutdown();
int Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();
VOID XInput_Vibrate(int contNum = 0, int amount = 65535);
BOOL XInput_Controller_Found();

//Game functions
BOOL Game_Init(HWND);
VOID Game_Run(HWND);
VOID Game_End();

