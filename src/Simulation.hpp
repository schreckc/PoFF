#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <list>
#include <fstream>

#include "Grid.hpp"
#include "Particule.hpp"
#include "Obstacle.hpp"


class Simulation : public Object {

private :
  std::string conf_file;
  std::string import_path;
  std::string export_path;
  std::string scene_path;
  bool import_;
  bool export_;
  bool load_conf_;
  uint nb_file_i;
  mutable uint nb_file_e;
  uint t;
  
  Grid grid;
  std::vector<Particule*> particules;
  std::list<Obstacle*> obstacles;

  inline FLOAT weight(FLOAT x);
public :
  Simulation(int shader);
  ~Simulation();

  void init();
  void clear();
  void clearParticules();
  
  void animate();
#ifndef NO_GRAPHICS_ 
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);
#endif

  void oneStep();

  void importParticules(std::ifstream & file);
  void importParticulesAll(std::ifstream & file);
  void exportParticules(std::ofstream & file) const;
  void exportParticulesAll(std::ofstream & file) const;
  void importSim();
  void exportSim() const;

  void setLoad(std::string s);
  void setExport(std::string s);
  void setImport(std::string s);
  void setScene(std::string s);
  
  void backward(uint n);
  //TODO
  void initPoissonSampling(uint i1, uint i2, uint j1, uint j2, uint k1, uint k2);

  void loadScene();

  void saveState(std::string save_file);
  
  void addRotatingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT angular_speed);
  void addExtendingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addSimpleShearingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addPurShearingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addTranslatingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
};



#endif
