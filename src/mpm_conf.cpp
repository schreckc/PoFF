#include "mpm_conf.hpp"
#include <fstream>
#include "error.hpp"

using namespace Eigen;

namespace mpm_conf {
  //  unsigned int nb_particules_ = 1000;

  VEC3 size_grid_ = VEC3(1.0, 1.0, 1.0);
  Vector3i size_grid_n_ = Vector3i(200, 200, 200); 
  FLOAT grid_spacing_ = 0.01; 

  VEC4 hardenning_param_ = VEC4(35.0, 0.0, 0.02, 10.0);

  FLOAT young_modulus_ = 3.537e+7;
  FLOAT poisson_ = 0.3;
  FLOAT mu_ = young_modulus_*poisson_/((1+poisson_)*(1-2*poisson_));
  FLOAT lambda_ = young_modulus_/(2*(1+poisson_)); 
  
  FLOAT dt_ = 0.1; 
  VEC3 gravity_ = VEC3(0.0, 0.0, -0.01); 

  FLOAT density_ = 150.0; //sable 1530 kg/m^3

  FLOAT friction_coef_ = 1.0;

  bool display_sphere_ = false;
  int replay_speed_ = 1;

  uint mode_ = 0;
  bool plasticity_ = true;

  FLOAT damping_ = 1;
  
  void loadConf(std::string path_file) {
    std::ifstream file(path_file.c_str());
    bool young_mod_def = false;
    bool mu_def = false;
    // bool size_grid_def = false;
    // bool size_grid_def_n = false;
    if (file.good()) {
      std::string line;
      while (getline(file, line)) {
	if (line.substr(0,1) == "#") {
	  INFO(2, line);
	} else if (line.substr(0,9) == "<nb_part>") {
	  // std::istringstream s(line.substr(9));
	  // s >> nb_particules_;
	} else if (line.substr(0,11) == "<grid_size>") {
	  std::istringstream s(line.substr(11));
	  for (uint i = 0; i < 3; ++i) {
	    s >> size_grid_(i);
	  }
	  // size_grid_def = true;
	// } else if (line.substr(0,13) == "<grid_size_n>") {
	//   std::istringstream s(line.substr(13));
	//   for (uint i = 0; i < 3; ++i) {
	//     s >> size_grid_n_(i);
	//   }
	//   size_grid_def_n = true;
	}  else if (line.substr(0,14) == "<grid_spacing>") {
	  std::istringstream s(line.substr(14));
	  s >> grid_spacing_;
	}  else if (line.substr(0,4) == "<mu>") {
	  std::istringstream s(line.substr(4));
	  s >> mu_;
	}  else if (line.substr(0,8) == "<lambda>") {
	  std::istringstream s(line.substr(8));
	  s >> lambda_;
	}  else if (line.substr(0,15) == "<young_modulus>") {
	  std::istringstream s(line.substr(15));
	  s >> young_modulus_;
	  young_mod_def = true;
	}  else if (line.substr(0,15) == "<poisson_ratio>") {
	  std::istringstream s(line.substr(15));
	  s >> poisson_;
	  young_mod_def = true;
	}  else if (line.substr(0,11) == "<time_step>") {
	  std::istringstream s(line.substr(11));
	  s >> dt_;
	}  else if (line.substr(0,9) == "<gravity>") {
	  std::istringstream s(line.substr(9));
	  for (uint i = 0; i < 3; ++i) {
	    s >> gravity_(i);
	  }
	}  else if (line.substr(0,9) == "<density>") {
	  std::istringstream s(line.substr(9));
	  s >> density_;
	} else if (line.substr(0,12) == "<hardenning>") {
	  std::istringstream s(line.substr(12));
	  for (uint i = 0; i < 4; ++i) {
	    s >> hardenning_param_(i);
	  }
	}  else if (line.substr(0,10) == "<friction>") {
	  std::istringstream s(line.substr(10));
	  s >> friction_coef_;
	}  else if (line.substr(0,16) == "<display_sphere>") {
	  std::istringstream s(line.substr(16));
	  s >> display_sphere_;
	}  else if (line.substr(0,6) == "<mode>") {
	  std::istringstream s(line.substr(6));
	  s >> mode_;
	}  else if (line.substr(0,12) == "<plasticity>") {
	  std::istringstream s(line.substr(12));
	  s >> plasticity_;
	}  else if (line.substr(0,9) == "<damping>") {
	  std::istringstream s(line.substr(9));
	  s >> damping_;
	} else {
	  ERROR(false, "Invalid configuration file \""<<path_file<<"\"", line);
	}
      }
      if (young_mod_def) {
	mu_ = young_modulus_*poisson_/((1+poisson_)*(1-2*poisson_));
	lambda_ = young_modulus_/(2*(1+poisson_));
	WARNING(mu_def, "both Lame parameters and Young's modulus and/or Poisson ratio defined: using Young's modulus and Poisson ratio values",
		young_modulus_<<" "<<poisson_<<" "<<mu_<<" "<<lambda_);
      }
      	
    } else {
      ERROR(false, "Cannot found file \""<<path_file<<"\"", "");
    }
    file.close();
  }
    
}
