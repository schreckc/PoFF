#include "Grid.hpp"
#include <algorithm>
#include <omp.h>
#include "Eigen/IterativeLinearSolvers"

#include "Scene.hpp"
#include "Simulation.hpp"
#include "Sphere.hpp"
#include "error.hpp"
#include "mpm_conf.hpp"
#include <list>

inline uint Grid::index(uint i, uint j, uint k) const {
  return i*(j_max+1)*(k_max+1) + j*(k_max+1) + k;
}

Vector3i Grid::nodeFromIndex(int ind) const {
  int i, j, k;
  i = (int) ind / ((j_max+1)*(k_max+1));
  int aux = ind - i*(j_max+1)*(k_max+1);
  j = (int) aux / (k_max+1);
  k = aux - j*(k_max+1);
  return Vector3i(i, j, k);
}

Grid::Grid() : Object(), x_max(0),  y_max(0), z_max(0),  i_max(0),  j_max(0),  k_max(0) {
}

Grid::Grid(FLOAT width, FLOAT depth, FLOAT height, FLOAT space_step, int shader): Object(shader) {
  x_max = width;
  y_max = depth;
  z_max = height;
  spacing = space_step;
    
  i_max = x_max/mpm_conf::grid_spacing_ + 1;
  j_max = y_max/mpm_conf::grid_spacing_ + 1;
  k_max = z_max/mpm_conf::grid_spacing_ + 1;

  nb_lines = ((i_max+1)*(j_max+1) + (k_max+1)*(j_max+1) +(k_max+1)*(i_max+1));
  nb_nodes = (i_max+1)*(j_max+1)*(k_max+1);
  nb_cells = (i_max)*(j_max)*(k_max);
  
  masses = std::vector<FLOAT>(nb_nodes);
  active_nodes = std::vector<bool>(nb_nodes);
  fixed_nodes = std::vector<bool>(nb_nodes);
  fixed_velocities = std::vector<VEC3>(nb_nodes);
  velocities = std::vector<VEC3>(nb_nodes);
  inter_velocities = std::vector<VEC3>(nb_nodes);
  prev_velocities = std::vector<VEC3>(nb_nodes);
  positions = std::vector<VEC3>(nb_nodes);
  new_positions = std::vector<VEC3>(nb_nodes);
  cells = std::vector<std::list<Particule*> >(nb_cells);
  distance_collision = std::vector<FLOAT>(nb_nodes);
  rotations = std::vector<MAT3>(nb_nodes);

  for (uint i = 0; i <= i_max; ++i) {
    for (uint j = 0; j <= j_max; ++j) {
      for (uint k = 0; k <= k_max; ++k) {
	positions[index(i, j, k)] = VEC3(i*mpm_conf::grid_spacing_, j*mpm_conf::grid_spacing_, k*mpm_conf::grid_spacing_);
	velocities[index(i, j, k)] = VEC3(0, 0, 0);
	fixed_velocities[index(i, j, k)] = VEC3(0, 0, 0);
	fixed_nodes[index(i, j, k)] = false;
	rotations[index(i, j, k)] = MAT3::Zero();
      }
    }
  }
  
#ifndef NO_GRAPHICS_ 
  vertices = new GLfloat[nb_lines*6];
  colors = new GLfloat[nb_lines*6];
 
  uint h = 0;
  for (uint i = 0; i <= i_max; ++i) {
    for (uint j = 0; j <= j_max; ++j) {
      VEC3 p1 = positions[index(i, j, 0)];
      VEC3 p2 = positions[index(i, j, k_max)];
      for (uint l = 0; l < 3; ++l) {
   	vertices[h+l] = p1(l);
   	vertices[h+l+3] = p2(l);
      }
      h += 6;
    }
  }

  for (uint i = 0; i <= i_max; ++i) {
    for (uint k = 0; k <= k_max; ++k) {
      VEC3 p1 = positions[index(i, 0, k)];
      VEC3 p2 = positions[index(i, j_max, k)];
      for (uint l = 0; l < 3; ++l) {
  	vertices[h+l] = p1(l);
  	vertices[h+l+3] = p2(l);
      }
      h += 6;
    }
  }
  for (uint k = 0; k <= k_max; ++k) {
    for (uint j = 0; j <= j_max; ++j) {
      VEC3 p1 = positions[index(0, j, k)];
      VEC3 p2 = positions[index(i_max, j, k)];
      for (uint l = 0; l < 3; ++l) {
  	vertices[h+l] = p1(l);
  	vertices[h+l+3] = p2(l);
      }
      h += 6;
    }
  }

  for (uint i = 0; i < nb_lines*6; i+=3) {
    colors[i] = 1;
    colors[i+1] = 0;
    colors[i+2] = 0;
  }
#endif

  nextStep();
}


void Grid::animate() {
  
}

