#ifndef CAMERA_OBJECT_HPP
#define CAMERA_OBJECT_HPP

#include "Camera.hpp"

class CameraObject : public Camera {

public:
  CameraObject();
  CameraObject(glm::vec3 position, glm::vec3 pointCible, glm::vec3 axeVertical,
	      float sensibilite = 0.01, float vitesse = 0.02);
  ~CameraObject();

  void orienter(int xRel, int yRel);
  void orienterPerso(int xRel, int yRel);
  void deplacer(Input const &input);

  void lookAt(glm::mat4 &modelview);

  void setPointcible(glm::vec3 pointCible);
  void setPosition(glm::vec3 position);

  void alignAxis();

private:
  float m_phi;
  float m_theta;
  
};


#endif
