#ifndef MY_CAMERA_H
#define MY_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    glm::vec3 cameraPos ;
    glm::vec3 target ;
    glm::vec3 cameraUp ;
    glm::vec3 cameraFront ;
    glm::vec3 cameraRight ;
    glm::vec3 worldUp ;
    float theta;
    float phi;
    float speed;
    float roateSpeed;
    float radius;
   public:
Camera(){
   
}


Camera(glm::vec3 cameraPosV, glm::vec3 targetV, glm::vec3 cameraUpV);


   void processRotation(float xoffset, float yoffset);
   void processRadiusUpdate(float dir);
   void updateCoords();
   void updateUp();
   void updateCameraPos();
   glm::mat4 getViewMat();
   glm::vec3 getCameraPos();
};


#endif
