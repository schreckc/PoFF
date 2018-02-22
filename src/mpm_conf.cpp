#include "mpm_conf.hpp"
#include <fstream>
#include "error.hpp"
#include "Tensor.hpp"
#include "utils.hpp"

using namespace Eigen;

namespace mpm_conf {
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
  
  FLOAT dt_ = 0.01; 
  VEC3 gravity_ = VEC3(0.0, 0.0, -9.81); 

  FLOAT density_ = 150.0; //sable 1530 kg/m^3
  FLOAT lim_density_ = density_/2.0;

  FLOAT friction_coef_ = 1.0;

  bool display_sphere_ = false;
  int replay_speed_ = 1;

  uint plastic_mode_ = 0;
  uint elastic_mode_ = 0;
  bool anisotropy_on = false;
  
  FLOAT damping_ = 0;
  FLOAT cheat_damping_ = 1;
  bool smooth_vel_ = 0;

  VEC3 anisotropy_values_(1, 1, 1);
  Tensor anisotropy_stress_;
  Tensor inv_anisotropy_stress_;
  Tensor anisotropy_strain_;
  Tensor inv_anisotropy_strain_;

  MATX tangent_stiffness(6, 6);
  MATX inverse_tangent_stiffness(6, 6);
  MATX tangent_stiffness_iso(6, 6);
  MATX inverse_tangent_stiffness_iso(6, 6);

  uint method_ = apic_;
  bool implicit_ = false;
  
  uint export_step_ = 1;
  
