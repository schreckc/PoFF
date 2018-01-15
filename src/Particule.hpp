#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include "Object.hpp"
#include "Tensor.hpp"
#include <list>

// struct Tensor {
//   FLOAT dim;
//   std::vector<FLOAT> t;
//   Tensor(FLOAT d = 3): dim(d) {
//     t =  std::vector<FLOAT>(pow(d,4));
//     for (uint i = 0; i < t.size(); ++i) {
//       t[i] = 0;
//     }
//   };
//   FLOAT& operator() (int i, int j, int k, int l) {
//     int ind = pow(dim,3)*i+pow(dim,2)*k+dim*j+l;
//     return t[ind];
//   }
//   const FLOAT& operator()(int i, int j, int k, int l) const {
//     int ind = pow(dim,3)*i+pow(dim, 2)*k+dim*j+l;
//     return t[ind];
//   }
// };

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
  MAT3 F; //F_e*F_p

  MAT3 B;
  FLOAT alpha; //parametrize the yield surface
  FLOAT hardenning;

  VEC3 color;
  
  //anisotropy
  VEC3 normal;
  
  // VEC3 axex;
  // VEC3 axey;
  // VEC3 axez;

  MAT3 axes;
  
  FLOAT valx;
  FLOAT valy;
  FLOAT valz;

  MAT3 ellipse;

  MAT3 rotation;

  MAT3 orientation;

  MAT3 prevD;
  MAT3 mix_rot;
  
 mutable MAT3 forceIncrement;
  VEC3 energy_der;
  Tensor energy_second_der;

  Tensor anisotropy_strain;
  Tensor inv_anisotropy_strain;
  Tensor anisotropy_stress;
  Tensor inv_anisotropy_stress;

  // volume correction
  FLOAT vp; //plastic volume change 

  bool fixed;
  
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
  void setMass(FLOAT mass);
  FLOAT getVolume() const;
  void setVolume(FLOAT v);
  FLOAT getDensity() const;
  void setDensity(FLOAT d);
  FLOAT getVolumeCorrection() const;
  void setVolumeCorrection(FLOAT corr);
  const MAT3& getB() const;
  void setB(const MAT3 & b);
  Vector3i getCell() const;

 
  void setColor(FLOAT r, FLOAT g, FLOAT b);
  
  MAT3 getDeformationElastic() const;
  void addDeformationElastic(MAT3 Ddef); //F_e^n+1 = (I + Ddef)F_e^n
  void setDeformationElastic(MAT3 F); 
  MAT3 getDeformationPlastic() const;
  void setDeformationPlastic(MAT3 F); 
  MAT3 getDeformation() const;

  const MAT3& getForceIncrement() const;
  const Tensor& getSecondEnergyDer() const;
 
  FLOAT weight(Vector3i node);
  VEC3 gradWeight(Vector3i node);
  void updateForceIncrement();
  
  void setAnisotropyTensor(VEC3 a);
  void setAnisotropyTensor(MAT3 a);
  void update(VEC3 & p, VEC3 & v, MAT3 & b, MAT3 & t); 

  void initVolume(FLOAT d);

  void computeEnergyDerivative(VEC3 sigma); // take a diag mat in entry
  MAT3 linearElasticity();
  void project(VEC3 sigma, VEC3 & T);
  
  //anisotropy
  void setAnisotropyAxes(VEC3 x, VEC3 y, VEC3 z);
  void setAnisotropyRotation(MAT3 rot);
  void setAnisotropyValues(FLOAT vx, FLOAT vy, FLOAT vz);

  void rotate(MAT3 rot);

  MAT3 getRotation() const;
  VEC3 getAnisotropy() const;

  MAT3 getMixRot()const;
  
  void anisotropicProject(VEC3 sigma, VEC3 &T, MAT3 U);
  
  void computeEnergySecondDer(VEC3 sigma, MAT3 U, MAT3 V);
  FLOAT energySecondDer(VEC3 sigma, uint i, uint j);

  MAT3 getOrientation();

  void fix();

  void addToMesh(std::list<VEC3> & points, std::list<VEC3> & normals,
		 std::list<VEC2> & tex_points, std::list<unsigned int> & connectivity);

  void exportMitsuba(std::ofstream &file);
};





#endif
