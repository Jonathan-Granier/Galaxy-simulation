#include "Camera.h"

void Camera::UpdateViewMatrix()
{
    glm::mat4 rotM = glm::mat4(1.0f);
    glm::mat4 transM;

    rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 translation = position;
    if (flipY)
    {
        translation.y *= -1.0f;
    }
    transM = glm::translate(glm::mat4(1.0f), translation);
    view = transM * rotM;
    viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

    updated = true;
};

void Camera::SetPerspective(float iFov, float iAspect, float iZnear, float iZfar)
{
    this->fov = iFov;
    this->znear = iZnear;
    this->zfar = iZfar;
    perspective = glm::perspective(glm::radians(fov), iAspect, znear, zfar);
    if (flipY)
    {
        perspective[1][1] *= -1.0f;
    }
};

void Camera::UpdateAspectRatio(float iAspect)
{
    perspective = glm::perspective(glm::radians(fov), iAspect, znear, zfar);
    if (flipY)
    {
        perspective[1][1] *= -1.0f;
    }
}