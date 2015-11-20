#pragma once

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <Util/Log.h>

#ifdef _WIN32
#	ifdef NDEBUG
#		pragma comment(lib, "glew32s.lib")
#	else
#		pragma comment(lib, "glew32sd.lib")
#	endif
#endif