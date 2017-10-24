#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include "Particule.hpp"
#include <list>

struct Motion {
  VEC3 translation;
  FLOAT scale;
  MAT3 rotation;
  VEC3 center;
  float rotation_center_def;
  FLOAT begin_time, end_time;
  Motion(VEC3 trans = VEC3(0, 0, 0), FLOAT sc = 1, MAT3 rot = MAT3::Identity()) :
    translation(trans), scale(sc), rotation(rot), begin_time(0), end_time(0), rotation_center_def(false) {}
  Motion(MAT3 rot) : translation(VEC3(0, 0, 0)), scale(1), rotation(rot), begin_time(0), end_time(0), rotation_center_def(false) {}
  bool isRunning(FLOAT time) {return time >= begin_time && (end_time == 0 || time < end_time);}
};    


class Obstacle : public Object {

private :
  FLOAT friction;
  std::list<Motion> motions;
  
public :
  Obstacle(int shader = -1);
  void animate();
  virtual void apply(Motion m) = 0;
  
#ifndef NO_GRAPHICS_ 
  virtual void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1) = 0;
#endif
  
  virtual FLOAT distance(VEC3 v) const = 0;
  virtual VEC3 getNormal(VEC3 v) const = 0;
  virtual void getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const = 0;

  FLOAT getFriction() const;
  void setFriction(FLOAT f);

  void setMotions(std::list<Motion> lm);
  void addMotion(Motion m);
  bool isMoving() const;
};

#endif
