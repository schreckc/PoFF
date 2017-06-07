#include "Grid.hpp"

//#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <omp.h>

#include "Scene.hpp"
#include "Simulation.hpp"
#include "Sphere.hpp"
#include "error.hpp"
#include "mpm_conf.hpp"

inline uint Grid::index(uint i, uint j, uint k) const {
  return i*(j_max+1)*(k_max+1) + j*(k_max+1) + k;
}

Grid::Grid() : Object(), x_max(0),  y_max(0), z_max(0),  i_max(0),  j_max(0),  k_max(0) {
}

Grid::Grid(FLOAT width, FLOAT depth, FLOAT height, FLOAT space_step, int shader): Object(shader) {
  // assert(m_shader != -1);
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
  std::cout<<"i max "<< i_max<<" "<<j_max<<" "<<k_max<<std::endl;
  // std::cout<<"x max "<< x_max<<" "<<y_max<<" "<<z_max<<std::endl;
  
  masses = std::vector<FLOAT>(nb_nodes);
  active_nodes = std::vector<bool>(nb_nodes);
  velocities = std::vector<VEC3>(nb_nodes);
  inter_velocities = std::vector<VEC3>(nb_nodes);
  prev_velocities = std::vector<VEC3>(nb_nodes);
  positions = std::vector<VEC3>(nb_nodes);
  new_positions = std::vector<VEC3>(nb_nodes);
   cells = std::vector<std::list<Particule*> >(nb_cells);
  distance_collision = std::vector<FLOAT>(nb_nodes);

 for (uint i = 0; i <= i_max; ++i) {
    for (uint j = 0; j <= j_max; ++j) {
      for (uint k = 0; k <= k_max; ++k) {
	//std::cout<<index(i, j, k)<<std::endl;
	positions[index(i, j, k)] = VEC3(i*mpm_conf::grid_spacing_, j*mpm_conf::grid_spacing_, k*mpm_conf::grid_spacing_);
	velocities[index(i, j, k)] = VEC3(0, 0, 0);
      }
    }
  }
  
#ifndef NO_GRAPHICS_ 
  vertices = new GLfloat[nb_lines*6];
  colors = new GLfloat[nb_lines*6];
 
  //  std::cout<<"size "<<((i_max+1)*(j_max+1) + (k_max+1)*(j_max+1) +(k_max+1)*(i_max+1))*6<<std::endl;
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
      //       std::cout<<p1<<"\n"<<std::endl;
      // std::cout<<p2<<"\n\n\n"<<std::endl;

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
  
  //  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  //  glEnableVertexAttribArray(0);

  //  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, colors);
  //  glEnableVertexAttribArray(1);

  //  glDrawArrays(GL_LINES, 0, nb_lines*2);

  //  glDisableVertexAttribArray(0);
  //  glDisableVertexAttribArray(1);

  //  disableShader();

    glLineWidth(1.0f);
  for (uint i = 0; i <= i_max; ++i) {
     for (uint j = 0; j <= j_max; ++j) {
       for (uint k = 0; k <= k_max; ++k) {
  	 uint ind = index(i, j, k);
  	if (active_nodes[ind]) {
  	// //INFO(3, masses[ind]);
  	 // Sphere sp(0.01*masses[ind], Scene::SCENE->getShader(0));
  	 //  sp.setColor(0, 1, 0);
  	 //  if (active_nodes[ind]) {
  	 //    sp.setColor(1, 0, 0);
  	 //  }
  	 VEC3 pos = positions[ind];
  	 VEC3 vel = mpm_conf::dt_ * velocities[ind]+pos; 
  	 // glm::mat4 model = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)));
  	 // glm::mat4 cur_model = model * m_model_view;
  	 // 	 sp.draw(cur_model, Scene::SCENE->getShader(0));
  	   //   INFO(3, "vel "<<vel(0)<<", "<<vel(1)<<", "<<vel(2));
  	 //   //assert(false);
  	 // }
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
  // for (auto & elem : masses) {
  //   elem = 0;
  // }
  // for (auto && elem : active_nodes) {
  //   elem = false;
  // }
  // for (auto & elem : velocities) {
  //   elem = VEC3(0, 0, 0);
  // }
  // for (auto & elem : inter_velocities) {
  //   elem = VEC3(0, 0, 0);
  // }
  //  INFO(3, "next step");
#pragma omp parallel for
  for (uint i = 0; i < nb_nodes; ++i) {
    masses[i] = 0;
    active_nodes[i] = false;
    inter_velocities[i] = VEC3(0, 0, 0);
    prev_velocities[i] = velocities[i]; 
    velocities[i] = VEC3(0, 0, 0);
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
		      // if (active_nodes[ind]) {
		      // 	if (inter_velocities[ind_neigh](0) != 0) {
		      // 	  INFO(3,"laplace coef\n "<<inter_velocities[ind_neigh]);
		      // 	}
		      // }
		    }
		  }

		}
	      }
	    }
	  }
	  velocities[ind] += 1*sv;
	  // if (active_nodes[ind]) {
	  //   INFO(3, "lapl vel\n"<<sv);
	  // }
	}

      }
    }
  }
	
}

