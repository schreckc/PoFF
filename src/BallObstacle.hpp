#ifndef BALL_OBSTACLE_HPP
#define BALL_OBSTACLE_HPP

#include "Obstacle.hpp"

class BallObstacle : public Obstacle {
private :
  VEC3 pos;
  FLOAT ray;
  
public :
  BallObstacle(VEC3 p = VEC3(0, 0, 0), FLOAT r = 1, int shader = -1);
  ~BallObstacle();

  //void animate();
  void apply(Motion m);
#ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);
#endif

  
  
  VEC3 getPosition() const;
  FLOAT getRay() const;
  VEC3 getNormal(VEC3 p) const;
   
  FLOAT distance(VEC3 v) const;
  void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const;
};

#endif
