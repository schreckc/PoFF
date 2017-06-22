#ifndef MPM_CONF_HPP
#define MPM_CONF_HPP

#include "Eigen/Core"

#define FLOAT double
#define VEC2 Vector2d
#define VEC3 Vector3d
#define VEC4 Vector4d
#define VECX VectorXd
#define MAT2 Matrix2d
#define MAT3 Matrix3d
#define MAT4 Matrix4d
#define MATX MatrixXd
#define ANGLE_AXIS AngleAxisd

using namespace Eigen;

namespace mpm_conf {
  //  extern unsigned int nb_particules_;

  enum method_t {apic_ = 0, 
		 pic_,
		 flip_,
		 mix_,
		 nMethods};

  //Grid
  extern Eigen::VEC3 size_grid_; // meter
  extern Eigen::Vector3i size_grid_n_; // nb nodes per side
  extern FLOAT grid_spacing_; // m
  
  // isotropic elasticity
  extern FLOAT mu_, lambda_;
  extern FLOAT young_modulus_, poisson_;

  //anisotropic elasticity
  extern Eigen::VEC3 young_vec_;
  extern Eigen::VEC3 poisson_vec_;
  extern Eigen::VEC3 shearing_vec_;
  
  extern FLOAT dt_; // s
  extern Eigen::VEC3 gravity_; // m/s^2

  extern FLOAT density_; //kg/m^3 

  extern FLOAT friction_coef_;

  extern bool display_sphere_;
  extern int replay_speed_;

  extern unsigned int plastic_mode_; //0 drucker_prager (sand), 1 clamp eigenvalues (snow), 2 clamp diff between eigenvalues, 3 no plasticity
  extern unsigned int elastic_mode_; //0 sand, 1 snow, 2 linear
  extern bool plastic_anisotropy;
  extern bool elastic_anisotropy;

  // drucker-prager param
  extern Eigen::VEC4 hardenning_param_;
  // plastic_mode 2-3 param
   extern Eigen::VEC3 stretch_max;
  extern Eigen::VEC3 stretch_min;

  // damping
  extern FLOAT damping_;
  extern FLOAT cheat_damping_;
  extern bool smooth_vel_;

  extern uint method_; //0 apic, 1 pic, 2 flip, 3 0.5*pic+0.95*flip

  extern uint export_step_; //export particules every x steps 
  void loadConf(std::string file);
};

#endif
