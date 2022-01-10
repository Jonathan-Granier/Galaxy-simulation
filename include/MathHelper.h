#pragma once

#include <glm/glm.hpp>
#include <cmath>

static constexpr float PI = 3.141592653589793f;
/// Construct 3 vector from spherical coordiantes.
/// @param iNorm Vector's norm.
/// @param iTheta Theta angle.
/// @param iPhi Phi angle.
/// @return Float vector.
static glm::vec3 Spherical(double iNorm, double iTheta, double iPhi)
{
    return glm::vec3(iNorm * sin(iTheta) * sin(iPhi), iNorm * cos(iPhi), iNorm * cos(iTheta) * sin(iPhi));
}

/// Produce a pseudo random bounded float.
/// @param iMin Minimum limit.
/// @param iMax Maximum limit.
/// @return Pseudo random float between iMin and iMax.
static float RandomFloat(float iMin, float iMax)
{
    return iMin + static_cast<float>(rand()) / (RAND_MAX / (iMax - iMin));
}
