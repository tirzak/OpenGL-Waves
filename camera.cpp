
#include "camera.h"
#include <iostream>
#include <algorithm>
Camera::Camera(glm::vec3 cameraPosV, glm::vec3 targetV, glm::vec3 cameraUpV)
{
    cameraPos = cameraPosV;
    target = targetV;
    worldUp = cameraUpV;
    theta = 0;
    phi = 0;
    cameraFront = glm::vec3(0);
    radius = 10;
}

void Camera::processRadiusUpdate(float dir)
{
    radius += dir;
    radius = std::max(0.1f, radius);
    updateCameraPos();
}
void Camera::processRotation(float xoffset, float yoffset)
{
    theta += glm::radians(xoffset);
    // yoffset*=-1;
    phi += glm::radians(yoffset); // Updated this line
    phi=glm::clamp(phi, glm::radians(1.0f), glm::radians(179.0f));
    updateCameraPos();
}
void Camera::updateCameraPos()
{

    // cameraPos.x = target.x + radius * cameraFront.x;
    // cameraPos.y = target.y + radius * cameraFront.y;
    // cameraPos.z = target.z + radius * cameraFront.z;
    float newX = radius * sin(phi) * cos(theta);
    float newY = radius * cos(phi);
    float newZ = radius * sin(phi) * sin(theta);

    glm::vec3 newCameraPos(newX, newY, newZ);
    cameraPos=newCameraPos;
    updateUp();


}
void Camera::updateUp()
{
    // cameraFront.x = sin(phi) * cos(theta);
    // cameraFront.y = cos(phi);
    // cameraFront.z = sin(phi) * sin(theta);
    cameraFront = glm::normalize(cameraPos-target);
    glm::vec3 right = glm::normalize(glm::cross(worldUp,cameraFront));
    cameraUp = glm::normalize(glm::cross(cameraFront,right));
}
void Camera::updateCoords()
{
    float xVal = pow(cameraPos.x, 2);
    float yVal = pow(cameraPos.y, 2);
    float zVal = pow(cameraPos.z, 2);
    radius = sqrt(xVal + yVal + zVal);
    theta = atan2(cameraPos.z, cameraPos.x);
    phi = acos(cameraPos.y / radius);
    updateUp();
}
glm::vec3 Camera::getCameraPos()
{
    return cameraPos;
}
glm::mat4 Camera::getViewMat()
{
    return glm::lookAt(cameraPos, target, cameraUp);
}