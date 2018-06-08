#pragma once

#include "Dependencies.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000 ? 1 : 0))

#define EDGE_BUFFER_LEN 150

//Define consts
const int SCREENW = 1024;
const int SCREENH = 768;
const int IMAGEW = 30;
const int IMAGEH = 30;

const int ROWNUM = 25;
const int COLNUM = 50;

const int MAPTYPE = 2;

const string APPTITLE = "MapLoadingTest";

//Direct3D objects
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DDEVICE9 dddevBackground = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface = NULL;
BOOL gameover = FALSE;

//Create painting thread
HANDLE hPaintingPlayerThread = INVALID_HANDLE_VALUE;
HANDLE hBackgroundThread = INVALID_HANDLE_VALUE;

//Player class
class Player
{
public:

	const int stepLen = 3;

	int xPos = 0;
	int yPos = 0;

	RECT playerRect;

	Player(int initXPos, int initYPos);
	virtual ~Player();

	virtual RECT GetPlayerRect();
	virtual void MoveUp();
	virtual void MoveDown();
	virtual void MoveRight();
	virtual void MoveLeft();
} *player = new Player(200, 200);

//Create the map struct
struct MapInfo
{
	PBYTE mem_file_buffer = NULL;
	DWORD *mapIndicatorArr = NULL;
	LPDIRECT3DSURFACE9 *mapSurface = NULL;
	DWORD rowNum = 0;
	DWORD colNum = 0;
} mapInfoStruct;

RECT clientRect;
RECT paintRect;

//Functions
BOOL Game_Init(HWND hwnd);
VOID Game_Run(HWND hwnd);
VOID Game_End(HWND hwnd);

BOOL LoadMap(HWND hwnd, MapInfo *mapInfo);

DWORD __stdcall RenderingPlayerFunc(LPVOID lpParameter);
long __stdcall WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
