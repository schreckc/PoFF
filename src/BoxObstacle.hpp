#ifndef BOX_OBSTACLE_HPP
#define BOX_OBSTACLE_HPP

#include "Obstacle.hpp"

class BoxObstacle : public Obstacle {
private :
  VEC3 min_pos;
  VEC3 max_pos;
  
public :
  BoxObstacle(VEC3 min_p = VEC3(0, 0, 0), VEC3 max_p = VEC3(0, 0, 0), int shader = -1);
  ~BoxObstacle();

  //  void animate();
  void apply(Motion m);
  #ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1); 
#endif
  
  VEC3 getNormal(VEC3 p) const;
  FLOAT distance(VEC3 v) const;
  void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const;
  void exportMitsuba(std::ofstream & file) const;
};

#endif
