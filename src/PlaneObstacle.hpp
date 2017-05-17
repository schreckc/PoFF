#ifndef PLANE_OBSTACLE_HPP
#define PLANE_OBSTACLE_HPP

#include "Obstacle.hpp"

class PlaneObstacle : public Obstacle {
private :
  FLOAT pos;
  VEC3 normal;

public :
  PlaneObstacle(FLOAT p = 0, VEC3 n = VEC3(0, 0, 1), int shader = -1);
  ~PlaneObstacle();

  void animate();
#ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);
#endif
  
  FLOAT getPosition() const;
  VEC3 getNormal(VEC3 pos) const;
  VEC3 getNormal() const;
  
  FLOAT distance(VEC3 v) const;
  void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &n) const;
};

#endif
 
