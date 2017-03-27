#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include "Object.hpp"

#include "mpm_conf.hpp"
#include "Eigen/Dense"
#include "Eigen/Core"

using namespace Eigen;

class Particule : public Object {

private :
  
  VEC3 pos0; //init position
  VEC3 pos; //position
  VEC3 vel; //velocity

  Vector3i cell;

  FLOAT v0; //init volume
  FLOAT v; //volume
  FLOAT m; //masse
  FLOAT density;
  
  MAT3 F_p; //plastic deformation
  MAT3 F_e; //elastic deformation

  MAT3 B;
  FLOAT alpha; //parametrize the yield surface
  FLOAT hardenning;

  VEC3 normal;

 mutable MAT3 forceIncrement;
  
public:
  Particule(Shader* shader = NULL);
  Particule(FLOAT mass, FLOAT vol, VEC3 p, VEC3 n = VEC3(0, 0, 0), VEC3 vel = VEC3(0, 0, 0), Shader* shader = NULL);

    
  void animate();
  void draw(glm::mat4 m = glm::mat4(1.0f), Shader *s = NULL);

  VEC3 getPosition() const;
  void setPosition(VEC3 p);
  VEC3 getVelocity() const;
  void setVelocity(VEC3 velo);

  FLOAT getMass() const;
  FLOAT getVolume() const;
  const MAT3& getB() const;
  void setB(const MAT3 & b);
  Vector3i getCell() const;
  
  MAT3 getDeformationElastic() const;
  void addDeformationElastic(MAT3 Ddef); //F_e^n+1 = (I + Ddef)F_e^n
  MAT3 getDeformationPlastic() const;
  MAT3 getDeformation() const;

  const MAT3& getForceIncrement() const;
 
  FLOAT weight(Vector3i node);
  VEC3 gradWeight(Vector3i node);
  void updateForceIncrement();
  void update(VEC3 & p, VEC3 & v, MAT3 & b, MAT3 & t); //TODO : include plactity

  void initVolume(FLOAT d);

  MAT3 energyDerivative(VEC3 sigma); // take a diag mat in entry
};


//TODO
void project(VEC3 sigma, FLOAT alpha, VEC3 & T, FLOAT & plastic_def);


#endif
