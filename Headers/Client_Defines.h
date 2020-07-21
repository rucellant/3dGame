#pragma once

#include "stdafx.h"

const unsigned int g_iWinCX = 1280;
const unsigned int g_iWinCY = 720;

enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_BOSS, SCENE_END };

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern SCENEID g_eScene;