  void loadConf(std::string path_file) {
    std::ifstream file(path_file.c_str());
    bool young_mod_def = false;
    bool young_vec_def = false;
    bool mu_def = false;
    bool shearing_def = false;
    if (file.good()) {
      std::string line;
      while (getline(file, line)) {
	if (line.substr(0,1) == "#") {
	  INFO(2, "COMMENT: "<<line);
	} else if (line.substr(0,11) == "<grid_size>") {
	  std::istringstream s(line.substr(11));
	  for (uint i = 0; i < 3; ++i) {
	    s >> size_grid_(i);
	  }
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
	  young_modulus_ =  young_vec_(0);
	  young_vec_def = true;
	  young_mod_def = true;
	}  else if (line.substr(0,15) == "<poisson_ratio>") {
	  std::istringstream s(line.substr(15));
	  s >> poisson_;
	  young_mod_def = true;
	}  else if (line.substr(0,19) == "<poisson_ratio_vec>") {
	  std::istringstream s(line.substr(19));
	  for (uint i = 0; i < 3; ++i) {
	    s >> poisson_vec_(i);
	  }
	  poisson_ = poisson_vec_(0);
	  INFO(3, "poisson "<<poisson_vec_);
	  young_vec_def = true;
	  young_mod_def = true;
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
	}  else if (line.substr(0,14) == "<elastic_mode>") {
	  std::istringstream s(line.substr(14));
	  s >> elastic_mode_;
	}  else if (line.substr(0,14) == "<plastic_mode>") {
	  std::istringstream s(line.substr(14));
	  s >> plastic_mode_;
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
	}  else if (line.substr(0,11) == "<implicite>") {
	  implicit_ = true;
	  INFO(3, "IMPLICIT");
	}  else if (line.substr(0,11) == "<explicite>") {
	  implicit_ = false;
	}  else if (line.substr(0,12) == "<anisotropy>") {
	  std::istringstream s(line.substr(12));
	  for (uint i = 0; i < 3; ++i) {
	    s >> anisotropy_values_(i);
	    anisotropy_stress_(i, i, i, i) = anisotropy_values_(i);
	  }
	  anisotropy_on = true;
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
	shearing_vec_ = 0.5*VEC3(young_vec_(0)/(1+poisson_vec_(0)), young_vec_(1)/(1+poisson_vec_(1)),young_vec_(2)/(1+poisson_vec_(2)));// (G_23, G_13, G_12)
      }
    } else {
      ERROR(false, "Cannot found file \""<<path_file<<"\"", "");
    }
    file.close();
    INFO(3, "plasticity: "<<plastic_mode_);



    
    // constitutive elasticity tensor
    FLOAT E1 = young_vec_(0);
    FLOAT E2 = young_vec_(1);
    FLOAT E3 = young_vec_(2);
    FLOAT nu12 = poisson_vec_(2);
    FLOAT nu13 = poisson_vec_(1);
    FLOAT nu23 = poisson_vec_(0);

    INFO(3, "YOUNG "<<E1<<" "<<E2<<" "<<E3);
    INFO(3, "poisson "<<nu23<<" "<<nu13<<" "<<nu12);

    FLOAT E = young_modulus_;
    FLOAT nu = poisson_;
    FLOAT G = E/(2*(1+nu));
  
    // in the isotropic space
    for (uint i = 0; i < 3; ++i) {
      inverse_tangent_stiffness_iso(i, i) = 1/E;
      inverse_tangent_stiffness_iso((i+1)%3, i) = -nu/E;
      inverse_tangent_stiffness_iso((i+2)%3, i) = -nu/E;
      inverse_tangent_stiffness_iso(i+3, i+3) = 1/(2*G);
    }
 
    MAT3 inv_stretch_stiff_iso;
    inv_stretch_stiff_iso << 1/E, -nu/E, -nu/E,
      -nu/E, 1/E, -nu/E,
      -nu/E, -nu/E, 1/E;
    MAT3 stretch_stiff_iso = inv_stretch_stiff_iso.inverse();

    for (uint i = 0; i < 3; ++i) {
      for (uint j = 0; j < 3; ++j) {
	if (fabs(stretch_stiff_iso(i, j)) > 1e-15) {
	  tangent_stiffness_iso(i, j) = stretch_stiff_iso(i, j);
	} else {
	  tangent_stiffness_iso(i, j) = 0;
	}
	tangent_stiffness_iso(i, 3+j) = 0;
	tangent_stiffness_iso(3+i, 3+j) = 0;
	tangent_stiffness_iso(3+i, j) = 0;
      }
    }
    tangent_stiffness_iso(3, 3) = 2*G;
    tangent_stiffness_iso(4, 4) = 2*G;
    tangent_stiffness_iso(5, 5) = 2*G;


    // in the anisostropic space
    for (uint i = 0; i < 3; ++i) {
      inverse_tangent_stiffness(i, i) = 1/young_vec_(i);
      inverse_tangent_stiffness(i+3, i+3) = 1/(2*shearing_vec_(i));
    }
    inverse_tangent_stiffness(0, 1) = -nu12/E1;
    inverse_tangent_stiffness(0, 2) = -nu13/E1;
    inverse_tangent_stiffness(1, 0) = -nu12/E1;
    inverse_tangent_stiffness(1, 2) = -nu23/E2;
    inverse_tangent_stiffness(2, 0) = -nu13/E1;
    inverse_tangent_stiffness(2, 1) = -nu23/E2;

 
    MAT3 inv_stretch_stiff;
    inv_stretch_stiff << 1/E1, -nu12/E1, -nu13/E1,
      -nu12/E1, 1/E2, -nu23/E2,
      -nu13/E1, -nu23/E2, 1/E3;
    MAT3 stretch_stiff = inv_stretch_stiff.inverse();
  
    for (uint i = 0; i < 3; ++i) {
      for (uint j = 0; j < 3; ++j) {
	if (fabs(stretch_stiff(i, j)) > 1e-15) {
	  tangent_stiffness(i, j) = stretch_stiff(i, j);
	} else {
	  tangent_stiffness(i, j) = 0;
	}
	tangent_stiffness(i, 3+j) = 0;
	tangent_stiffness(3+i, 3+j) = 0;
	tangent_stiffness(3+i, j) = 0;
      }
    }
    tangent_stiffness(3, 3) = 2*shearing_vec_(0); //2*G23
    tangent_stiffness(4, 4) = 2*shearing_vec_(1);;//2*G13;
    tangent_stiffness(5, 5) = 2*shearing_vec_(2);//2*G12;

    // map from anisotropic to isotropc space
    for (uint i = 0; i < 3; ++i) {
      for (uint j = 0; j < 3; ++j) {
	anisotropy_stress_(i, j, i, j) = 0.5*anisotropy_values_(i)*anisotropy_values_(j);
	anisotropy_stress_(j, i, i, j) = 0.5*anisotropy_values_(i)*anisotropy_values_(j);
	inv_anisotropy_stress_(i, j, i, j) = 0.5/anisotropy_values_(i)/anisotropy_values_(j);
	inv_anisotropy_stress_(i, j, j, i) = 0.5/anisotropy_values_(i)/anisotropy_values_(j);
      }
      anisotropy_stress_(i, i, i, i) = anisotropy_values_(i)*anisotropy_values_(i);
      inv_anisotropy_stress_(i, i, i, i) = 1.0/anisotropy_values_(i)/anisotropy_values_(i);

    }
    Tensor C_iso = mat2TensorOrtho(tangent_stiffness_iso);
    Tensor inv_C_iso = mat2TensorOrtho(inverse_tangent_stiffness_iso);
    Tensor C = mat2TensorOrtho(tangent_stiffness);
    Tensor inv_C = mat2TensorOrtho(inverse_tangent_stiffness);
   
    Tensor aux = innerProduct(anisotropy_stress_, C);
    anisotropy_strain_ = innerProduct(inv_C_iso, aux);
   
    aux = innerProduct(inv_anisotropy_stress_, C_iso);
    inv_anisotropy_strain_ = innerProduct(inv_C, aux);
    
    INFO(2, "stress anisotropy map"<< anisotropy_stress_);
    INFO(2, "strain anisotropy map"<< anisotropy_strain_);
  }

  
  
} //end namespace mpm_conf
