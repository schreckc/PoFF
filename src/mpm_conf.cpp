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
  VEC3 stretch_max = VEC3(1.0075, 1.0075, 1.0075);
  VEC3 stretch_min = VEC3(0.975, 0.975, 0.975);

  FLOAT young_modulus_ = 3.537e+7;
  FLOAT poisson_ = 0.3;
  FLOAT lambda_ = young_modulus_*poisson_/((1+poisson_)*(1-2*poisson_));
  FLOAT mu_ = young_modulus_/(2*(1+poisson_));

  VEC3 young_vec_(young_modulus_, young_modulus_, young_modulus_);
  VEC3 poisson_vec_(poisson_, poisson_, poisson_); // (nu_23, nu_13, nu_12)
  VEC3 shearing_vec_(young_modulus_/(1+poisson_), young_modulus_/(1+poisson_),young_modulus_/(1+poisson_));// (G_23, G_13, G_12)
  
  FLOAT dt_ = 0.1; 
  VEC3 gravity_ = VEC3(0.0, 0.0, -0.01); 

  FLOAT density_ = 150.0; //sable 1530 kg/m^3
  FLOAT lim_density_ = density_/2.0;

  FLOAT friction_coef_ = 1.0;

  bool display_sphere_ = false;
  int replay_speed_ = 1;

  uint plastic_mode_ = 0;
  uint elastic_mode_ = 0;
  bool plastic_anisotropy_ = true;
  bool elastic_anisotropy_ = true;

  FLOAT damping_ = 0;
  FLOAT cheat_damping_ = 1;
  bool smooth_vel_ = 0;

  uint method_ = apic_;
  bool implicit_ = false;
  
  uint export_step_ = 1;
  
  void loadConf(std::string path_file) {
    std::ifstream file(path_file.c_str());
    bool young_mod_def = false;
     bool young_vec_def = false;
    bool mu_def = false;
    bool shearing_def = false;
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
	  mu_def = true;
	}  else if (line.substr(0,8) == "<lambda>") {
	  std::istringstream s(line.substr(8));
	  s >> lambda_;
	  mu_def = true;
	}  else if (line.substr(0,15) == "<young_modulus>") {
	  std::istringstream s(line.substr(15));
	  s >> young_modulus_;
	  young_mod_def = true;
	}  else if (line.substr(0,19) == "<young_modulus_vec>") {
	  std::istringstream s(line.substr(19));
	  for (uint i = 0; i < 3; ++i) {
	    s >> young_vec_(i);
	  }
	  young_vec_def = true;
	}  else if (line.substr(0,15) == "<poisson_ratio>") {
	  std::istringstream s(line.substr(15));
	  s >> poisson_;
	  young_mod_def = true;
	}  else if (line.substr(0,19) == "<poisson_ratio_vec>") {
	  std::istringstream s(line.substr(19));
	  for (uint i = 0; i < 3; ++i) {
	    s >> poisson_vec_(i);
	  }
	  INFO(3, "poisson "<<poisson_vec_);
	  young_vec_def = true;
	}  else if (line.substr(0,8) == "<mu_vec>") {
	  std::istringstream s(line.substr(8));
	  for (uint i = 0; i < 3; ++i) {
	    s >> shearing_vec_(i);
	  }
	  shearing_def = true;
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
	  s >> plastic_mode_;
	  elastic_mode_ = plastic_mode_;
	  if (elastic_mode_ == 2) {
	    plastic_mode_ = 1;
	  }
	}  else if (line.substr(0,12) == "<plasticity>") {
	  std::istringstream s(line.substr(12));
	  bool p;
	  s >> p;
	  if (!p) {
	    plastic_mode_ = 3;
	  }
	}  else if (line.substr(0,9) == "<damping>") {
	  std::istringstream s(line.substr(9));
	  s >> damping_;
	}  else if (line.substr(0,15) == "<cheat_damping>") {
	  std::istringstream s(line.substr(15));
	  s >> cheat_damping_;
	}  else if (line.substr(0,17) == "<smooth_velocity>") {
	  std::istringstream s(line.substr(17));
	  s >> smooth_vel_;
	}  else if (line.substr(0,8) == "<method>") {
	  std::istringstream s(line.substr(9));
	  std::string method;
	  getline(s, method);
	  if (method.substr(0, 4) == "apic" || method.substr(0, 1) == "0" ) { 
	    method_ = apic_;
	  } else if (method.substr(0, 3) == "pic" || method.substr(0, 1) == "1" ) {
	    method_ = pic_;
	  } else if (method.substr(0, 4) == "flip" || method.substr(0, 1) == "2" ) {
	    method_ = flip_;
	  } else if (method.substr(0, 3) == "mix" || method.substr(0, 1) == "3" ) {
	    method_ = mix_;
	  } else {
	    ERROR(false, "Invalid configuration file \""<<path_file<<"\"", method);
	  }
	  INFO(3, "METHOD "<<method_);
	}  else if (line.substr(0,13) == "<stretch_max>") {
	  std::istringstream s(line.substr(13));
	  for (uint i = 0; i < 3; ++i) {
	    s >> stretch_max(i);
	  }
	}  else if (line.substr(0,13) == "<stretch_min>") {
	  std::istringstream s(line.substr(13));
	  for (uint i = 0; i < 3; ++i) {
	    s >> stretch_min(i);
	  }
	} else {
	  ERROR(false, "Invalid configuration file \""<<path_file<<"\"", line);
	}
      }
      if (young_mod_def) {
	lambda_ = young_modulus_*poisson_/((1+poisson_)*(1-2*poisson_));
	mu_ = young_modulus_/(2*(1+poisson_));
	WARNING(!mu_def, "both Lame parameters and Young's modulus and/or Poisson ratio defined: using Young's modulus and Poisson ratio values",
		young_modulus_<<" "<<poisson_<<" "<<mu_<<" "<<lambda_);
      }
      if ((young_mod_def || mu_def) && !young_vec_def) {
	   young_vec_ = VEC3(young_modulus_, young_modulus_, young_modulus_);
	   poisson_vec_ = VEC3(poisson_, poisson_, poisson_); // (nu_23, nu_13, nu_12)
      }
      if (!shearing_def) {
	shearing_vec_ = VEC3(young_vec_(0)/(1+poisson_vec_(0)), young_vec_(1)/(1+poisson_vec_(1)),young_vec_(2)/(1+poisson_vec_(2)));// (G_23, G_13, G_12)
      }
    } else {
      ERROR(false, "Cannot found file \""<<path_file<<"\"", "");
    }
    file.close();
  }
    
}
