#include "MapLoading.h"

Player::Player(int initXPos, int initYPos)
{
	this->xPos = initXPos;
	this->yPos = initYPos;
}

Player::~Player()
{

}

RECT Player::GetPlayerRect()
{
	playerRect.top = this->yPos;
	playerRect.left = this->xPos;
	playerRect.bottom = this->yPos + IMAGEH;
	playerRect.right = this->xPos + IMAGEW;
	return playerRect;
}

void Player::MoveUp()
{
	this->yPos -= this->stepLen;
}

void Player::MoveDown()
{
	this->yPos += this->stepLen;
}

void Player::MoveLeft()
{
	this->xPos -= this->stepLen;
}

void Player::MoveRight()
{
	this->xPos += this->stepLen;
}

BOOL Game_Init(HWND hwnd)
{
	//Initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
	{
		MessageBox(hwnd, "Error initializing Direct3D!", "Error", MB_OK);
		return FALSE;
	}

	//Set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;										//Not in the full screen
	d3dpp.EnableAutoDepthStencil = TRUE;						//Enable the Z buffer
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					//Set the D16 format of the Z buffer in GPU
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;						//Set the swap chain 
	d3dpp.BackBufferCount = 1;									//Set the number of the back buffer
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;					//Set the color format
	d3dpp.BackBufferWidth = SCREENW;							//Set width of the buffer
	d3dpp.BackBufferHeight = SCREENH;							//Set height of the buffer
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	//Set presentation interval
	d3dpp.hDeviceWindow = hwnd;									//Bind the client window

	//Create Direct3D devices
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,						//Default rendering adapter
		D3DDEVTYPE_HAL,							//Default type
		hwnd,									//Client window
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,	//Enable vertex processing
		&d3dpp,									//DX parameters
		&d3ddev									//DX device
	);

	//Check null
	if (!d3ddev)
	{
		MessageBox(hwnd, "Error creating Direct3D device", "Error", MB_OK);
		return FALSE;
	}

	//Clear the backbuffer to white
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//Enable z buffer state
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);

	//Start the initializing work
	long hResult = 0;

	//Initialize the player rectangle
	hResult = d3ddev->CreateOffscreenPlainSurface(
		IMAGEW,				//Surface width
		IMAGEH,				//Surface height
		D3DFMT_X8R8G8B8,	//Color format
		D3DPOOL_DEFAULT,	//Image pool
		&surface,
		NULL
	);	

	if (hResult != D3D_OK) return FALSE;

	//Check Map loading
	if (!LoadMap(hwnd, &mapInfoStruct)) return FALSE;

	//Get client rect to elongate the buffer
	GetClientRect(hwnd, &clientRect);
	clientRect.bottom += EDGE_BUFFER_LEN;
	clientRect.top -= EDGE_BUFFER_LEN;
	clientRect.right += EDGE_BUFFER_LEN;
	clientRect.left -= EDGE_BUFFER_LEN;

	return TRUE;
}

VOID Game_Run(HWND hwnd)
{
	//Check Direct3D device is valid all the time
	if (!d3ddev) return;
	//Create pointer to the backbuffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	d3ddev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	//Start rendering
	DWORD i;
	DWORD j;
	DWORD t;
	if (d3ddev->BeginScene())
	{
		for (i = 0, t = 0; i < mapInfoStruct.rowNum; i++)
		{
			for (j = 0; j < mapInfoStruct.colNum; j++)
			{
				paintRect.left = j * IMAGEW + player->xPos;
				paintRect.top = i * IMAGEH + player->yPos;
				paintRect.right = (j + 1) * IMAGEW + player->xPos;
				paintRect.bottom = (i + 1) * IMAGEH + player->yPos;
				//Draw the surface to the backbuffer
				d3ddev->StretchRect(mapInfoStruct.mapSurface[mapInfoStruct.mapIndicatorArr[t++] - 1], NULL, backbuffer, &paintRect, D3DTEXF_NONE);
			}
		}

		RECT playerRect;
		playerRect.left = player->xPos;
		playerRect.right = player->xPos + IMAGEW;
		playerRect.top = player->yPos;
		playerRect.bottom = player->yPos + IMAGEH;

		//Draw the surface to the backbuffer
		d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(0, 0, 0));
		d3ddev->StretchRect(surface, NULL, backbuffer, &playerRect, D3DTEXF_NONE);

		//Stop rendering
		d3ddev->EndScene();

		//Elongate the buffer in the clientRect
		d3ddev->Present(NULL, &clientRect, NULL, NULL);
	}
}

