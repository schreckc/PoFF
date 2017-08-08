#ifndef CYLINDER_OBSTACLE_HPP
#define CYLINDER_OBSTACLE_HPP

#include "Obstacle.hpp"

class CylinderObstacle : public Obstacle {
private :
  VEC3 pos;
  VEC3 dir;
  FLOAT ray;
  
public :
  CylinderObstacle(VEC3 p = VEC3(0, 0, 0), VEC3 d = VEC3(0, 0, 1), FLOAT r = 0, int shader = -1);
  ~CylinderObstacle();

  //  void animate();
  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);
#endif
  
  VEC3 getNormal(VEC3 p) const;
  FLOAT distance(VEC3 v) const;
  void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const;
};

#endif
