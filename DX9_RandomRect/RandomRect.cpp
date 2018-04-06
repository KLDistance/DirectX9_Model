#include "RandomRect.h"

//Game initialization function
BOOL Game_Init(HWND hwnd)
{
	//Initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
	{
		MessageBox(hwnd, "Error initialize Direct3D", "Error", MB_OK);
		return FALSE;
	}

	//Set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = SCREENW;
	d3dpp.BackBufferHeight = SCREENH;
	d3dpp.hDeviceWindow = hwnd;

	//Create Direct3D device
	d3d->CreateDevice(	D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
		&d3dpp, &d3ddev
	);
	if (!d3ddev)
	{
		MessageBox(hwnd, "Error creating Direct3D device", "Error", MB_OK);
		return FALSE;
	}

	//Set the random seed
	srand((DWORD)time(NULL));

	//Clear the backbuffer to white
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//Create pointer to the back buffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//Create surface
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		100, 
		100,
		D3DFMT_X8R8G8B8, 
		D3DPOOL_DEFAULT,
		&surface,
		NULL
	);
	if (result != D3D_OK) return FALSE;
	return TRUE;
}

//Game update function
VOID Game_Run(HWND hwnd)
{
	//Make sure the DIrect3D device is valid
	if (!d3ddev) return;

	//Start rendering
	if (d3ddev->BeginScene())
	{
		//Fill the surface with random color
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;

		d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(r, g, b));

		//Copy the surface to the backbuffer
		RECT rect;
		rect.left = rand() % SCREENW / 2;
		rect.right = rect.left + rand() % SCREENW / 2;
		rect.top = rand() % SCREENH / 2;
		rect.bottom = rect.top + rand() % SCREENH / 2;
		d3ddev->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE);

		//Stop rendering
		d3ddev->EndScene();

		//Copy the backbuffer to the screen
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
	//Check for escape key (to exit)
	if (KEY_DOWN(VK_ESCAPE)) PostMessage(hwnd, WM_DESTROY, 0, 0);
	Sleep(30);
}

//Game shutdown function
VOID Game_End(HWND hwnd)
{
	if (surface) surface->Release();
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
}

//Windows event callback function
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		gameover = TRUE;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Windows entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPresInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Create the window class structure
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MainWindowClass";
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	//Create a new window
	HWND window = CreateWindow("MainWindowClass", APPTITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		SCREENW, SCREENH, NULL, NULL, hInstance, NULL
	);

	//Was there an error creating the window?
	if (!window) return 0;

	//Display the window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	//Initialize the game
	if (!Game_Init(window)) return 0;

	//Main message loop
	MSG message = { 0 };
	while (!gameover)
	{
		if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		Game_Run(window);
	}
	return message.wParam;
}