#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include "Particule.hpp"

class Obstacle : public Object {

private :
  FLOAT friction;
  
public :
  Obstacle(Shader *shader = NULL);
  virtual void animate() = 0;
  virtual void draw(glm::mat4 m = glm::mat4(1.0f), Shader *s = NULL) = 0;
  
  virtual FLOAT distance(VEC3 v) const = 0;
  virtual VEC3 getNormal(VEC3 v) const = 0;
  virtual void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const = 0;

  FLOAT getFriction() const;
  void setFriction(FLOAT f);
};

#endif
