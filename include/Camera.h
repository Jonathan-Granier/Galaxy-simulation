#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    const glm::mat4 &GetViewMatrix() const { return view; }
    const glm::mat4 &GetPerspectiveMatrix() const { return perspective; }
    float GetRotationSpeed() const { return rotationSpeed; }
    float GetNearClip() { return znear; }
    float GetFarClip() { return zfar; }

    void SetPerspective(float iFov, float iAspect, float iZnear, float iZfar);

    void UpdateAspectRatio(float iAspect);

    void SetPosition(glm::vec3 iPosition)
    {
        position = iPosition;
        UpdateViewMatrix();
    }

    void SetRotation(glm::vec3 iRotation)
    {
        rotation = iRotation;
        UpdateViewMatrix();
    }

    void Rotate(glm::vec3 iDelta)
    {
        rotation += iDelta;
        UpdateViewMatrix();
    }

    void SetTranslation(glm::vec3 iTranslation)
    {
        position = iTranslation;
        UpdateViewMatrix();
    };

    void Translate(glm::vec3 iDelta)
    {
        position += iDelta;
        UpdateViewMatrix();
    }

    void SetRotationSpeed(float iRotationSpeed)
    {
        rotationSpeed = iRotationSpeed;
    }

    void SetMovementSpeed(float iMovementSpeed)
    {
        movementSpeed = iMovementSpeed;
    }

private:
    void UpdateViewMatrix();

    float fov;
    float znear, zfar;

    glm::vec3 rotation = glm::vec3();
    glm::vec3 position = glm::vec3();
    glm::vec4 viewPos = glm::vec4();

    float rotationSpeed = 1.0f;
    float movementSpeed = 1.0f;

    bool updated = false;
    bool flipY = false;

    glm::mat4 perspective;
    glm::mat4 view;
};