void Grid::particulesToGrid(std::vector<Particule*> & particules) {
  for (auto &p : particules) {
    Vector3i c = p->getCell();
    // INFO(3, c);
    // INFO(3, p->getPosition());
    uint ind = c(0)*j_max*k_max + c(1)*(k_max) + c(2);
    // INFO(3,"ind set "<<ind);
    // assert(ind < nb_nodes);
    // assert(ind < nb_cells);
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
  //  INFO(2, "particules per cell : "<<((FLOAT)particules.size())/(FLOAT)nb_ac<<" / active cells: "<<nb_ac<<" / particules: "<<particules.size());
  
  // for (uint i = 0; i < nb_cells; ++i) {
  //   INFO(3, "part per cell "<<i<<" "<<cells[i].size());
  //  }
  
  //   INFO(2, "Part 2 Grid");
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
		      // for (auto& p : particules) {
		      // INFO(3,"lmn "<<indc<<" "<<l<<" "<<m<<" "<<n);
		      // INFO(3, "part per cell "<<cells[index(l, m, n)].size());
		      FLOAT w = p->weight(Vector3i(i, j, k));
		      if (w > 0) {
			active_nodes[ind] = true;
		  
			//INFO(3, "weight "<<w);
			masses[ind] += w*p->getMass();
			if (mpm_conf::method_ == mpm_conf::apic_) {
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
	  
	  //     for (auto& p : particules) {
	  //       f += p->getForceIncrement()*p->gradWeight(Vector3i(i, j, k));
	  // //      // if (p->gradWeight(Vector3i(i, j, k)).norm() > 0.001) {
	  // //      //   INFO(3, "grad "<<i<<" "<<j<<" "<<k<<"\n"<<p->gradWeight(Vector3i(i, j, k)));
	  //        }

	  //     }
	  //inter_velocities[ind] = velocities[ind]/ masses[ind];
	  //	  INFO(3,"masses " << masses[ind]<<" "<<f.norm()/masses[ind]);
	   IS_DEF(velocities[ind](0));
	  if (masses[ind] > 1e-8*mpm_conf::dt_) {
	    //  IS_DEF(f(0));
	    if (std::isnan(f(0)) || std::isinf(f(0))) {
	      f = VEC3(0, 0, 0);
	    }
	    velocities[ind] -= /*mpm_conf::cheat_damping_*/mpm_conf::dt_*f + mpm_conf::dt_*mpm_conf::damping_*velocities[ind];
	    velocities[ind] /= masses[ind];
	  } else {
	    velocities[ind] = VEC3(0, 0, 0);
	  }
	  IS_DEF(velocities[ind](0));
	  velocities[ind] += mpm_conf::dt_*mpm_conf::gravity_;
	  inter_velocities[ind] = velocities[ind];//particules.front()->gradWeight(Vector3i(i, j, k));
	  new_positions[ind] = positions[ind] + mpm_conf::dt_*velocities[ind];
		  //	  inter_velocities[ind] =  velocities[ind];//particules.front()->gradWeight(Vector3i(i, j, k));
	  //	  INFO(3, "vel\n" << velocities[ind]);
	  //	  INFO(3, "density "<<masses[ind]/pow(mpm_conf::grid_spacing_, 3));
	   IS_DEF(velocities[ind](0));
	}
	// } else {
	//   velocities[ind] = VEC3(0, 0, 0);
      }
    }
  }
  //INFO(2, "END Part 2 Grid");
}

