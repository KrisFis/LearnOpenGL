
#pragma once

#include "TypeTraitsCore.h"
#include <glm/glm.hpp>

// Added glm vector and matrix to primitives
template<> struct TIsPrimitiveType<glm::vec2> { enum { Value = true }; };
template<> struct TIsPrimitiveType<glm::vec3> { enum { Value = true }; };
template<> struct TIsPrimitiveType<glm::vec4> { enum { Value = true }; };

template<> struct TIsPrimitiveType<glm::mat2> { enum { Value = true }; };
template<> struct TIsPrimitiveType<glm::mat3> { enum { Value = true }; };
template<> struct TIsPrimitiveType<glm::mat4> { enum { Value = true }; };
