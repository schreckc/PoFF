#ifndef OPEN_BOX_OBSTACLE_HPP
#define OPEN_BOX_OBSTACLE_HPP

#include "Obstacle.hpp"

class OpenBoxObstacle : public Obstacle {
private :
  VEC3 min_pos;
  VEC3 max_pos;

  std::vector<VEC3> v;
  VEC3 size;
  VEC3 middle;
  
  //  const static float vertices[90];
  const static float normals[90];
  // float couleurs[108];
  const static float coordTexture[60];  
public :
  OpenBoxObstacle(VEC3 min_p = VEC3(0, 0, 0), VEC3 max_p = VEC3(0, 0, 0), int shader = -1);
  ~OpenBoxObstacle();

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