void Grid::gridToParticules(std::vector<Particule*> & particules) {
  // INFO(2, "Grid 2 Part");
    FLOAT s3 = pow(mpm_conf::grid_spacing_, 3);
#pragma omp parallel for
  //for (auto& p : particules) {
  for (uint ip = 0; ip < particules.size(); ++ip) {
    Particule *p = particules[ip];
    Vector3i cell = p->getCell();
    VEC3 vel(0, 0, 0);
    VEC3 vel_flip = p->getVelocity();
    // INFO(3, "vel FLIP \n"<<vel_flip);
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
		  pos += w*(positions[ind] + mpm_conf::dt_*velocities[ind]); //mult by w twice ?
		  T += mpm_conf::cheat_damping_*velocities[ind]*p->gradWeight(Vector3i(i, j, k)).transpose();
		  //	INFO(3, "vel g2p \n"<<prev_velocities[ind]);
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
    if (mpm_conf::method_ == mpm_conf::apic_ || mpm_conf::method_ == mpm_conf::pic_) {
      p->update(pos, vel, B, T);
    } else if (mpm_conf::method_ == mpm_conf::flip_) {
      p->update(pos, vel_flip, B, T); 
    } else if  (mpm_conf::method_ == mpm_conf::mix_) {
      FLOAT alpha = 0.95;
      VEC3 v = alpha*vel_flip + (1-alpha)*vel;
      p->update(pos, v, B, T);
    }

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
		  A += w * (new_positions[ind] - p->getPosition())*(positions[ind] - prev_pos).transpose();
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
    
  }
  // INFO(2, "END Grid 2 Part");
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
    FLOAT density = mpm_conf::density_;
    // FLOAT density = 0;//mpm_conf::density_;;
    // Vector3i cell = p->getCell();
    // for (int i = cell(0) - 2; i <= cell(0) + 2; ++i) {
    //   if (i >= 0 && i <= (int)i_max) {
    // 	for (int j = cell(1) - 2; j <= cell(1) + 2; ++j) {
    // 	  if (j >= 0 && j <= (int)j_max) {
    // 	    for (int k = cell(2) - 2; k <= cell(2) + 2; ++k) {
    // 	      if (k >= 0 && k <= (int)k_max) {
    // 		uint ind = index(i, j, k);
    // 		FLOAT w = p->weight(Vector3i(i, j, k));
    // 		density += masses[ind]*w;
    // 	      }
    // 	    }
    // 	  }
    // 	}
    //   }
    // }
    // density /= s3;
    //INFO(3,"density "<<mpm_conf::density_<<"   denstity local "<<density);
 
    p->initVolume(density);
  }
  double masse = 0;
  for (uint ip = 0; ip < particules.size(); ++ip) {
    Particule *p = particules[ip];
    masse += p->getMass();
  }
  INFO(2, "END Init Volume "<<masse );
}

void Grid::initCollision(Obstacle *ob) {
  #pragma omp parallel for
  for (uint i = 0; i < nb_nodes; ++i) {
    FLOAT d = ob->distance(positions[i]);
    if (fabs(d) < fabs( distance_collision[i])) {
      distance_collision[i] = d;
    }
  }
}

void Grid::collision(Obstacle *ob) {
  //  INFO(2, "Collision");
#pragma omp parallel for
  for (uint i = 0; i < nb_nodes; ++i) {
    if (active_nodes[i]) {
      VEC3 pos = positions[i] + mpm_conf::dt_*velocities[i];
      VEC3 n;// = ob->getNormal(pos);
      FLOAT d;// = ob->distance(pos);
      ob->getCollisionValues(pos, d, n);
      //FLOAT dcomp = d - std::min(ob->distance(positions[i]), (FLOAT)0.0);
      FLOAT dcomp = d - std::min(distance_collision[i],(FLOAT)0.0);
      
      //   distance_collision[i] = ob->distance(positions[i]);
      if (fabs(d) <= 2*mpm_conf::grid_spacing_ && dcomp < 0) {
	VEC3 vel_prev = velocities[i];
	//INFO(3,"vel prev "<< velocities[i](0)<<" "<< velocities[i](1)<<" "<< velocities[i](2));
	//	VEC3 n = ob->getNormal(pos);
	FLOAT dv = -dcomp/mpm_conf::dt_;
	velocities[i] += dv*n;//dcomp*ob->getNormal(pos)/Simulation::dt;
	//INFO(3,"vel new\n"<< velocities[i]);
	
	//friction
	VEC3 vt = velocities[i] - velocities[i].dot(n)*n;
	FLOAT nvt = vt.norm();
	//INFO(3, "nvt dv" <<nvt<<" "<<dv);
	
	if (nvt > ob->getFriction()*dv) {
	  velocities[i] -= ob->getFriction()*dv*vt/nvt;
	} else {
	  velocities[i] -= vt;
	}
	// INFO(3,"vel new "<< velocities[i](0)<<" "<< velocities[i](1)<<" "<< velocities[i](2));
	// INFO(3,"vel norm"<< velocities[i].norm()<<" "<<vel_prev.norm());
	//	TEST(velocities[i].norm() <= vel_prev.norm()); 
      }
    }
    // FLOAT d = ob->distance(positions[i]);
    // if (fabs(d) < fabs( distance_collision[i])) {
    // //if (d < distance_collision[i]) {
    //   distance_collision[i] = d;
    // }
  }
  //  INFO(2, "END Collision");
}
