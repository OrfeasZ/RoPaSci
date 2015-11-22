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
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Util/Log.h>

#ifdef _WIN32
#	pragma comment(lib, "opengl32.lib")

#	ifdef NDEBUG
#		pragma comment(lib, "glfw3.lib")
#		pragma comment(lib, "freetype.lib")
#		pragma comment(lib, "glew32s.lib")
#	else
#		pragma comment(lib, "glfw3d.lib")
#		pragma comment(lib, "freetyped.lib")
#		pragma comment(lib, "glew32sd.lib")
#	endif
#endif