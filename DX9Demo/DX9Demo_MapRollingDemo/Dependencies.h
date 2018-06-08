#pragma once

//Include the system header files
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <list>

//Include the DirectX auxiliary files
#include "..//..//Dependencies//Include//d3d9.h"
#include "..//..//Dependencies//Include//d3dx9.h"
#include "..//..//Dependencies//Include//dinput.h"
#include "..//..//Dependencies//Include//XInput.h"

//Load the system static-linked libraries
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

//Load the DirectX auxiliary statc-linked libraries
#pragma comment(lib, "..//..//Dependencies//Lib//x86//dxguid.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//d3d9.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//d3dx9.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//dinput8.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//xinput.lib")

using namespace std;
