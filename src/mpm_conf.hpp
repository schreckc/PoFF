#ifndef MPM_CONF_HPP
#define MPM_CONF_HPP

#define FLOAT double
#define VEC3 Vector3d
#define VEC4 Vector4d
#define MAT3 Matrix3d
#define MATX MatrixXd
#define ANGLE_AXIS AngleAxisd

#include "Eigen/Core"

namespace mpm_conf {
  extern unsigned int nb_particules_;

  extern Eigen::VEC3 size_grid_; // meter
  extern Eigen::Vector3i size_grid_n_; // nb nodes per side
  extern FLOAT grid_spacing_; // m

  extern Eigen::VEC4 hardenning_param_;

  extern FLOAT mu_, lambda_;
  extern FLOAT young_modulus_, poisson_;
  
  extern FLOAT dt_; // s
  extern Eigen::VEC3 gravity_; // m/s^2

  extern FLOAT density_; //kg/m^3 

  extern FLOAT friction_coef_;

  extern bool display_sphere_;
  extern int replay_speed_;
  
  void loadConf(std::string file);
};

#endif
