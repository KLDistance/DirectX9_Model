#pragma once

#include <Windows.h>
#include <time.h>
#include <iostream>

//Beware that change the path to the DX9 2010 Jun project directory (includes and libs)
#include "Include//d3d9.h"
#include "Include//d3dx9.h"

#pragma comment(lib, "Lib//x86//d3d9.lib")
#pragma comment(lib, "Lib//x86//d3dx9.lib")

//Application title
const CHAR *APPTITLE = "Create Surface Program";

//Macro to read the keyboard
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

//Screen resolution
#define SCREENW 800
#define SCREENH 600

//Drect3D objects
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface = NULL;
BOOL gameover = FALSE;

//Function declarations
BOOL Game_Init(HWND hwnd);
VOID Game_Run(HWND hwnd);
VOID Game_End(HWND hwnd);
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPresInstance, LPSTR lpCmdLine, int nCmdShow);
