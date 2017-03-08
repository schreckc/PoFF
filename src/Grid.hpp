#ifndef GRID_HPP
#define GRID_HPP

#include "Object.hpp"

#include <vector>
#include <list>
#include "Eigen/Core"
#include "Particule.hpp"
#include "Obstacle.hpp"

using namespace Eigen;

class Grid : public Object {

private :
  GLfloat* vertices;
  GLfloat* colors;
  uint nb_lines;

  FLOAT spacing;
  
  FLOAT x_max, y_max, z_max;
  uint i_max, j_max, k_max;
  uint nb_nodes;
  uint nb_cells;

  std::vector<FLOAT> masses;
  std::vector<bool> active_nodes;
  std::vector<VEC3> velocities;
  std::vector<VEC3> inter_velocities;
  std::vector<VEC3> positions;
  std::vector<std::list<Particule*> > cells;
  std::vector<FLOAT> distance_collision;

  inline uint index(uint i, uint j, uint k) const;
  
public :
  Grid();
  Grid(FLOAT width, FLOAT depth, FLOAT height, FLOAT space_step, Shader* shader = NULL);
  
  void animate();
  void draw(glm::mat4 m = glm::mat4(1.0f), Shader *s = NULL);

  VEC3 position(uint i, uint j, uint k) const;
  VEC3 velocity(uint i, uint j, uint k) const;
  VEC3 & velocity(uint i, uint j, uint k);

  void nextStep(); //reset values in vectors

  void particulesToGrid(std::vector<Particule*> & particules);
  void gridToParticules(std::vector<Particule*> & particules);

  void initCollision(Obstacle *ob);
  void initVolumes(std::vector<Particule*> & particules);
  void collision(Obstacle *ob);
    
};

#endif
