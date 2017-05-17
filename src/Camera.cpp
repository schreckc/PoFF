#ifndef NO_GRAPHICS_ 

#include "Camera.hpp"

Camera::Camera() :  m_orientation(), m_axeVertical(0, 0, 1),
			      m_deplacementLateral(), m_position(), m_pointCible(),
			      m_sensibilite(0.0), m_vitesse(0.0) {

}

 Camera::Camera(glm::vec3 position, glm::vec3 pointCible, glm::vec3 axeVertical,
			     float sensibilite, float vitesse) :
   m_axeVertical(axeVertical), 
  m_deplacementLateral(), m_position(position), m_pointCible(pointCible),
  m_sensibilite(sensibilite), m_vitesse(vitesse) {
   m_orientation = m_pointCible - m_position;
   m_orientation = normalize(m_orientation);
}

Camera::~Camera() {

}


void Camera::setPosition(glm::vec3 position) {
  m_position = position;
  m_pointCible = m_position + m_orientation;
}


float Camera::getSensibilite() const {
  return m_vitesse;
}



float Camera::getVitesse() const {
  return m_vitesse;
}



void Camera::setSensibilite(float sensibilite) {
  m_sensibilite = sensibilite;
}



void Camera::setVitesse(float vitesse) {
  m_vitesse = vitesse;
}

glm::vec3 Camera::getPosition() const {
  return m_position;
}

#endif
