#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include "Object.hpp"

#include "mpm_conf.hpp"
#include "Eigen/Dense"
#include "Eigen/Core"


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

  VEC3 color;
  
  //anisotropy
  VEC3 normal;
  
  VEC3 axex;
  VEC3 axey;
  VEC3 axez;
  FLOAT valx;
  FLOAT valy;
  FLOAT valz;

  MAT3 ellipse;

  MAT3 rotation;

 mutable MAT3 forceIncrement;
  
public:
  Particule(int shader = -1);
  Particule(FLOAT mass, FLOAT vol, VEC3 p, VEC3 n = VEC3(0, 0, 0), VEC3 vel = VEC3(0, 0, 0), int shader = -1);

    
  void animate();

  #ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);
  #endif
  
  VEC3 getPosition() const;
  void setPosition(VEC3 p);
  VEC3 getVelocity() const;
  void setVelocity(VEC3 velo);

  FLOAT getMass() const;
  FLOAT getVolume() const;
  const MAT3& getB() const;
  void setB(const MAT3 & b);
  Vector3i getCell() const;

  void setDensity(FLOAT d);
  void setColor(FLOAT r, FLOAT g, FLOAT b);
  
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
  void project(VEC3 sigma, VEC3 & T);
  
  //anisotropy
  void setAnisotropyAxes(VEC3 x, VEC3 y, VEC3 z);
  void setAnisotropyValues(FLOAT vx, FLOAT vy, FLOAT vz);

  void rotate(MAT3 rot);

  MAT3 getRotation() const;
  VEC3 getAnisotropy() const;
  
  void anisotropicProject(VEC3 sigma, VEC3 &T, MAT3 U);
};





#endif
