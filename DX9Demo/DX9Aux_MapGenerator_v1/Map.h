#pragma once

#include <Windows.h>
#include <stdio.h>
#include <string.h>

typedef DWORD element_size;

const int mapRow = 25;
const int mapCol = 50;

VOID MapWrite(IN const char *fileName);
