
#pragma once

// Core
#include "IntegerTypes.h"
#include "RenderTypes.h"

// STL
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <string>
#include <iostream>

// GLAD + GLFW
#include <glad/glad.h>
#include <glfw/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Forward declaration
class FShader;
class FTexture;
class FCamera;
class FMesh;