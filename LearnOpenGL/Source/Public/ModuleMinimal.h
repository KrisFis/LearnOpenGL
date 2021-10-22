
#pragma once

// ASTD
#include "ASTD.h"

// Core
#include "RenderTypes.h"

// STL
#include <assert.h>
#include <iostream>
#include <memory>

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
class FShaderProgram;
class FTexture;
class FCamera;
class FMesh;
class FModel;
class FScene;
class FFramebuffer;
class FRenderTexture;
class FRenderBuffer;