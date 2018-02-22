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
  std::string import_path_grid;
  std::string export_path_grid;
  std::string scene_path;
  std::string mitsuba_path;
  bool import_;
  bool export_;
  bool import_grid;
  bool export_grid;
  bool mitsuba;
  bool load_conf_;
  uint nb_file_i;
  mutable uint nb_file_e;
  mutable uint nb_file_mitsuba;
  uint time;
  
  Grid grid;
  std::vector<Particule*> particules;
  std::vector<Subparticule*> subparticules;
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
  void importSubparticules(std::ifstream & file);
  void importParticulesAll(std::ifstream & file);
  void exportParticules(std::ofstream & file) const;
  void exportParticulesAll(std::ofstream & file) const;
  void exportSubparticules(std::ofstream & file) const;
  
  void importGrid();
  void exportGrid() const;
  void importSim();
  void exportSim() const;
  

  //  void exportMesh(std::ofstream & file) const;


  void setLoad(std::string s);
  void setExport(std::string s);
  void setImport(std::string s);
  void setScene(std::string s);
  void setExportGrid(std::string s);
  void setImportGrid(std::string s);
  void setExportMitsuba(std::string s);
  
  void backward(uint n);
  void initPoissonSampling(uint i1, uint i2, uint j1, uint j2, uint k1, uint k2);

  void loadScene();

  /** not impmented yet **/
  void saveState(std::string save_file);
  
  void addRotatingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT angular_speed);
  void addRotatingCubeOfParticules(VEC3 center, FLOAT ray, FLOAT angular_speed);
  void addExtendingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addSimpleShearingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addSimpleShearingCubeOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addPurShearingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addPurShearingCubeOfParticules(VEC3 center, FLOAT ray, FLOAT speed);
  void addTranslatingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed);

  void addSphereOfCenterOrientedParticules(VEC3 center, FLOAT ray);

  void fix(VEC3 min, VEC3 max);
  void move(VEC3 min, VEC3 max, VEC3 trans);

  void exportMitsuba() const;
  void exportMitsuba(std::string file_name) const;
};



#endif
