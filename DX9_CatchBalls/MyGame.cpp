#include "DXWindowImport.h"

const string APPTITLE = "Bomb Catcher Game";
const int SCREENW = 1024;
const int SCREENH = 1024;

LPDIRECT3DSURFACE9 bomb_surf = NULL;
LPDIRECT3DSURFACE9 bucket_surf = NULL;

struct BOMB
{
	float x, y;
	VOID reset()
	{
		x = ((float)(rand() % (SCREENW - 250)));
		y = 0;
	}
};

BOMB bomb;

struct BUCKET
{
	float x, y;
};

BUCKET bucket;
int score = 0;
int vibrating = 0;

BOOL Game_Init(HWND window)
{
	Direct3D_Init(window, SCREENW, SCREENH, FALSE);
	DirectInput_Init(window);
	bomb_surf = LoadSurface("BallPic.bmp");
	if (!bomb_surf)
	{
		MessageBox(window, "Error loading bomb", "Error", MB_OK);
		return FALSE;
	}
	bucket_surf = LoadSurface("BucketPic.bmp");
	if (!bucket_surf)
	{
		MessageBox(window, "Error loading bomb", "Error", MB_OK);
		return FALSE;
	}

	//Get the back buffer surface
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//Position the bomb
	srand((DWORD)time(NULL));
	bomb.reset();

	//Positioning the bucket
	bucket.x = 500;
	bucket.y = 780;
	return TRUE;
}

VOID Game_Run(HWND window)
{
	//Make sure the Direct3D device is valid
	if (!d3ddev) return;

	//Update input devices
	DirectInput_Update();

	//Move the bomb down the screen
	bomb.y += 2.0f;

	//See if bomb hit the floor
	if (bomb.y + 240 > SCREENH)
	{
		MessageBox(0, "Oh no, the bomb exploded!!", "YOU STINK", 0);
		gameover = TRUE;
	}
	/*
	//Move the bucket with the mouse
	int mx = Mouse_X();
	if (mx < 0) bucket.x -= 6.0f;
	else if (mx > 0) bucket.x += 6.0f;
	*/
	//Move the bucket with the keyboard
	if (Key_Down(DIK_LEFT)) bucket.x -= 6.0f;
	else if (Key_Down(DIK_RIGHT)) bucket.x += 6.0f;

	//Move the bucket with the controller
	if (XInput_Controller_Found())
	{
		//Left analog thumb stick
		if (controllers[0].sThumbLX < -5000) bucket.x -= 6.0f;
		else if (controllers[0].sThumbLX > 5000) bucket.x += 6.0f;

		//Left and right triggers
		if (controllers[0].bLeftTrigger < -128) bucket.x -= 6.0f;
		else if (controllers[0].bRightTrigger > 128) bucket.x += 6.0f;

		//Left and right D-PAD
		if (controllers[0].wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) bucket.x + 6.0f;
		else if (controllers[0].wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) bucket.x -= 6.0f;
		
		//Left and right shoulders
		if (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) bucket.x += 6.0f;
		else if (controllers[0].wButtons & XINPUT_GAMEPAD_DPAD_LEFT) bucket.x -= 6.0f;
	}

	//Update vibration
	if (vibrating > 0)
	{
		vibrating++;
		if (vibrating > 20)
		{
			XInput_Vibrate(0, 0);
			vibrating = 0;
		}
	}

	//Keep bucket inside the screen
	if (bucket.x < 0) bucket.x = 0;
	if (bucket.x > SCREENW - 300) bucket.x = SCREENW - 300;
	
	//See if bucket caught the bomb
	int cx = bomb.x + 64;
	int cy = bomb.y + 64;
	if (cx > bucket.x && cx < bucket.x + 200 &&
		cy > bucket.y && cy < bucket.y + 200)
	{
		//Update and display score
		score++;
		ostringstream os;
		os << APPTITLE << " [SCORE " << score << "]";
		string scoreStr = os.str();
		SetWindowText(window, scoreStr.c_str());

		//Vibrate the controller
		XInput_Vibrate(0, 50000);
		vibrating = 1;

		//Restart bomb
		bomb.reset();
	}
	
	//Clear the backbuffer
	d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(255, 255, 255));

	//Start rendering
	if (d3ddev->BeginScene())
	{
		//Draw the bomb
		DrawSurface(backbuffer, bomb.x, bomb.y, bomb_surf);

		//Draw the bucket
		DrawSurface(backbuffer, bucket.x, bucket.y, bucket_surf);

		//Stop rendering
		d3ddev->EndScene();
		d3ddev->Present(0, 0, 0, 0);
	}

	//Escape key exits
	if (Key_Down(DIK_SPACE) || Key_Down(DIK_ESCAPE))
	{
		gameover = TRUE;
	}

	//Controller back button also exits
	if (controllers[0].wButtons & XINPUT_GAMEPAD_B)
	{
		gameover = TRUE;
	}
}

VOID Game_End()
{
	if (bomb_surf) bomb_surf->Release();
	if (bucket_surf) bucket_surf->Release();
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}