#ifndef NO_GRAPHICS_ 
void Grid::draw(glm::mat4 m, int s) {
  if (!positions.empty()) {
    GLfloat ext_vert[72];
    for (uint l = 0; l < 3; ++l) {
      ext_vert[l] = positions[index(0, 0, 0)](l);
      ext_vert[l+3] = positions[index(i_max, 0, 0)](l);

      ext_vert[l+6] = positions[index(0, 0, 0)](l);
      ext_vert[l+9] = positions[index(0, j_max, 0)](l);

      ext_vert[l+12] = positions[index(0, 0, 0)](l);
      ext_vert[l+15] = positions[index(0, 0, k_max)](l);

      ext_vert[l+18] = positions[index(i_max, j_max, k_max)](l);
      ext_vert[l+21] = positions[index(i_max, j_max, 0)](l);

      ext_vert[l+24] = positions[index(i_max, j_max, k_max)](l);
      ext_vert[l+27] = positions[index(i_max, 0, k_max)](l);

      ext_vert[l+30] = positions[index(i_max, j_max, k_max)](l);
      ext_vert[l+33] = positions[index(0, j_max, k_max)](l);

      ext_vert[l+36] = positions[index(i_max, 0, 0)](l);
      ext_vert[l+39] = positions[index(i_max, j_max, 0)](l);

      ext_vert[l+42] = positions[index(i_max, 0, 0)](l);
      ext_vert[l+45] = positions[index(i_max, 0, k_max)](l);

      ext_vert[l+48] = positions[index(0, j_max, 0)](l);
      ext_vert[l+51] = positions[index(i_max, j_max, 0)](l);

      ext_vert[l+54] = positions[index(0, j_max, 0)](l);
      ext_vert[l+57] = positions[index(0, j_max, k_max)](l);

      ext_vert[l+60] = positions[index(0, 0, k_max)](l);
      ext_vert[l+63] = positions[index(i_max, 0, k_max)](l);

      ext_vert[l+66] = positions[index(0, 0, k_max)](l);
      ext_vert[l+69] = positions[index(0, j_max, k_max)](l);
    }
    GLfloat ext_col[72];
    for (uint l = 0; l < 72; l+=3) {
      ext_col[l] = 1.0f;
      ext_col[l+1] = 0;
      ext_col[l+2] = 0;
    }

    glLineWidth(1.0f);

    enableShader();
    setMVP(m, s);
  
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ext_vert);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, ext_col);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, 24);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    disableShader();
     
    glLineWidth(1.0f);

    enableShader();
    setMVP(m, s);

    glLineWidth(1.0f);
    for (uint i = 0; i <= i_max; ++i) {
      for (uint j = 0; j <= j_max; ++j) {
	for (uint k = 0; k <= k_max; ++k) {
	  uint ind = index(i, j, k);
	  if (active_nodes[ind]) {
	    VEC3 pos = positions[ind];
	    VEC3 vel = mpm_conf::dt_ * velocities[ind]+pos; 
	    GLfloat vel_line[6] = {pos(0),  pos(1), pos(2), vel(0), vel(1), vel(2)};
	    GLfloat vel_color[6] = {0, 0, 1, 0, 0, 1};
	    GLfloat vel_color2[6] = {1, 0, 0, 1, 0, 0};

	    enableShader();
	    setMVP(m, s);
  
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vel_line);
	    glEnableVertexAttribArray(0);

	    if (velocities[ind].norm() < 2) {
	      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vel_color);
	    } else {
	      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vel_color2);
	    }
	    glEnableVertexAttribArray(1);

	    glDrawArrays(GL_LINES, 0, 2);

	    glDisableVertexAttribArray(0);
	    glDisableVertexAttribArray(1);

	    disableShader();
      
	  }
	}
      }
    }
  
  }
}
#endif

VEC3 Grid::position(uint i, uint j, uint k) const {
  assert(i <= i_max);
  assert(j <= j_max);
  assert(k <= k_max);
  return positions[index(i, j, k)];
}

VEC3 Grid::velocity(uint i, uint j, uint k) const {
  assert(i <= i_max);
  assert(j <= j_max);
  assert(k <= k_max);
  return velocities[index(i, j, k)];
}

VEC3 & Grid::velocity(uint i, uint j, uint k) {
  assert(i <= i_max);
  assert(j <= j_max);
  assert(k <= k_max);
  return velocities[index(i, j, k)];
}
 

void Grid::nextStep() {
#pragma omp parallel for
  for (uint i = 0; i < nb_nodes; ++i) {
    masses[i] = 0;
    active_nodes[i] = false;
    inter_velocities[i] = VEC3(0, 0, 0);
    prev_velocities[i] = velocities[i]; 
    velocities[i] = VEC3(0, 0, 0);
    rotations[i] = MAT3::Zero();
  }

#pragma omp parallel for
  for (uint i = 0; i < nb_cells; ++i) {
    cells[i].clear();
  }
}

void Grid::smoothVelocity() {
  std::vector<FLOAT> laplacien = {0, 0, 0,
				  0, 1.0/6.0, 0,
				  0, 0, 0,

				  0, 1.0/6.0, 0,
				  1.0/6.0, -1, 1.0/6.0,
				  0, 1.0/6.0, 0,

				  0, 0, 0,
				  0, 1.0/6.0, 0,
				  0, 0, 0};
  
  
  
#pragma omp parallel for
  for (int i = 0; i <= (int)i_max; ++i) {
    for (int j = 0; j <= (int)j_max; ++j) {
      for (int k = 0; k <= (int)k_max; ++k) {
	
	uint ind = index(i, j, k);
	if (active_nodes[ind]) {
	  VEC3 sv(0, 0, 0);

	  for (int l = 0; l < 3; ++l) {
	    if (i-1+l >= 0 && i-1+l < (int)i_max) {

	      for (int m = 0; m < 3; ++m) {
		if (j-1+m >= 0 && j-1+m < (int)j_max) {

		  for (int n = 0; n < 3; ++n) {
		    if (k-1+n >= 0 && k-1+n < (int)k_max) {
		      uint ind_neigh = index(i-1+l, j-1+m, k-1+n);
		      if (active_nodes[ind_neigh]) {
			sv += inter_velocities[ind_neigh]*laplacien[l*9 + m*3 + n];
		      } else {
		       	sv += inter_velocities[ind]*laplacien[l*9 + m*3 + n];
		      }
		    }
		  }

		}
	      }
	    }
	  }
	  velocities[ind] += 1*sv;
	}

      }
    }
  }
	
}

