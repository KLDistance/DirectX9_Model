#include "PaintMain.h"

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
	d3dpp.Windowed = TRUE;

	//The following two are essential in repainting
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREENW;
	d3dpp.BackBufferHeight = SCREENH;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.hDeviceWindow = hwnd;

	//Create Direct3D devices
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &d3ddev
	);
	if (!d3ddev)
	{
		MessageBox(hwnd, "Error creating Direct3D device", "Error", MB_OK);
		return FALSE;
	}

	//Clear the backbuffer to white 
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(240, 240, 255), 1.0f, 0);
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	//Enable zbuffer state
	d3ddev->SetRenderState(D3DRS_ZENABLE, true);

	//Start the initialize work
	long hResult = 0;
	
	//Initialize the player rectangle
	hResult = d3ddev->CreateOffscreenPlainSurface(
		IMAGEW,
		IMAGEH,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&surface,
		NULL
	);

	if (hResult != D3D_OK)
	{
		return FALSE;
	}
	
	//Initialize the maps
	mapSurface = new LPDIRECT3DSURFACE9[MAPTYPE];

	DWORD i;
	CHAR imagePath[36] = { 0 };
	for (i = 0; i < MAPTYPE; i++)
	{
		ZeroMemory(imagePath, 36);
		//Create surface for the images
		hResult = d3ddev->CreateOffscreenPlainSurface(
			IMAGEW,
			IMAGEH,
			D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT,
			&(mapSurface[i]),
			NULL
		);
		if (hResult != D3D_OK)
		{
			delete[] mapSurface;
			return FALSE;
		}
		//Load surface from file into newly created surface
		sprintf_s(imagePath, "Image%u.png", i + 1);
		hResult = D3DXLoadSurfaceFromFile(
			mapSurface[i],
			NULL,
			NULL,
			imagePath,
			NULL,
			D3DX_DEFAULT,
			0,
			NULL
		);
		//Make sure the file is correctly loaded
		if (hResult != D3D_OK)
		{
			delete[] mapSurface;
			return FALSE;
		}
	}
	return TRUE;
}

VOID Game_Run(HWND hwnd)
{
	//Make sure the Direct3D device is valid all the time
	if (!d3ddev) return;
	//Create pointer to the backbuffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(240, 240, 255), 1.0f, 0);
	//Start rendering
	DWORD i;
	DWORD j;
	if (d3ddev->BeginScene())
	{
		RECT paintRect;
		for (i = 0; i < ROWNUM; i++)
		{
			for (j = 0; j < COLNUM; j++)
			{
				paintRect.left = j * IMAGEW + player->xPos;
				paintRect.top = i * IMAGEH + player->yPos;
				paintRect.right = (j + 1) * IMAGEW + player->xPos;
				paintRect.bottom = (i + 1) * IMAGEH + player->yPos;
				//Draw the surface to the backbuffer
				d3ddev->StretchRect(mapSurface[mapInfo[i * ROWNUM + j] - 1], NULL, backbuffer, &paintRect, D3DTEXF_LINEAR);
			}
		}
		
		RECT rect;
		rect.left = player->xPos;
		rect.right = player->xPos + IMAGEW;
		rect.top = player->yPos;
		rect.bottom = player->yPos + IMAGEH;
		//Draw the surface to the backbuffer
		d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(0, 0, 0));
		d3ddev->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_LINEAR);

		//Stop rendering
		d3ddev->EndScene();

		//Elongate the buffer in the clientRect
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		clientRect.bottom += EDGE_BUFFER_LEN;
		clientRect.top -= EDGE_BUFFER_LEN;
		clientRect.right += EDGE_BUFFER_LEN;
		clientRect.left -= EDGE_BUFFER_LEN;

		//Copy the backbuffer to the screen
		d3ddev->Present(NULL, &clientRect, NULL, NULL);
	}
	if (KEY_DOWN(VK_ESCAPE)) PostMessage(hwnd, WM_DESTROY, 0, 0);
	Sleep(5);
}

VOID Game_End(HWND hwnd)
{
	if (surface) surface->Release();
	if (mapSurface)
	{
		DWORD i;
		for (i = 0; i < MAPTYPE; i++)
		{
			mapSurface[i]->Release();
		}
		delete[] mapSurface;
	}
	if (d3ddev) d3ddev->Release();
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
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "ObstacleTest";
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	//Create a new window
	HWND window = CreateWindow(
		"ObstacleTest",
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
	hPaintingPlayerThread = ::CreateThread(NULL, NULL, RenderingPlayerFunc, &window, CREATE_SUSPENDED, NULL);

	Sleep(30);
	//Resume the thread
	::ResumeThread(hPaintingPlayerThread);

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
	return message.wParam;
}
