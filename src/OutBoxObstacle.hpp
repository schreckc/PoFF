#ifndef OUT_BOX_OBSTACLE_HPP
#define OUT_BOX_OBSTACLE_HPP

#include "Obstacle.hpp"

class OutBoxObstacle : public Obstacle {
private :
  VEC3 min_pos;
  VEC3 max_pos;
  FLOAT thickness;
  
public :
  OutBoxObstacle(VEC3 min_p = VEC3(0, 0, 0), VEC3 max_p = VEC3(0, 0, 0), FLOAT thick = 0, int shader = -1);
  ~OutBoxObstacle();

  //  void animate();
  void apply(Motion m);
  #ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1); 
#endif
  
  VEC3 getNormal(VEC3 p) const;
  FLOAT distance(VEC3 v) const;
  VEC3 projection(VEC3 v) const {return v;} 
  void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const;
};

#endif