/*** not tested ***/
void Grid::smoothRotation(std::vector<Particule*> & particules, std::vector<Subparticule*> & subparticules) {
#pragma omp parallel for
  for (int i = 0; i <= (int)i_max; ++i) {
    for (int j = 0; j <= (int)j_max; ++j) {
      for (int k = 0; k <= (int)k_max; ++k) {
	uint ind = index(i, j, k);
	if (active_nodes[ind]) {
	  // QUATERNION q(MAT3::Identity());
	  // FLOAT we = 0;
	  for (int l = i - 2; l < i + 2; ++l) {
	    if (l >= 0 && l < (int)i_max) {
	      for (int m = j - 2; m < j + 2; ++m) {
		if (m >= 0 && m < (int)j_max) {
		  for (int n = k - 2; n < k + 2; ++n) {
		    if (n >= 0 && n < (int)k_max) {
		      uint indc = l*j_max*k_max + m*(k_max) + n;
		      for (auto& p : cells[indc]) {
		      
			FLOAT w = p->weight(Vector3i(i, j, k));
			//if (w > 10e-8) {
			// QUATERNION qp( p->getMixRot());
			// q = q.slerp( w/(we+w), qp);
			// we += w;
			MAT3 r = p->getMixRot();
			ANGLE_AXIS aa(r);
			FLOAT angle = w*aa.angle();
			rotations[ind] += aa.toRotationMatrix();
			
			//}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	  HouseholderQR<MAT3> decomp(rotations[ind]);
	  MAT3 Q = decomp.householderQ();
	  ANGLE_AXIS aa(Q);
	  FLOAT angle = aa.angle();
	  if (angle < 0.1) {
	    rotations[ind] = Q;
	  }
	  //	 INFO(3, "Q\n"<<Q*Q.transpose());
	  //rotations[ind] = q.toRotationMatrix();
	  //INFO(3, "rot ind\n"<<rotations[ind]);
	}
      }
    }
  }

#pragma omp parallel for
  for (uint ip = 0; ip < subparticules.size(); ++ip) {
    Subparticule *p = subparticules[ip];
    Vector3i cell = p->getCell();
    // QUATERNION q(MAT3::Identity());
    // FLOAT we = 0;
    MAT3 rot = MAT3::Zero();
    for (int i = cell(0) - 2; i <= cell(0) + 2; ++i) {
      if (i >= 0 && i <= (int)i_max) {
    	for (int j = cell(1) - 2; j <= cell(1) + 2; ++j) {
    	  if (j >= 0 && j <= (int)j_max) {
    	    for (int k = cell(2) - 2; k <= cell(2) + 2; ++k) {
	      if (k >= 0 && k <= (int)k_max) {
		uint ind = index(i, j, k);
		if (!active_nodes[ind]) {
		  rotations[ind] = MAT3::Zero();
		}
		FLOAT w = p->weight(Vector3i(i, j, k));
		// if (w > 10e-8) {
		// QUATERNION qp(rotations[ind]);
		// q = q.slerp(w/(we+w), qp);
		// we += w;

		MAT3 r = rotations[ind];
		ANGLE_AXIS aa(r);
		FLOAT angle = w*aa.angle();
		rot += aa.toRotationMatrix();
		//rot += w*rotations[ind];
		//   }

		//	}
	      }
	    }
	  }
	}
      }
    }
    HouseholderQR<MAT3> decomp(rot);
    MAT3 Q = decomp.householderQ();
    ANGLE_AXIS aa(Q);
    FLOAT angle = aa.angle();
    if (angle < 0.5 || M_PI - angle < 0.5) {
      p->rotate(Q);
    }
    // MAT3 rot = q.toRotationMatrix();
    // p->rotate(rot);
  }
}
  

void Grid::particulesToGrid(std::vector<Particule*> & particules) {
  for (auto &p : particules) {
    Vector3i c = p->getCell();
    if (c(0) <= 0 && c(0) >= i_max-1 && c(1) <= 0 && c(1) >= j_max-1 && c(2) <= 0 && c(2) >= k_max-1) {
      p->setOut(true);
    } else {
    uint ind = c(0)*j_max*k_max + c(1)*(k_max) + c(2);
    if (ind < nb_cells) {
      cells[ind].push_back(p);
    }
    }
  }
  
  uint nb_ac = 0;
  for (uint i = 0; i < cells.size(); ++i) {
    if (cells[i].size() != 0) {
      ++nb_ac;
    }
  }
  FLOAT s3 = pow(mpm_conf::grid_spacing_, 3);
  FLOAT s2 = mpm_conf::grid_spacing_*mpm_conf::grid_spacing_;
#pragma omp parallel for
  for (int i = 0; i <= (int)i_max; ++i) {
    for (int j = 0; j <= (int)j_max; ++j) {
      for (int k = 0; k <= (int)k_max; ++k) {
	uint ind = index(i, j, k);
	VEC3 f(0, 0, 0);
	for (int l = i - 2; l < i + 2; ++l) {
	  if (l >= 0 && l < (int)i_max) {
	    for (int m = j - 2; m < j + 2; ++m) {
	      if (m >= 0 && m < (int)j_max) {
		for (int n = k - 2; n < k + 2; ++n) {
		  if (n >= 0 && n < (int)k_max) {
		    uint indc = l*j_max*k_max + m*(k_max) + n;
		    for (auto& p : cells[indc]) {
	
		      FLOAT w = p->weight(Vector3i(i, j, k));
		      if (w > 0 && !p->isOut()) {
			active_nodes[ind] = true;
			masses[ind] += w*p->getMass();
			if (mpm_conf::method_ == mpm_conf::apic_) {
			  MAT3 C = 3.0/s2*p->getB();
			  MAT3 C_skew = 0.5*(C - C.transpose());
			  MAT3 C_sym = 0.5*(C + C.transpose());
			  FLOAT v = 1; // damping parameter
			  velocities[ind] += w*p->getMass()*(p->getVelocity() + (C_skew + (1-v)*C_sym)*(positions[ind] - p->getPosition()));
			  // velocities[ind] += w*p->getMass()*(p->getVelocity() + 3.0/s2*p->getB()*(positions[ind] - p->getPosition()));
			} else {
			  velocities[ind] += w*p->getMass()*p->getVelocity();
			}
			IS_DEF(velocities[ind](0));
			VEC3 incrf = p->getForceIncrement()*p->gradWeight(Vector3i(i, j, k));
			if (!std::isnan(incrf(0)) && !std::isinf(incrf(0))) {
			  f += incrf;
			}
			IS_DEF(f(0));
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
	if (active_nodes[ind]) {
	  IS_DEF(velocities[ind](0));
	  if (masses[ind] > 1e-8*mpm_conf::dt_) {
	    if (std::isnan(f(0)) || std::isinf(f(0))) {
	      f = VEC3(0, 0, 0);
	    }
	    velocities[ind] -= mpm_conf::dt_*f + mpm_conf::dt_*mpm_conf::damping_*velocities[ind];
	    velocities[ind] /= masses[ind];
	  } else {
	    active_nodes[ind] = false;
	    velocities[ind] = VEC3(0, 0, 0);
	    masses[ind] = 0;
	  }
	  IS_DEF(velocities[ind](0));
	  velocities[ind] += mpm_conf::dt_*mpm_conf::gravity_;
	  inter_velocities[ind] = velocities[ind];//particules.front()->gradWeight(Vector3i(i, j, k));
	  new_positions[ind] = positions[ind] + mpm_conf::dt_*velocities[ind];
	  IS_DEF(velocities[ind](0));
	}
      }
    }
  }
  //INFO(2, "END Part 2 Grid");
}


/*** not tested !!*/
void Grid::particulesToGridImplicite(std::vector<Particule*> & particules) {
  for (auto &p : particules) {
    Vector3i c = p->getCell();
    uint ind = c(0)*j_max*k_max + c(1)*(k_max) + c(2);
    if (ind < nb_cells) {
      cells[ind].push_back(p);
    }   
  }
  
  uint nb_ac = 0;
  for (uint i = 0; i < cells.size(); ++i) {
    if (cells[i].size() != 0) {
      ++nb_ac;
    }
  }
  
  FLOAT s2 = mpm_conf::grid_spacing_*mpm_conf::grid_spacing_;
#pragma omp parallel for
  for (int i = 0; i <= (int)i_max; ++i) {
    for (int j = 0; j <= (int)j_max; ++j) {
      for (int k = 0; k <= (int)k_max; ++k) {
	uint ind = index(i, j, k);
	VEC3 f(0, 0, 0);
	for (int l = i - 2; l < i + 2; ++l) {
	  if (l >= 0 && l < (int)i_max) {
	    for (int m = j - 2; m < j + 2; ++m) {
	      if (m >= 0 && m < (int)j_max) {
		for (int n = k - 2; n < k + 2; ++n) {
		  if (n >= 0 && n < (int)k_max) {
		    uint indc = l*j_max*k_max + m*(k_max) + n;
		    for (auto& p : cells[indc]) {
	
		      FLOAT w = p->weight(Vector3i(i, j, k));
		      if (w > 0) {
			active_nodes[ind] = true;
			masses[ind] += w*p->getMass();
			if (mpm_conf::method_ == mpm_conf::apic_) {
			  // MAT3 C = 3.0/s2*p->getB();
			  // MAT3 C_skew = 0.5*(C - C.transpose());
			  // MAT3 C_sym = 0.5*(C + C.transpose());
			  // FLOAT v = 1;
			  //  velocities[ind] += w*p->getMass()*(p->getVelocity() + (C_skew + (1-v)*C_sym)*(positions[ind] - p->getPosition()));
			  velocities[ind] += w*p->getMass()*(p->getVelocity() + 3.0/s2*p->getB()*(positions[ind] - p->getPosition()));
			} else {
			  velocities[ind] += w*p->getMass()*p->getVelocity();
			}
			IS_DEF(velocities[ind](0));
			VEC3 incrf = p->getForceIncrement()*p->gradWeight(Vector3i(i, j, k));
			if (!std::isnan(incrf(0)) && !std::isinf(incrf(0))) {
			  f += incrf;
			}
			IS_DEF(f(0));
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
	if (active_nodes[ind]) {
	  IS_DEF(velocities[ind](0));
	  if (masses[ind] > 1e-8*mpm_conf::dt_) {
	    if (std::isnan(f(0)) || std::isinf(f(0))) {
	      f = VEC3(0, 0, 0);
	    }
	    velocities[ind] -= mpm_conf::dt_*f + mpm_conf::dt_*mpm_conf::damping_*velocities[ind];
	    velocities[ind] /= masses[ind];
	  } else {
	     
	    velocities[ind] = VEC3(0, 0, 0);
	  }
	  IS_DEF(velocities[ind](0));
	  //	   velocities[ind] += mpm_conf::dt_*mpm_conf::gravity_;
	  inter_velocities[ind] = velocities[ind];//particules.front()->gradWeight(Vector3i(i, j, k));
	  new_positions[ind] = positions[ind] + mpm_conf::dt_*velocities[ind];
	  //	  inter_velocities[ind] =  velocities[ind];//particules.front()->gradWeight(Vector3i(i, j, k));
	  // INFO(3, "vel\n" << velocities[ind]);
	  //	  INFO(3, "density "<<masses[ind]/pow(mpm_conf::grid_spacing_, 3));
	  IS_DEF(velocities[ind](0));
	}
      }
    }
  }
  uint n_active = 0;
  for (uint i = 0; i < nb_nodes; ++i) {
    if (active_nodes[i] /*&& masses[i] > 1e-15*mpm_conf::dt_*/) {
      ++n_active;
    }
  }
  std::vector<uint> l_index(n_active);
  uint inde = 0;
  for (uint i = 0; i < nb_nodes; ++i) {
    if (active_nodes[i] /*&& masses[i] > 1e-15*mpm_conf::dt_*/) {
      l_index[inde] = i;
      //INFO(3, "prev index "<<i);
      ++inde;
    }
  }

  MATX K(3*n_active, 3*n_active);
  FLOAT beta = 0.5;
#pragma omp parallel for
  for (uint i = 0; i < 3*n_active; ++i) {
    for (uint j = 0; j < 3*n_active; ++j) {
      K(i, j) = 0;
    }
  }
  INFO(3, "nb nodes  " <<nb_nodes<<" "<<n_active<<" "<<Eigen::nbThreads( ));


  
#pragma omp parallel for
  for (uint i = 0; i < n_active; ++i) {
    uint index1 = l_index[i];
    //   if (active_nodes[i] && masses[i] > 1e-8*mpm_conf::dt_*mpm_conf::dt_) {
    for (uint l = 0; l < 3; ++l) {
      K(3*i + l, 3*i + l) = masses[index1];//1.0;
    }
    //  INFO(3, "index "<<index1<<" "<<l_index[10]);
    for (uint j = i; j < n_active; ++j) {
      uint index2 = l_index[j];
      bool close_enough = true;
      Vector3i indi = nodeFromIndex(index1);
      Vector3i indj = nodeFromIndex(index2);
      for (uint k = 0; k < 3; ++k) {
	close_enough = close_enough && std::abs(indj(k) - indi(k)) <= 2;
      }

      if (close_enough) {
	//   //	    if (active_nodes[j] && masses[j] > 1e-8*mpm_conf::dt_*mpm_conf::dt_) {
	MAT3 second_der = secondDer(index1, index2, particules);
	//  MAT3 second_der2 = secondDer(index2, index1, particules);
	MAT3 Kij = beta * mpm_conf::dt_*mpm_conf::dt_/*masses[index1]*/*second_der;
	// if (i < j) {
	//   INFO(3, "Kij\n"<<Kij);
	// }
	for (uint l = 0; l < 3; ++l) {
	  for (uint m = 0; m < 3; ++m) {
	    K(3*i + l, 3*j + m) += Kij(l, m);
	    K(3*j + m, 3*i + l) += Kij(l, m);
	    // if (std::fabs(second_der(l, m) - second_der2(m, l)) > 0.01) {
	    //   	INFO(3, "i, j  der der2 "<<i<<" "<<j<<"\n"<<second_der<<"\n\n"<<second_der2);
	    //   	//TEST(false);
	    //   }
	    IS_DEF(K(3*i + l, 3*j + m));
	  }
	}
      }
      
    }
  }



  VECX V(3*n_active);
  VECX V_prev(3*n_active);
  
#pragma omp parallel for  
  for (uint i = 0; i < n_active; ++i) {
    uint ind = l_index[i];
    //      if (active_nodes[i]  && masses[i] > 1e-8*mpm_conf::dt_*mpm_conf::dt_) {
    for (uint l = 0; l < 3; ++l) {
      IS_DEF(velocities[ind](l));
      V(3*i + l) = masses[ind]*velocities[ind](l);
      V_prev(3*i + l) = velocities[ind](l);// - mpm_conf::dt_*mpm_conf::gravity_(l);
      IS_DEF(velocities[ind](l));
      IS_DEF(V(3*i + l));
    }
    //  	++ind;
    //}
  }

  //INFO(3, K);
  INFO(3, "prev vel"<<V(90)<<" "<<V(91)<<" "<<V(92));
  // INFO(3, "prev vel"<<V(90)/ masses[ l_index[30]]<<" "<<V(91)/ masses[ l_index[30]]<<" "<<V(92)/ masses[ l_index[30]]);
  //INFO(3, "part vel\n"<<particules.front()->getVelocity());
  //INFO(3, "prev vel\n"<<velocities[10]);

  ConjugateGradient<MATX, Lower|Upper> cg(K);
  // BiCGSTAB<MATX> cg;
  cg.setMaxIterations(512);
  cg.compute(K);
  VECX new_V(3*n_active);
  new_V = cg.solveWithGuess(V, V_prev);
  //new_V = cg.solve(V);
  std::cout << "#iterations:     " << cg.iterations() << std::endl;
  std::cout << "estimated error: " << cg.error()      << std::endl;
  INFO(3, "IMPLICIT: size of K "<< n_active);

#pragma omp parallel for  
  for (uint i = 0; i < n_active; ++i) {
    uint ind = l_index[i];
    //  if (active_nodes[i] && masses[i] > 1e-8*mpm_conf::dt_*mpm_conf::dt_) {
    for (uint k = 0; k < 3; ++k) {
      velocities[ind](k) = new_V(3*i+k);
      // if (std::fabs(new_V(3*i+k)) < 1e-12) {
      //   velocities[ind](k) = 0;
      // }
      IS_DEF(velocities[ind](k));
    }
    velocities[ind] += mpm_conf::dt_*mpm_conf::gravity_;
    // INFO(3, "vel\n" << velocities[ind]);
    // INFO(3, "prev vel " << V_prev(3*i)<<" "<<V_prev(3*i + 1)<<" "<<V_prev(3*i + 2));
    inter_velocities[i] = velocities[i];//particules.front()->gradWeight(Vector3i(i, j, k));
    new_positions[ind] = positions[ind] + mpm_conf::dt_*velocities[ind];
    IS_DEF(velocities[ind](0));
    //     ++ind;
    //}
  }
  //   INFO(3, "up vel"<<new_V(90)<<" "<<new_V(91)<<" "<<new_V(92));
  // TEST(std::fabs(V(90)/ masses[ l_index[30]] - new_V(90)) < 0.00000001);
  // INFO(2, "END Part 2 Grid");
}

void Grid::gridToParticules(std::vector<Particule*> & particules) {
  FLOAT s3 = pow(mpm_conf::grid_spacing_, 3);
#pragma omp parallel for
  for (uint ip = 0; ip < particules.size(); ++ip) {
    Particule *p = particules[ip];
    Vector3i cell = p->getCell();
    VEC3 vel(0, 0, 0);
    VEC3 vel_flip = p->getVelocity();
    MAT3 B = MAT3::Zero();
    MAT3 T = MAT3::Zero();
    VEC3 pos(0, 0, 0);
    VEC3 prev_pos = p->getPosition();
    FLOAT density_max = 0;
    FLOAT density_av = 0;

    for (int i = cell(0) - 2; i <= cell(0) + 2; ++i) {
      if (i >= 0 && i <= (int)i_max) {
    	for (int j = cell(1) - 2; j <= cell(1) + 2; ++j) {
    	  if (j >= 0 && j <= (int)j_max) {
    	    for (int k = cell(2) - 2; k <= cell(2) + 2; ++k) {
	      if (k >= 0 && k <= (int)k_max) {
		uint ind = index(i, j, k);
		if (active_nodes[ind]) {
		  FLOAT w = p->weight(Vector3i(i, j, k));
		  vel += w*velocities[ind];
		  vel_flip += w*(velocities[ind] - prev_velocities[ind]);
		  B += w*velocities[ind]*(positions[ind] - p->getPosition()).transpose();
		  pos += w*/*(positions[ind] + mpm_conf::dt_*velocities[ind]);*/new_positions[ind];
		  T += mpm_conf::cheat_damping_*velocities[ind]*p->gradWeight(Vector3i(i, j, k)).transpose();
		  IS_DEF(velocities[ind](0));
		  IS_DEF(w);
		  IS_DEF(pos(0));
		  if (density_max < masses[ind]) {
		    density_max = masses[ind];
		  }
		  density_av += masses[ind]*w;
		}
	      }
	    }
	  }
	}
      }
    }
    density_max /= s3;
    density_av /= s3;
    p->setDensity(density_max);
    
    /* Rotation */
    MAT3 A = MAT3::Zero(3, 3);
    FLOAT sum = 0;
    for (int i = cell(0) - 2; i <= cell(0) + 2; ++i) {
      if (i >= 0 && i <= (int)i_max) {
    	for (int j = cell(1) - 2; j <= cell(1) + 2; ++j) {
    	  if (j >= 0 && j <= (int)j_max) {
    	    for (int k = cell(2) - 2; k <= cell(2) + 2; ++k) {
    	      if (k >= 0 && k <= (int)k_max) {
    		uint ind = index(i, j, k);
    		if (active_nodes[ind]) {
    		  FLOAT w = p->weight(Vector3i(i, j, k));
    		  sum += w;
    		  A += w * (new_positions[ind] - pos)*(positions[ind] - prev_pos).transpose();
    		}
    	      }
    	    }
    	  }
    	}
      }
    }
    A /= sum;
    JacobiSVD<MAT3> svd(A, ComputeFullU | ComputeFullV);
    MAT3 rot = svd.matrixU()*svd.matrixV().transpose();
    p->rotate(rot);
    
    if (mpm_conf::method_ == mpm_conf::apic_ || mpm_conf::method_ == mpm_conf::pic_) {
      p->update(pos, vel, B, T);
    } else if (mpm_conf::method_ == mpm_conf::flip_) {
      p->update(pos, vel_flip, B, T);
    } else if  (mpm_conf::method_ == mpm_conf::mix_) {
      FLOAT alpha = 0.95;
      VEC3 v = alpha*vel_flip + (1-alpha)*vel;
      p->update(pos, v, B, T);
    }
  }
  // INFO(2, "END Grid 2 Part");
}




void Grid::gridToSubparticules(std::vector<Subparticule*> & subparticules) {
  FLOAT s3 = pow(mpm_conf::grid_spacing_, 3);
#pragma omp parallel for
  for (uint ip = 0; ip < subparticules.size(); ++ip) {
    Subparticule *p = subparticules[ip];
    Vector3i cell = p->getCell();
    VEC3 prev_pos = p->getPosition();
    VEC3 vel(0, 0, 0);
    VEC3 vel_flip = p->getVelocity();
    VEC3 pos(0, 0, 0);
    FLOAT total_masse = 0;
    FLOAT total_weigth = 0;

    for (int i = cell(0) - 2; i <= cell(0) + 2; ++i) {
      if (i >= 0 && i <= (int)i_max) {
    	for (int j = cell(1) - 2; j <= cell(1) + 2; ++j) {
    	  if (j >= 0 && j <= (int)j_max) {
    	    for (int k = cell(2) - 2; k <= cell(2) + 2; ++k) {
	      if (k >= 0 && k <= (int)k_max) {
		uint ind = index(i, j, k);
		if (!active_nodes[ind]) {
		  velocities[ind] = VEC3(0, 0, 0);
		  masses[ind] = 0;
		}
		//if (active_nodes[ind]) {
		FLOAT w = p->weight(Vector3i(i, j, k));
		vel += w*velocities[ind];
		vel_flip += w*(velocities[ind] - prev_velocities[ind]);
		// pos += w*new_positions[ind];
		pos += w*(positions[ind] + mpm_conf::dt_*velocities[ind]);
		//INFO(3, w);
		total_masse += w*masses[ind];
		total_weigth += w;
		//}
	      }
	    }
	  }
	}
      }
    }

    if (total_masse > 0.001*mpm_conf::density_*s3) {
      /* Rotation */
      MAT3 A = MAT3::Zero(3, 3);
      FLOAT sum = 0;
      for (int i = cell(0) - 2; i <= cell(0) + 2; ++i) {
        if (i >= 0 && i <= (int)i_max) {
	  for (int j = cell(1) - 2; j <= cell(1) + 2; ++j) {
	    if (j >= 0 && j <= (int)j_max) {
	      for (int k = cell(2) - 2; k <= cell(2) + 2; ++k) {
		if (k >= 0 && k <= (int)k_max) {
		  uint ind = index(i, j, k);
		  if (active_nodes[ind]) {
		    FLOAT w = p->weight(Vector3i(i, j, k));
		    sum += w;
		    A += w * (positions[ind] + mpm_conf::dt_*velocities[ind] - pos)*(positions[ind] - prev_pos).transpose();
		  }
		}
	      }
	    }
	  }
        }
      }
      A /= sum;
      JacobiSVD<MAT3> svd(A, ComputeFullU | ComputeFullV);
      MAT3 rot = svd.matrixU()*svd.matrixV().transpose();
      p->rotate(rot);
      if (vel.norm() > 2) {
	vel = VEC3(0, 0, 0);
      }
      if (mpm_conf::method_ == mpm_conf::apic_ || mpm_conf::method_ == mpm_conf::pic_) {
	p->update(pos, vel);
      } else if (mpm_conf::method_ == mpm_conf::flip_) {
	p->update(pos, vel_flip);
      } else if  (mpm_conf::method_ == mpm_conf::mix_) {
	FLOAT alpha = 0.95;
	VEC3 v = alpha*vel_flip + (1-alpha)*vel;
	p->update(pos, v);
      }
    } else {
      p->eulerStep();
    }
  }

}


void Grid::init(std::vector<Particule*> & particules) {
  for (auto &p : particules) {
    Vector3i c = p->getCell();
    uint ind = c(0)*j_max*k_max + c(1)*(k_max) + c(2);
    if (ind < nb_cells) {
      cells[ind].push_back(p);
    }
  }

  FLOAT s2 = mpm_conf::grid_spacing_*mpm_conf::grid_spacing_;
  FLOAT mt = 0;
#pragma omp parallel for
  for (int i = 0; i <= (int)i_max; ++i) {
    for (int j = 0; j <= (int)j_max; ++j) {
      for (int k = 0; k <= (int)k_max; ++k) {
	uint ind = index(i, j, k);
	VEC3 f(0, 0, 0);
	for (int l = i - 2; l < i + 2; ++l) {
	  if (l >= 0 && l < (int)i_max) {
	    for (int m = j - 2; m < j + 2; ++m) {
	      if (m >= 0 && m < (int)j_max) {
		for (int n = k - 2; n < k + 2; ++n) {
		  if (n >= 0 && n < (int)k_max) {
		    uint indc = l*j_max*k_max + m*(k_max) + n;
		    for (auto& p : cells[indc]) {
		      FLOAT w = p->weight(Vector3i(i, j, k));
		      if (w > 0) {
			active_nodes[ind] = true;
			masses[ind] += w*p->getMass();
			velocities[ind] += w*p->getMass()*p->getVelocity();
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
	mt += masses[ind];
	if (active_nodes[ind]) {
	  if (masses[ind] > 1e-8*mpm_conf::dt_) {
	    velocities[ind] /= masses[ind];
	  } else {
	    velocities[ind] = VEC3(0, 0, 0);
	  }
	}
      }
    }
  }
  FLOAT s3 = pow(mpm_conf::grid_spacing_, 3);
#pragma omp parallel for
  for (uint ip = 0; ip < particules.size(); ++ip) {
    Particule *p = particules[ip];
    // FLOAT density = mpm_conf::density_;
    FLOAT density = 0;
    Vector3i cell = p->getCell();
    for (int i = cell(0) - 2; i <= cell(0) + 2; ++i) {
      if (i >= 0 && i <= (int)i_max) {
    	for (int j = cell(1) - 2; j <= cell(1) + 2; ++j) {
    	  if (j >= 0 && j <= (int)j_max) {
    	    for (int k = cell(2) - 2; k <= cell(2) + 2; ++k) {
    	      if (k >= 0 && k <= (int)k_max) {
    		uint ind = index(i, j, k);
    		FLOAT w = p->weight(Vector3i(i, j, k));
    		density += masses[ind]*w;
    	      }
    	    }
    	  }
    	}
      }
    }
    density /= s3;
    p->initVolume(density);
  }
  double masse = 0;
  for (uint ip = 0; ip < particules.size(); ++ip) {
    Particule *p = particules[ip];
    masse += p->getMass();
  }
  INFO(2, "END Init Volume "<<masse<<" "<<mt );
}

void Grid::initCollision(std::list<Obstacle*> obstacles) {
#pragma omp parallel for
  for (uint i = 0; i < nb_nodes; ++i) {
    distance_collision[i] = 100;
    for (auto & ob : obstacles) {
      if (ob->isMoving()) {
	FLOAT d = ob->distance(positions[i]);
	if (fabs(d) < fabs( distance_collision[i])) {
	  distance_collision[i] = d;
	}
      }
    }
  }
}


void Grid::collision(std::list<Obstacle*> obstacles) {
#pragma omp parallel for
  for (uint i = 0; i < nb_nodes; ++i) {
    if (fixed_nodes[i]) {
      velocities[i] = fixed_velocities[i];
      inter_velocities[i] = VEC3(0, 0, 0);
      new_positions[i] = positions[i] + mpm_conf::dt_*velocities[i];
    } else {

      if (active_nodes[i]) {
	VEC3 n(0, 0, 1);
	FLOAT d = 10;
	FLOAT friction = mpm_conf::friction_coef_;
	VEC3 pos = positions[i] + mpm_conf::dt_*velocities[i];
	for (auto & ob : obstacles) {
	  if (ob->isMoving()) {
	    VEC3 n_cur;
	    FLOAT d_cur;
	    ob->getCollisionValues(pos, d_cur, n_cur);
	    if (fabs(d_cur) < fabs(d)) {
	      d = d_cur;
	      n = n_cur;
	      friction = ob->getFriction();
	    }
	  }
      
	  FLOAT dcomp;
	  dcomp = d - std::min(distance_collision[i],(FLOAT)0.0);
	  if (dcomp < 0) {
	    VEC3 vel_prev = velocities[i];
	    FLOAT dv = -dcomp/mpm_conf::dt_;
	    velocities[i] += dv*n;
	
	    // //friction
	    VEC3 vt = velocities[i] - velocities[i].dot(n)*n;
	    FLOAT nvt = vt.norm();
		
	    if (nvt > friction*dv) {
	      velocities[i] -= friction*dv*vt/nvt;
	    } else {
	      velocities[i] -= vt;
	    }
	  }
	}


	for (auto & ob : obstacles) {
	  if (!ob->isMoving()) {

	    FLOAT d_prev= ob->distance(positions[i]);
	    ob->getCollisionValues(positions[i] + mpm_conf::dt_*velocities[i], d, n);
	    FLOAT dcomp = d - std::min(d_prev, (FLOAT)0.0);
	    if (dcomp < 0) {
	      //VEC3 vel_prev = velocities[i];
	      FLOAT dv = -dcomp/mpm_conf::dt_;
	      velocities[i] += 3*dv*n;
	      // INFO(3, "vel "<<velocities[i]);
	      // //friction
	      VEC3 vt = velocities[i] - velocities[i].dot(n)*n;
	      FLOAT nvt = vt.norm();
	      friction = ob->getFriction();
	      if (nvt > friction*dv) {
		velocities[i] -= friction*dv*vt/nvt;
	      } else {
	       	velocities[i] -= vt;
	      }

	    }
	  }
	}
    
      }

    }
  }
  
  //  INFO(2, "END Collision");
}


/*** not tested ***/
MAT3 Grid::secondDer(uint i, uint j, std::vector<Particule*> & particules) {
  MAT3 second_der = MAT3::Zero();
  Vector3i indi = nodeFromIndex(i);
  Vector3i indj = nodeFromIndex(j);

  for (int l = indi(0) - 2; l <= indi(0) + 2; ++l) {
    if (l >= 0 && l < (int)i_max) {
      for (int m = indi(1) - 2; m <= indi(1) + 2; ++m) {
    	if (m >= 0 && m < (int)j_max) {
    	  for (int n = indi(2) - 2; n <= indi(2) + 2; ++n) {
    	    if (n >= 0 && n < (int)k_max) {
    	      uint indc = l*j_max*k_max + m*(k_max) + n;
  
	      for (auto& p : cells[indc]) {
		//	 	  for (auto& p : particules) {
		//second deriative energy for implicite scheme
		VEC3 wip = p->gradWeight(indi);
		VEC3 wjp = p->gradWeight(indj);
		MAT3 F = p->getDeformationElastic();
		FLOAT det = F.determinant()*p->getVolume();
		//	F = 0.5*(F + F.transpose());
		VEC3 wipF = wip;
		VEC3 wjpF = (wjp.transpose()*F).transpose();
		const Tensor T = p->getSecondEnergyDer();
		for (uint alpha = 0; alpha < 3; ++alpha) {
		  for (uint beta = 0; beta < 3; ++beta) {
   	
		    //     // INFO(3, "wipF\n"<<wipF);
		    //     // INFO(3, "wjpF\n"<<wjpF);
		    FLOAT der_ab = 0;
		    for (uint u = 0; u < 3; ++u) {
		      for (uint v = 0; v < 3; ++v) {
			der_ab += T(alpha, u, beta, v)*wjpF(v)*wipF(u);
			// 	IS_DEF(T(alpha, u, beta, v));
			// 	IS_DEF(second_der(alpha, beta));
			//TEST(std::fabs(T(alpha, u, beta, v) - T(beta, v, alpha, u)) < 0.0001);
		      }
		    }
		    second_der(alpha, beta) += det*der_ab;
		  }
		}
		 
	      }

      	    }
      	  }
      	}
      }
    }
    
  }
  //  }
  //     INFO(3, "seconde der\n"<<second_der);
  // TEST(second_der == second_der.transpose());
  // for (uint u = 0; u < 3; ++u) {
  //   for (uint v = 0; v < 3; ++v) {
  //     if (std::fabs(second_der(u, v) -second_der(v, u)) > 0.00001) {
  //       INFO(3, "seconde der\n"<<second_der);
  //       //   INFO(3, "F\n"<<F);
  //       TEST(false);
  //     }
  //   }
  // }
  return second_der;
}


void Grid::fix(VEC3 min, VEC3 max) {
  for (uint i = 0; i < nb_nodes; ++i) {
    bool to_fix = true;
    VEC3 v = positions[i];
    for (uint i = 0; i < 3; ++i) {
      to_fix = to_fix && v(i) > min(i) && v(i) < max(i);
    }
    if (to_fix) {
      fixed_nodes[i] = true;
    }
  }
}


void Grid::move(VEC3 min, VEC3 max, VEC3 trans) {
  for (uint i = 0; i < nb_nodes; ++i) {
    bool to_fix = true;
    VEC3 v = positions[i];
    for (uint i = 0; i < 3; ++i) {
      to_fix = to_fix && v(i) > min(i) && v(i) < max(i);
    }
    if (to_fix) {
      fixed_nodes[i] = true;
      fixed_velocities[i] = trans;
    }
  }
}


void Grid::exportGrid(std::ofstream & file) const {
  file << "# grid: "<<i_max<<"x"<<j_max<<"x"<<k_max<<" / "<<x_max<<"x"<<y_max<<"x"<<z_max<<"\n";
  for (uint i = 0; i < masses.size(); ++i) {
    if (active_nodes[i]) {
      file <<i<<" "<<masses[i]<<" " ;
      file<<velocities[i](0)<<" "<<velocities[i](1)<<" "<<velocities[i](2)<<"\n";
    }
  }
}


void Grid::importGrid(std::ifstream & file) {
  std::string line;
  while (getline(file, line)) {
    if (line[0] == '#') {
      INFO(3, "COMMENT "<<line);
    } else {
      std::istringstream s(line);
      uint index;
      FLOAT m;
      VEC3 vel;
      s >> index;
      active_nodes[index] = true;
      s >> m;
      masses[index] = m;
      for (uint i = 0; i < 3; ++i) {
	s >> vel(i);
      }
      velocities[index] = vel;
    }
  }
  
}