VOID Game_End(HWND hwnd)
{
	if (surface) surface->Release();
	if (mapInfoStruct.mapSurface)
	{
		DWORD i;
		for (i = 0; i < MAPTYPE; i++)
		{
			mapInfoStruct.mapSurface[i]->Release();
		}
		delete[] mapInfoStruct.mapSurface;
		free(mapInfoStruct.mem_file_buffer);
		mapInfoStruct.mem_file_buffer = NULL;
		mapInfoStruct.mapIndicatorArr = NULL;
		mapInfoStruct.rowNum = 0;
		mapInfoStruct.colNum = 0;
	}
	if (d3ddev)d3ddev->Release();
}

BOOL LoadMap(HWND hwnd, MapInfo *mapInfo)
{
	FILE *pFile = NULL;
	PBYTE mapInfoMem = NULL;
	PDWORD mapDataOffset = NULL;
	DWORD fileSize = 0;

	DWORD map_row_num = 0;
	DWORD map_col_num = 0;

	DWORD i;
	DWORD j;

	fopen_s(&pFile, "TestMap1.klmp", "rb");
	if (!pFile)
	{
		MessageBox(hwnd, "Unable to load the file!", "Error", MB_OK);
		return FALSE;
	}
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	rewind(pFile);

	mapInfoMem = (PBYTE)malloc(fileSize);
	if (!mapInfoMem)
	{
		MessageBox(hwnd, "Unable to assign the space!", "Error", MB_OK);
		fclose(pFile);
		return FALSE;
	}
	ZeroMemory(mapInfoMem, fileSize);

	fread_s(mapInfoMem, fileSize, fileSize, 1, pFile);
	fclose(pFile);

	map_row_num = *((DWORD*)((PBYTE)mapInfoMem + 0x8));
	map_col_num = *((DWORD*)((PBYTE)mapInfoMem + 0xC));

	mapDataOffset = (DWORD*)((PBYTE)mapInfoMem + 0x1C);

	//Initialize the maps
	mapInfo->mapSurface = new LPDIRECT3DSURFACE9[MAPTYPE];

	CHAR imagePath[36] = { 0 };
	long hResult = 0;
	for (i = 0; i < MAPTYPE; i++)
	{
		ZeroMemory(imagePath, 36);
		//Create surface for the images
		hResult = d3ddev->CreateOffscreenPlainSurface(
			IMAGEW, 
			IMAGEH,
			D3DFMT_X8R8G8B8, 
			D3DPOOL_DEFAULT,
			&(mapInfo->mapSurface[i]), 
			NULL
		);
		if (hResult != D3D_OK)
		{
			free(mapInfoMem);
			delete[] mapInfo->mapSurface;
			return FALSE;
		}
		//Load surface from file into newly created surface
		sprintf_s(imagePath, "Image%u.png", i + 1);
		hResult = D3DXLoadSurfaceFromFile(
			(mapInfo->mapSurface)[i],
			NULL,
			NULL,
			imagePath,
			NULL,
			D3DX_DEFAULT,
			NULL,
			NULL
		);
		//Check NULL
		if (hResult != D3D_OK)
		{
			free(mapInfoMem);
			delete[] mapInfo->mapSurface;
			return FALSE;
		}
	}
	mapInfo->mem_file_buffer = mapInfoMem;
	mapInfo->rowNum = map_row_num;
	mapInfo->colNum = map_col_num;
	mapInfo->mapIndicatorArr = (PDWORD)(0x40 + (PBYTE)mapInfoMem);
	return TRUE;
}

DWORD __stdcall RenderingPlayerFunc(LPVOID lpParameter)
{
	while (!gameover)
	{
		Game_Run(*((HWND*)lpParameter));
	}
	return 0;
}

long __stdcall WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		gameover = TRUE;
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
			player->MoveUp();
			break;
		case 'S':
			player->MoveDown();
			break;
		case 'A':
			player->MoveLeft();
			break;
		case 'D':
			player->MoveRight();
			break;
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WinProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "MapLoading";
	wndClass.hIconSm = NULL;

	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL, "Unable to initiate a window class!", "Error", MB_OK);
		return 0;
	}

	//Create a new window
	HWND window = CreateWindow(
		"MapLoading",
		APPTITLE.c_str(),
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREENW,
		SCREENH,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//Was there an error creating the window
	if (!window) return 0;

	//Display the window
	MoveWindow(window, 150, 50, SCREENW, SCREENH, TRUE);
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	//Initialize the game
	if (!Game_Init(window)) return 0;

	//Create the thread in the suspended state
	hPaintingPlayerThread = CreateThread(NULL, NULL, RenderingPlayerFunc, &window, CREATE_SUSPENDED, NULL);

	Sleep(100);
	//Resume the thread
	ResumeThread(hPaintingPlayerThread);

	//Main message loop
	MSG message = { 0 };
	while (!gameover)
	{
		if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		Sleep(2);
	}
	Game_End(window);
	UnregisterClass("MapLoading", hInstance);
	return message.wParam;
}