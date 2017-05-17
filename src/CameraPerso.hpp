#ifndef NO_GRAPHICS_ 

#ifndef CAMERA_PERSO_HPP
#define CAMERA_PERSO_HPP

#include "Camera.hpp"

class CameraPerso : public Camera {

public:
  CameraPerso();
  CameraPerso(glm::vec3 position, glm::vec3 pointCible, glm::vec3 axeVertical,
	      float sensibilite = 0.5, float vitesse = 0.5);
  ~CameraPerso();

  void orienter(int xRel, int yRel);
  void deplacer(Input const &input);

  void lookAt(glm::mat4 &modelview);

  void setPointcible(glm::vec3 pointCible);
  void setPosition(glm::vec3 position);
private:

  float m_phi;
  float m_theta;
};


#endif

#endif
