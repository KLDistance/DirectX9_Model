#include "DXWindowImport.h"

//Direct3D variables
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;

//DirectInput variables
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];
XINPUT_GAMEPAD controllers[4];

//Direct3D initialization

BOOL Direct3D_Init(HWND window, int width, int height, BOOL fullscreen)
{
	//Initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) return FALSE;

	//Set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = (!fullscreen);
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferWidth = height;
	d3dpp.hDeviceWindow = window;

	//Create Direct3D device
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev
	);
	if (!d3ddev) return FALSE;

	//Get a pointer to the back buffer surface
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	return TRUE;
}

//Direct3D shutdown
VOID Direct3D_Shutdown()
{
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
}

//Draws a surface to the screen using StretchRect
VOID DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
	//Get width / height from source surface
	D3DSURFACE_DESC desc;
	source->GetDesc(&desc);

	//Create rects for drawing
	RECT source_rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };
	RECT dest_rect = { (LONG)x, (LONG)y, (LONG)x + desc.Width, (LONG)y + desc.Height };
	
	//Draw the source surface onto the dest
	d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);
}

//Loads a bitmap file into a surface
LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
	LPDIRECT3DSURFACE9 image = NULL;

	//Get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	//Create surface
	result = d3ddev->CreateOffscreenPlainSurface(
		info.Width,					//Width of the surface
		info.Height,				//Height of the surface
		D3DFMT_X8R8G8B8,			//Surface format
		D3DPOOL_DEFAULT,			//Memory pool to use
		&image,						//Pointer to the surface
		NULL						//Reserved parameter
	);
	if (result != D3D_OK) return NULL;

	//Load surface from file into newly created surface
	result = D3DXLoadSurfaceFromFile(
		image,						//Destination surface
		NULL,						//Destination palette
		NULL,						//Destination rectangle
		filename.c_str(),				//Source filename
		NULL,						//Source rectangle
		D3DX_DEFAULT,				//Controls how image is filtered
		D3DCOLOR_XRGB(0, 0, 0),		//For ransparency (0 for none)
		NULL						//Source image info: NULL
	);
	
	//Make sure file was loaded okay
	if (result != D3D_OK) return NULL;
	return image;
}

//DirectInput initialization
BOOL DirectInput_Init(HWND hwnd)
{
	//Initialize DirectInput object
	HRESULT result = DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(LPVOID*)&dinput,
		NULL
	);
	//Initialize the keyboard
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();

	//Initialize the mouse
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	d3ddev->ShowCursor(FALSE);
	return TRUE;
}

//DirectInput update
VOID DirectInput_Update()
{
	//Update mouse
	dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);
	
	//Update keyboard
	dikeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);

	//Update controllers
	for (int i = 0; i < 4; i++)
	{
		ZeroMemory(&controllers[i], sizeof(XINPUT_STATE));

		//Get the state of the controller
		XINPUT_STATE state;
		DWORD result = XInputGetState(i, &state);

		//Store state in global controllers array
		if (!result) controllers[i] = state.Gamepad;
	}
}

//Return mouse x movement
int Mouse_X()
{
	return mouse_state.lX;
}

//Return mouse y movement
int Mouse_Y()
{
	return mouse_state.lY;
}

//Return mouse button state
int Mouse_Button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}

//Return key press state
int Key_Down(int key)
{
	return (keys[key] & 0x80);
}

//DirectInput shutdown
VOID DirectInput_Shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}
	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
}

//Returns true if controller is plugged in
BOOL XInput_Controller_Found()
{
	XINPUT_CAPABILITIES caps;
	ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
	XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps);
	if (caps.Type != 0) return FALSE;
	return TRUE;
}

//Vibrates the controller
VOID XInput_Vibrate(int contNum, int amount)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(vibration));
	vibration.wLeftMotorSpeed = amount;
	vibration.wRightMotorSpeed = amount;
	XInputSetState(contNum, &vibration);
}