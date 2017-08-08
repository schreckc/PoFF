#ifndef PLANE_OBSTACLE_HPP
#define PLANE_OBSTACLE_HPP

#include "Obstacle.hpp"

class PlaneObstacle : public Obstacle {
private :
  VEC3 pos;
  VEC3 normal;
  FLOAT length, width;
  
public :
  PlaneObstacle(VEC3 p = VEC3(0, 0, 0), VEC3 n = VEC3(0, 0, 1), FLOAT l = 0, FLOAT w = 0, int shader = -1);
  ~PlaneObstacle();

  //  void animate();
  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);
#endif
  
  VEC3 getPosition() const;
  VEC3 getNormal(VEC3 pos) const;
  VEC3 getNormal() const;
  
  FLOAT distance(VEC3 v) const;
  void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &n) const;

  void rotate(FLOAT angle, VEC3 axe);
};

#endif
 
