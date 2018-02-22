#ifndef SUBPARTICULE_HPP
#define SUBPARTICULE_HPP

#include "mpm_conf.hpp"
#include "Object.hpp"

class Subparticule : public Object {

private :
  VEC3 pos;
  VEC3 vel;

  Vector3i cell;

  FLOAT mass;
  MAT3 rotation;

public :

  Subparticule(int shader = -1);
  Subparticule(FLOAT mass, VEC3 p, VEC3 vel = VEC3(0, 0, 0), int shader = -1);

    
  void animate();

#ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);
#endif
  
  VEC3 getPosition() const;
  void setPosition(VEC3 &p);
  VEC3 getVelocity() const;
  void setVelocity(VEC3 &velo);
  
  FLOAT getMass() const;
  void setMass(FLOAT mass);

  Vector3i getCell() const;
  
  void rotate(MAT3 &rot);
  MAT3 setRotation(MAT3 &rot);
  MAT3 getRotation() const;

  void update(VEC3 &p, VEC3 &vel);
  FLOAT weight(Vector3i node);

  void eulerStep(VEC3 forces = VEC3(0, 0, 0));
  void exportMitsuba(std::ofstream &file);
};

#endif
