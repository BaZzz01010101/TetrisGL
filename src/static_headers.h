#pragma once

#pragma warning(push, 3)
#pragma warning (disable : 4005) // 'APIENTRY': macro redefinition.
#pragma warning(disable : 4996)  // 'fopen': This function or variable may be unsafe.

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <memory>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "ft2build.h"
#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "FMOD/fmod.hpp"
#include FT_FREETYPE_H
#include "stb_image_write.h"
#include "stb_image.h"

#pragma warning(pop)

static const float VERY_SMALL_NUMBER = sqrtf(FLT_MIN);
static const float M_SQRT2 = 1.4142135623730950488016887242097f;
static const float M_PI = 3.1415926535897932384626433832795f;
static const float M_2_PI = 2.0f * M_PI;
static const float M_PI_2 = 0.5f * M_PI;
