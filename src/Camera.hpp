#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Input.hpp"

class Camera {

public:
  Camera();
  Camera(glm::vec3 position, glm::vec3 pointCible, glm::vec3 axeVertical,
	 float sensibilite, float vitesse);
  ~Camera();

  virtual void deplacer(Input const &input) = 0;

  virtual void lookAt(glm::mat4 &modelview) = 0;

  virtual void setPointcible(glm::vec3 pointCible) = 0;
  virtual void setPosition(glm::vec3 position) = 0;

  float getSensibilite() const;
  float getVitesse() const;
  void setSensibilite(float sensibilite);
  void setVitesse(float vitesse);

protected:
  glm::vec3 m_orientation;
  glm::vec3 m_axeVertical;
  glm::vec3 m_deplacementLateral;
    
  glm::vec3 m_position;
  glm::vec3 m_pointCible;

  float m_sensibilite;
  float m_vitesse;
};


#endif
