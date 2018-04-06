#include "DXWindowImport.h"

BOOL gameover = FALSE;

//Windows event handler
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

//Windows entry function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Initialize the windows settings
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MainWindowClass";
	wc.hIconSm = NULL;

	//Register class
	RegisterClassEx(&wc);

	//Create a new window
	HWND window = CreateWindow(
		"MainWindowClass", APPTITLE.c_str(), SCREENW, SCREENH, 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL
	);

	//Display the window
	MoveWindow(window, 300, 100, SCREENW, SCREENH, TRUE);
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	//Initialize the game
	if (!Game_Init(window)) return 0;

	//Main message loop
	MSG message = { 0 };
	while (!gameover)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//Process the game message
		Game_Run(window);
	}

	//Shutdown
	Game_End();
	return message.wParam;
}