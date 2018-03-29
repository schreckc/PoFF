#include "Particule.hpp"
#include "Sphere.hpp"
#include "Scene.hpp"
#include "Grid.hpp"
#include "Simulation.hpp"
#include "error.hpp"
#include "Tensor.hpp"
#include "Cube.hpp"
#include "utils.hpp"

#include <iostream>

Particule::Particule(int shader) : Object(shader) {
  pos = VEC3(0, 0, 0);
}

Particule::Particule(FLOAT mass, FLOAT vol, VEC3 p, VEC3 n, VEC3 velo, int shader) :
  Object(shader), m(mass), v0(vol), pos(p), normal(n), vel(velo), /*F_p(MAT3::Identity(3, 3)),*/ F_e(MAT3::Identity(3, 3))/*, F(MAT3::Identity(3, 3))*/ {
  
  FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(p(0)/h), (int)(p(1)/h), (int)(p(2)/h));
  B = MAT3::Zero();
  forceIncrement = MAT3::Zero();
  hardenning = 0;
  FLOAT fric_angle = mpm_conf::hardenning_param_(0) + mpm_conf::hardenning_param_(3);
  fric_angle = fric_angle*M_PI/180; 
  alpha = sqrt(2.0/3.0)*(2*sin(fric_angle))/(3-sin(fric_angle));

  if (normal.squaredNorm() == 0) {
    normal = VEC3((FLOAT)rand()/(FLOAT)RAND_MAX - 0.5, (FLOAT)rand()/(FLOAT)RAND_MAX - 0.5, (FLOAT)rand()/(FLOAT)RAND_MAX - 0.5);
    normal.normalize();
  }

  mix_rot = MAT3::Identity();

  valx = 1;
  valy = 1;
  valz = 1;

  rotation << 1, 0, 0,
    0, 1, 0,
    0, 0, 1;

  color = VEC3(1, 1, 1);

  orientation << 0.05, 0, 0,
    0, 0.01, 0,
    0, 0, 0.94;
      
  anisotropy_strain = mpm_conf::anisotropy_strain_;
  inv_anisotropy_strain = mpm_conf::inv_anisotropy_strain_;
  anisotropy_stress = mpm_conf::anisotropy_stress_;
  inv_anisotropy_stress = mpm_conf::inv_anisotropy_stress_;
  
  valx = 0.1;
  valy = 0.1;
  valz = 1;

  out = false;
}


void Particule::animate() {
}

#ifndef NO_GRAPHICS_ 
void Particule::draw(glm::mat4 m, int s) {
  /* draw spheres */
  m_model_view = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)));
  if (mpm_conf::display_sphere_) {

    glm::mat4 cur_model = m * m_model_view;
    
    VEC3 x = rotation.col(0);
    VEC3 y = rotation.col(1);
    VEC3 z = rotation.col(2);


    
      glm::mat3 R;
      R[0] = glm::vec3(x[0], x[1], x[2]);
      R[1] = glm::vec3(y[0], y[1], y[2]);
      R[2] = glm::vec3(z[0], z[1], z[2]);


      
      //valx = 0.08; valy = 0.08;  valz = 1.0;
       valx = 0.5; valy = 0.5;  valz = 0.5;
      //valx = 0.1; valy = 0.1;  valz = 1.0;
      //valx = 0.5; valy = 0.5;  valz = 0.05;
      //valx = 0.2; valy = 0.2;  valz = 0.02;
      glm::mat3 D;
      D[0] = glm::vec3(valx, 0, 0);
      D[1] = glm::vec3(0, valy, 0);
      D[2] = glm::vec3(0, 0, valz);

      glm::mat3 S = R*D;
      //glm::mat3 S = R;
      glm::mat4 S4 = glm::mat4(S);
       cur_model = cur_model * S4;
  
    
      Sphere sp(0.01, m_shader);
      sp.setColor(color(0), color(1), color(2));
      if (out) {
	sp.setColor(0, 0, 0);
      }
      sp.setColor(1, 1, 1);
      int cur_shader = m_shader;
      if (m_shader == -1) {
        cur_shader = s;
      }
   
      sp.draw(cur_model, cur_shader);


  //   // leaf
  //   m_shader = 1;
  //   m_texture = 1;
  //   m_model_view = cur_model;
    
    
  //   float vertices[18] = {-0.0825, -0.1155, 0,   0.0825, -0.1155, 0,   0.0825, 0.1155, 0, 
  // 			  -0.0825, -0.1155, 0,   -0.0825, 0.1155, 0,   0.0825, 0.1155, 0};
  //   float coordTexture[12] =  {0, 0,   1, 0,   1, 1,
  //    			     1, 1,   0, 1,   0, 0};
  //   float normals[18] =  {0.0, 0.0, -1.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0, 
  // 			  0.0, 0.0, -1.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0};

  //   float couleurs[18] = {1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,
  // 			  1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0};  
    
  //   enableShader();
  //   setMVP(m, s);
  //   // glm::mat4 vp;
  //   // Scene::SCENE->getProjView(vp);
  //   // glm::mat4 mvp = vp * m_model_view;
  //   //   glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));
      
  //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  //   glEnableVertexAttribArray(0);

  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, couleurs);
  // glEnableVertexAttribArray(1);

  // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, normals);
  // glEnableVertexAttribArray(2);

  //  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, coordTexture);
  //  glEnableVertexAttribArray(3);

  // glDrawArrays(GL_TRIANGLES, 0, 6);

  // glDisableVertexAttribArray(3);
  // glDisableVertexAttribArray(2);
  // glDisableVertexAttribArray(1);
  // glDisableVertexAttribArray(0);

  // disableShader();

    
  } else {
  // /* draw points */
  GLfloat vertex[6] = {0, 0, 0};//{pos(0), pos(1), pos(2)};
  GLfloat color[6] = {0, 0, 0};
  GLfloat normal[6] = {1, 0, 0};
  //  glPointSize(10.0f);
  glLineWidth(50.0f);

  enableShader();
  setMVP(m, s);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertex);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, color);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_POINTS, 0, 1);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  disableShader();
  }
  
  if (Scene::SCENE->running) {
    color = VEC3(1, 1, 1);
  } else {
    color = VEC3(0.5, 1, 0.5);
  }
}
#endif


VEC3 Particule::getPosition() const {
  return pos;
}

void Particule::setPosition(VEC3 p) {
  pos = p;
}

VEC3 Particule::getVelocity() const {
  return vel;
}

void Particule::setVelocity(VEC3 velo) {
  vel = velo;
}

FLOAT Particule::getMass() const {
  return m;
}

void Particule::setMass(FLOAT mass) {
  m = mass;
}

FLOAT Particule::getVolume() const {
  return v0;
}

void Particule::setVolume(FLOAT v) {
  v0 = v;
}
  

FLOAT Particule::getDensity() const {
  return density;
}

void Particule::setDensity(FLOAT d) {
  density = d;
}


FLOAT Particule::getVolumeCorrection() const {
  return vp;
}

void Particule::setVolumeCorrection(FLOAT corr) {
  vp = corr;
}

const MAT3 & Particule::getB() const {
  return B;
}

void Particule::setB(const MAT3 & b) {
  B = b;
}

Vector3i Particule::getCell() const {
  return cell;
}

bool Particule::isOut() const {
  return out;
}
void Particule::setOut(bool is_out) {
  out = is_out;
}


void Particule::setColor(FLOAT r, FLOAT g, FLOAT b) {
  color = VEC3(r, g, b);
}

MAT3 Particule::getDeformationElastic() const {
  return F_e;
}

void Particule::addDeformationElastic(MAT3 Ddef) { //F_e^n+1 = (I + Ddef)F_e^n
  F_e = (MAT3::Identity() + Ddef)*F_e;
}

void Particule::setDeformationElastic(MAT3 F) {
  F_e = F;
}

// MAT3 Particule::getDeformationPlastic() const {
//   return F_p;
// }

// void Particule::setDeformationPlastic(MAT3 F) {
//   F_p = F;
// }

// MAT3 Particule::getDeformation() const {
//   return F_e*F_p;
// }

const MAT3& Particule::getForceIncrement() const {
  IS_DEF(forceIncrement(0, 0));
  return forceIncrement;
}

const Tensor& Particule::getSecondEnergyDer() const {
  return energy_second_der;
}

FLOAT Particule::weight(Vector3i node) {
  FLOAT w = 1;
  FLOAT absx = 0;
  for (uint i = 0; i < 3; ++i) {
    if (cell(i) == node(i) || cell(i) == node(i) - 1) {
      absx = fabs(pos(i)/mpm_conf::grid_spacing_ - (FLOAT)node(i));
      w *= 0.5*pow(absx, 3) - pow(absx, 2) + 2.0/3.0;
    } else if (cell(i) == node(i) + 1 || cell(i) == node(i) - 2) {
      absx = fabs(pos(i)/mpm_conf::grid_spacing_ - (FLOAT)node(i));
      w *= 1.0/6.0*pow(2 - absx, 3);
    } else {
      w = 0;
      break;
    }
  }
  return w;
}

VEC3 Particule::gradWeight(Vector3i node) {
  VEC3 n(0, 0, 0);
  VEC3 gradn(0, 0, 0);

  VEC3 u;
  for (uint i = 0; i < 3; ++i) {
    u(i) = pos(i) - mpm_conf::grid_spacing_*node(i);
  }
  for (uint i = 0; i < 3; ++i) {
    FLOAT ux = u(i)/mpm_conf::grid_spacing_;
    if (fabs(ux) > 2) {
      n(i) = 0;
      gradn(i) = 0;
    } else if (fabs(ux) > 1) {
      n(i) = 1.0/6.0*pow(2 - fabs(ux), 3);
      if (ux < 0) {
       	gradn(i) = 0.5*pow(2+ux, 2);
      } else {
       	gradn(i) = -0.5*pow(2-ux, 2);
      }
    } else {
      n(i) = 0.5*pow(fabs(ux), 3) - pow(fabs(ux), 2) +2.0/3.0;
      if (ux < 0) {
  	gradn(i) = -3.0/2.0*pow(ux, 2) - 2*ux;
      } else {
  	gradn(i) = 3.0/2.0*pow(ux, 2) - 2*ux;
      }
    }
  }
  
  IS_DEF(n(0));
  IS_DEF(gradn(0));
  return 1.0/mpm_conf::grid_spacing_*VEC3(gradn(0)*n(1)*n(2), gradn(1)*n(0)*n(2), gradn(2)*n(1)*n(0));
}

void Particule::setAnisotropyTensor(VEC3 a) {
 // constitutive elasticity tensor
  FLOAT E1 = mpm_conf::young_vec_(0);
  FLOAT E2 = mpm_conf::young_vec_(1);
  FLOAT E3 = mpm_conf::young_vec_(2);
  FLOAT nu12 = mpm_conf::poisson_vec_(2);
  FLOAT nu13 = mpm_conf::poisson_vec_(1);
  FLOAT nu23 = mpm_conf::poisson_vec_(0);

  FLOAT E = mpm_conf::young_modulus_;
  FLOAT nu = mpm_conf::poisson_;
  FLOAT G = E/(2*(1+nu));
  
  TEST(E1 != 0);

  MATX tangent_stiffness_iso(6, 6);
  MATX inverse_tangent_stiffness_iso(6, 6);
  MATX tangent_stiffness(6, 6);
  MATX inverse_tangent_stiffness(6, 6);
  
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
   inverse_tangent_stiffness(i, i) = 1/mpm_conf::young_vec_(i);
   inverse_tangent_stiffness(i+3, i+3) = 1/(2*mpm_conf::shearing_vec_(i));
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
   tangent_stiffness(3, 3) = 2*mpm_conf::shearing_vec_(0);
   tangent_stiffness(4, 4) = 2*mpm_conf::shearing_vec_(1);
   tangent_stiffness(5, 5) = 2*mpm_conf::shearing_vec_(2);

   
   for (uint i = 0; i < 3; ++i) {
     for (uint j = 0; j < 3; ++j) {
        anisotropy_stress(i, j, i, j) = 0.5*a(i)*a(j);
        anisotropy_stress(i, j, j, i) = 0.5*a(i)*a(j);
 	inv_anisotropy_stress(i, j, i, j) = 0.5/a(i)/a(j);
 	inv_anisotropy_stress(j, i, i, j) = 0.5/a(i)/a(j);
      }
     anisotropy_stress(i, i, i, i) = a(i)*a(i);
      inv_anisotropy_stress(i, i, i, i) = 1.0/a(i)/a(i);

   }
   Tensor C_iso = mat2TensorOrtho(tangent_stiffness_iso);
   Tensor inv_C_iso = mat2TensorOrtho(inverse_tangent_stiffness_iso);
   Tensor C = mat2TensorOrtho(tangent_stiffness);
   Tensor inv_C = mat2TensorOrtho(inverse_tangent_stiffness);
   
   Tensor aux = innerProduct(anisotropy_stress, C);
   anisotropy_strain = innerProduct(inv_C_iso, aux);
   
   aux = innerProduct(inv_anisotropy_stress, C_iso);
   inv_anisotropy_strain = innerProduct(inv_C, aux);
}


/** not tested **/
void Particule::setAnisotropyTensor(MAT3 a) {
   // constitutive elasticity tensor
  FLOAT E1 = mpm_conf::young_vec_(0);
  FLOAT E2 = mpm_conf::young_vec_(1);
  FLOAT E3 = mpm_conf::young_vec_(2);
  FLOAT nu12 = mpm_conf::poisson_vec_(2);
  FLOAT nu13 = mpm_conf::poisson_vec_(1);
  FLOAT nu23 = mpm_conf::poisson_vec_(0);

  FLOAT E = mpm_conf::young_modulus_;
  FLOAT nu = mpm_conf::poisson_;
  FLOAT G = E/(2*(1+nu));

  MATX tangent_stiffness_iso(6, 6);
  MATX inverse_tangent_stiffness_iso(6, 6);
  MATX tangent_stiffness(6, 6);
  MATX inverse_tangent_stiffness(6, 6);
  
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
   inverse_tangent_stiffness(i, i) = 1/mpm_conf::young_vec_(i);
   inverse_tangent_stiffness(i+3, i+3) = 1/(2*mpm_conf::shearing_vec_(i));
  }
 inverse_tangent_stiffness(0, 1) = -nu12/E1;
 inverse_tangent_stiffness(0, 2) = -nu13/E1;
 inverse_tangent_stiffness(1, 0) = -nu12/E1;
 inverse_tangent_stiffness(1, 2) = -nu23/E2;
 inverse_tangent_stiffness(2, 0) = -nu13/E1;
 inverse_tangent_stiffness(2, 1) = -nu23/E2;

 //INFO(3, "nu31 "<<E3/E1*nu13);
 
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
   tangent_stiffness(3, 3) = 2*mpm_conf::shearing_vec_(0); //2*G23
   tangent_stiffness(4, 4) = 2*mpm_conf::shearing_vec_(1);;//2*G13;
   tangent_stiffness(5, 5) = 2*mpm_conf::shearing_vec_(2);//2*G12;
   anisotropy_stress = Tensor(a);
   inv_anisotropy_stress = Tensor(a.inverse());
   
   Tensor C_iso = mat2TensorOrtho(tangent_stiffness_iso);
   Tensor inv_C_iso = mat2TensorOrtho(inverse_tangent_stiffness_iso);
   Tensor C = mat2TensorOrtho(tangent_stiffness);
   Tensor inv_C = mat2TensorOrtho(inverse_tangent_stiffness);
   
   Tensor aux = innerProduct(anisotropy_stress, C);
   anisotropy_strain = innerProduct(inv_C_iso, aux);
   
   aux = innerProduct(inv_anisotropy_stress, C_iso);
   inv_anisotropy_strain = innerProduct(inv_C, aux);

}




void Particule::update(VEC3 & p, VEC3 & v, MAT3 & b, MAT3 & t) {
  if (!out) {
  VEC3 prev_pos = pos;
  IS_DEF(t(0, 0));
  if (mpm_conf::method_ == mpm_conf::apic_) {
    pos = p;
  } else {
    pos += mpm_conf::dt_*v;
  }

  IS_DEF(pos(0));
  IS_DEF(pos(1));
  IS_DEF(pos(2));
  vel = v;

  FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(pos(0)/h), (int)(pos(1)/h), (int)(pos(2)/h));
  B = b;
  F_e *= (MAT3::Identity() + mpm_conf::dt_*t);

  //volume correction
  if (F_e.determinant() != 0) {
    vp += log(F_e.determinant());
  }
  if (std::isnan(vp)) {
      vp = 0;
    }

  //  F = F_e*F_p;

  //  updateAnisotropy(t);

  
  if (std::isnan(F_e(0, 0)) || std::isinf(F_e(0,0))) {
    F_e = MAT3::Identity();
  }

  MAT3 E = 0.5*(F_e.transpose()+F_e) - MAT3::Identity();
  MAT3 F_asym = 0.5*(F_e-F_e.transpose());
  MAT3 strain = E;

  if (mpm_conf::anisotropy_on) {
    realToFictitious(E, strain);
  } 
  
  
  SelfAdjointEigenSolver<MatrixXd> es(strain + MAT3::Identity());
  MAT3 V = es.eigenvectors();

  //  MAT3 EV =  es.eigenvectors();
  // INFO(3, "U \n"<<U);
  // INFO(3, "V \n"<<V);
  // INFO(3, "other V \n"<<es.eigenvectors());
  
  VEC3 T(0, 0, 0);
  VEC3 sigma = es.eigenvalues();
  //VEC3 sigma = svd.singularValues();

   
  // INFO(3, "strain\n"<<strain);
  // INFO(3, "recomputed strain\n"<<V*X*V.transpose());
  
  MAT3 der = MAT3::Zero();
      
   if (std::isnan(sigma(0)) || std::isinf(sigma(0))) {
     sigma = VEC3(1, 1, 1);
   }
   T = sigma;
      
  /**  elsastoplastic */
  if (mpm_conf::plastic_mode_ != 3) {  
    IS_DEF(sigma(0));
    IS_DEF(sigma(1));
    IS_DEF(sigma(2));

    /* plastic step */
    //INFO(3, "sigma "<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
    project(sigma, T);
    //     INFO(3, "proj "<<T(0)<<" "<<T(1)<<" "<<T(2));
  }
  IS_DEF(T(0));
  IS_DEF(T(1));
  IS_DEF(T(2));
    
  /* elastic step */
    computeEnergyDerivative(T);

    MAT3 stress;
    MAT3 proj_strain;
    if (mpm_conf::elastic_mode_ != 2) {
      MAT3 sigma_m = MAT3::Identity();
      for (uint i = 0; i < 3; ++i) {
	sigma_m(i, i) = T(i);
	der(i, i) = energy_der(i);
      }
      MAT3 F_anis = V*sigma_m*V.transpose();
      proj_strain = F_anis - MAT3::Identity();
	
      stress = V*der*V.inverse()*F_anis.transpose(); //TODO check F_e
    } else {
      MAT3 sigma_m = MAT3::Identity();
      for (uint i = 0; i < 3; ++i) {
	sigma_m(i, i) = T(i) - 1;
      }
      proj_strain =  V*sigma_m*V.transpose();
      stress = linearElasticity(proj_strain);
    }
    MAT3 S = stress;
    E = proj_strain;
    if (mpm_conf::anisotropy_on) {
      fictitiousToReal(proj_strain, stress, E, S);
    }
    F_e = E + MAT3::Identity() + F_asym;
    
    forceIncrement = v0*F_e.determinant()*S;
    
    if (std::isnan(forceIncrement(0, 0)) || std::isinf(forceIncrement(0, 0)) ) { //|| forceIncrement.maxCoeff() < 1e-15) {
      forceIncrement = MAT3::Zero();
    }


    /** hardenning etc.. */
       if (mpm_conf::plastic_mode_ == 0) {
       	FLOAT fric_angle = mpm_conf::hardenning_param_(0) +
       	  (mpm_conf::hardenning_param_(1)*hardenning - mpm_conf::hardenning_param_(3))*exp(-mpm_conf::hardenning_param_(2)*hardenning);
       	fric_angle = fric_angle*M_PI/180; //convert radian
       	alpha = sqrt(2.0/3.0)*(2*sin(fric_angle))/(3-sin(fric_angle));
       } else if (mpm_conf::plastic_mode_ == 1) {
       	//alpha = exp(mpm_conf::hardenning_param_(3)*(1-det_Fp));  //todo check 
       }

    IS_DEF(rotation(0,0));
    IS_DEF(T(0));
    IS_DEF(F_e(0, 0));

    // volume correction
    if (F_e.determinant() != 0) {
      vp -= log(F_e.determinant());
    }
    if (std::isnan(vp)) {
      vp = 0;
    }
    
  // } else { // no plastic //TODO
  //   // INFO(3, "NO PLASTIC");
  //   computeEnergyDerivative(sigma);
  //    for (uint i = 0; i < 3; ++i) {
  //      der(i, i) = energy_der(i);
  //    }
  //    MAT3 stress = V*der*V.inverse()*F_e.transpose();
  //    forceIncrement = v0*F_e.determinant()*stress;
  //    //forceIncrement = v0*F_e.determinant()*linearElasticity();
  // }
 if (mpm_conf::implicit_) {
   computeEnergySecondDer(sigma, V, V);
 }
  }
}



// void Particule::update(VEC3 & p, VEC3 & v, MAT3 & b, MAT3 & t) {
//   VEC3 prev_pos = pos;
//   IS_DEF(t(0, 0));
//   if (mpm_conf::method_ == mpm_conf::apic_) {
//     pos = p;
//   } else {
//     pos += mpm_conf::dt_*v;
//   }

//   IS_DEF(pos(0));
//   IS_DEF(pos(1));
//   IS_DEF(pos(2));
//   vel = v;

//   FLOAT h = mpm_conf::grid_spacing_;
//   cell = Vector3i((int)(pos(0)/h), (int)(pos(1)/h), (int)(pos(2)/h));
//   B = b;
//   F_e *= (MAT3::Identity() + mpm_conf::dt_*t);

//   //volume correction
//   if (F_e.determinant() != 0) {
//     // IS_DEF(F_e.determinant());
//     vp += log(F_e.determinant());
//   }
//   if (std::isnan(vp)) {
//       vp = 0;
//     }

//   //  F = F_e*F_p;

//   //  updateAnisotropy(t);

  
//   if (std::isnan(F_e(0, 0)) || std::isinf(F_e(0,0))) {
//     F_e = MAT3::Identity();
//   }
//   MAT3 symF = 0.5*(F_e + F_e.transpose());
//   MAT3 skewF = 0.5*(F_e - F_e.transpose());
//   MAT3 strain = F_e;
  
//   if (mpm_conf::anisotropy_on) {
//     // MAT3 rotF = rotation.transpose()*(F_e);//*rotation;
//     // MAT3 symF = 0.5*(rotF + rotF.transpose());
//     // skewF = 0.5*(rotF - rotF.transpose());
//     // isoF = innerProduct(anisotropy_strain, symF) - innerProduct(anisotropy_strain,  MAT3::Identity()) + MAT3::Identity();


//     MAT3 rotF = rotation.transpose()*symF*rotation;
//     strain = innerProduct(anisotropy_strain, rotF) - innerProduct(anisotropy_strain,  MAT3::Identity()) + MAT3::Identity();
//   } 
  
  
//   JacobiSVD<MATX> svd(strain, ComputeThinU | ComputeThinV);
//   MAT3 U = svd.matrixU();
//   MAT3 V = svd.matrixV();
//   VEC3 T(0, 0, 0);
//   VEC3 sigma = svd.singularValues();
//   MAT3 der = MAT3::Zero();

//   if (std::isnan(sigma(0)) || std::isinf(sigma(0))) {
//     sigma = VEC3(1, 1, 1);
//   }
//   IS_DEF(sigma(0));
  
//   /**  elsastoplastic */
//   if (mpm_conf::plastic_mode_ != 3) {  
//     IS_DEF(sigma(0));
//     IS_DEF(sigma(1));
//     IS_DEF(sigma(2));
    
//     project(sigma, T);
 
//     IS_DEF(T(0));
//     IS_DEF(T(1));
//     IS_DEF(T(2));
//     computeEnergyDerivative(T);
    
//     MAT3 inv_T = MAT3::Zero();
//     MAT3 T_m =  MAT3::Zero();
//     MAT3 sigma_m = MAT3::Zero();
//     double det_Fp = 1;
      
//     for (uint i = 0; i < 3; ++i) {
//       T_m(i, i) = T(i);
//       der(i, i) = energy_der(i);
//       TEST(T(i) != 0);
//       if (T(i) != 0) {
//   	inv_T(i, i) = 1.0/T(i);
//   	det_Fp *= sigma(i)*inv_T(i, i);
//       } else {
//   	det_Fp = 1;
//       }
//       sigma_m(i, i) = sigma(i);
//     }
//     if (mpm_conf::anisotropy_on) {
//       MAT3 isoF = U*(T_m)*V.transpose() - MAT3::Identity();
//       MAT3 anisF = innerProduct(inv_anisotropy_strain, isoF) +  MAT3::Identity();
//       F_e = rotation*anisF*rotation.transpose() + skewF;//*rotation.transpose();
//       F_p = F_e.inverse()*F;//rotation*V*inv_T*sigma_m*V.transpose()* rotation.transpose()*F_p;
//     } else {
//       F_e = U*(T_m)*V.transpose();
//       F_p = V*inv_T*sigma_m*V.transpose()*F_p;
//     }
//     F = F_e*F_p;
  
//     if (mpm_conf::plastic_mode_ == 0) {
//        FLOAT fric_angle = mpm_conf::hardenning_param_(0) +
// 	 (mpm_conf::hardenning_param_(1)*hardenning - mpm_conf::hardenning_param_(3))*exp(-mpm_conf::hardenning_param_(2)*hardenning);
//        fric_angle = fric_angle*M_PI/180; //convert radian
//        alpha = sqrt(2.0/3.0)*(2*sin(fric_angle))/(3-sin(fric_angle));
//     } else if (mpm_conf::plastic_mode_ == 1) {
//       alpha = exp(mpm_conf::hardenning_param_(3)*(1-det_Fp));
//     }

//     IS_DEF(rotation(0,0));
//     IS_DEF(T(0));
//     IS_DEF(F_e(0, 0));

//     // volume correction
//     if (F_e.determinant() != 0) {
//       //IS_DEF(F_e.determinant());
//       vp -= log(F_e.determinant());
//     }
//     if (std::isnan(vp)) {
//       vp = 0;
//     }
    
//   } else { // no plastic
//     computeEnergyDerivative(sigma);
//     for (uint i = 0; i < 3; ++i) {
//       der(i, i) = energy_der(i);
//     }
//   }
//   if (mpm_conf::elastic_mode_ != 2) {
//     if (mpm_conf::anisotropy_on) {
//       MAT3 iso_sigma = U*der*V.transpose()*strain.transpose();
//       MAT3 anis_sigma = innerProduct(inv_anisotropy_stress, iso_sigma);
//       //forceIncrement = v0*rotation*U*der*V.transpose()*rotation.transpose()*F_e.transpose();
//       forceIncrement = v0*rotation*anis_sigma*rotation.transpose();
//     } else {
//       forceIncrement = v0*U*der*V.transpose()*F_e.transpose();
//     }
//   } else {
//     forceIncrement = v0*F_e.determinant()*linearElasticity();
//   }
//   if (std::isnan(forceIncrement(0, 0)) || std::isinf(forceIncrement(0, 0)) ) { //|| forceIncrement.maxCoeff() < 1e-15) {
//     forceIncrement = MAT3::Zero();
//   }
//   if (mpm_conf::implicit_) {
//     computeEnergySecondDer(sigma, U, V);
//   }
// }

void Particule::initVolume(FLOAT d) {
  density = d;
  v0 = m/d;
}

void Particule::computeEnergyDerivative(VEC3 sigma) {
 IS_DEF(sigma(0));
   IS_DEF(sigma(1));
   IS_DEF(sigma(2));
 
  energy_der = VEC3(0, 0, 0);
  if (mpm_conf::elastic_mode_ == 0) {
    VEC3 invSigma(0, 0, 0);
    VEC3 lnSigma(0, 0, 0);
    FLOAT tr = 0;
    for (uint i = 0; i < 3; ++i) {
      FLOAT ev = sigma(i);
      if (ev != 0) {
	invSigma(i) = 1.0/ev;
	lnSigma(i) = log(ev);
	tr += log(ev);
	IS_DEF(invSigma(i));
	IS_DEF(lnSigma(i));
      }
    }
    for (uint i = 0; i < 3; ++i) {
      energy_der(i) = 2*mpm_conf::mu_*invSigma(i)*lnSigma(i) + mpm_conf::lambda_*tr*invSigma(i);
    }
  } else if (mpm_conf::elastic_mode_ == 1) {
     for (uint i = 0; i < 3; ++i) {
       if (sigma(i) < 1e-10) {
	 sigma(i) = 0;
       }
     }
    FLOAT det = sigma(0)*sigma(1)*sigma(2);
    if (std::isnan(det) || std::isinf(det)) {
      det = 1;
    }
    IS_DEF(det);
    for (uint i = 0; i < 3; ++i) {
      IS_DEF(alpha);
      energy_der(i) = 2*mpm_conf::mu_*alpha*(sigma(i)-1) + mpm_conf::lambda_*alpha*sigma((i+1)%3)*sigma((i+2)%3)*(det -1);
      if (std::isnan(energy_der(i)) || std::isinf(energy_der(i))) {
	energy_der(i) = 0;
      }
      IS_DEF(energy_der(i));
    }
  }
}



MAT3 Particule::linearElasticity() {
  
  MAT3 rotF = (F_e)*rotation;//*rotation;
  MAT3 symF = (rotF.transpose()*rotF);
  MAT3 isoF = 0.5*(symF -  MAT3::Identity());
  if (mpm_conf::anisotropy_on) {
    isoF = innerProduct(anisotropy_strain, symF) - innerProduct(anisotropy_strain,  MAT3::Identity());
  }
  //  rotF = isoF;//0.5*(isoF + isoF.transpose());
  
  //INFO(3, "\nrot F\n"<<rotF<<"\n\n"<<isoF);
  VECX strain(6);
  VECX stress(6);

  strain(0) = isoF(0, 0);
  strain(1) = isoF(1, 1);
  strain(2) = isoF(2, 2);
  strain(3) = isoF(1, 2);
  strain(4) = isoF(0, 2);
  strain(5) = isoF(0, 1);
   
  stress = mpm_conf::tangent_stiffness_iso * strain;
 
  
  MAT3 sigma;
  sigma << stress(0), stress(5), stress(4),
    stress(5), stress(1), stress(3),
    stress(4), stress(3), stress(2);

  for (uint i = 0; i < 3; ++i) {
    for (uint j = 0; j < 3; ++j) {
      IS_DEF(sigma(i, j));
      if (fabs(sigma(i, j)) < 1e-100) {
	sigma(i, j) = 0;
      }
    }
  }
  MAT3 out = rotation*sigma*rotation.transpose();
  if (mpm_conf::anisotropy_on) {
    MAT3 anis_sigma = innerProduct(inv_anisotropy_stress, sigma);
    out = rotation*anis_sigma*rotation.transpose();
  }
  return out;
}

MAT3 Particule::linearElasticity(MAT3 E) {
  
  VECX strain(6);
  VECX stress(6);

  strain(0) = E(0, 0);
  strain(1) = E(1, 1);
  strain(2) = E(2, 2);
  strain(3) = E(1, 2);
  strain(4) = E(0, 2);
  strain(5) = E(0, 1);
   
  stress = mpm_conf::tangent_stiffness_iso * strain;
 
  
  MAT3 sigma;
  sigma << stress(0), stress(5), stress(4),
    stress(5), stress(1), stress(3),
    stress(4), stress(3), stress(2);

  for (uint i = 0; i < 3; ++i) {
    for (uint j = 0; j < 3; ++j) {
      IS_DEF(sigma(i, j));
      if (fabs(sigma(i, j)) < 1e-100) {
	sigma(i, j) = 0;
      }
    }
  }
  // MAT3 out = rotation*sigma*rotation.transpose();
  // if (mpm_conf::anisotropy_on) {
  //   MAT3 anis_sigma = innerProduct(inv_anisotropy_stress, sigma);
  //   out = rotation*anis_sigma*rotation.transpose();
  // }
  return sigma;
}


void Particule::project(VEC3 sigma, VEC3 & T) {
  if (mpm_conf::plastic_mode_ == 0) {
    VEC3 ln_sigma(0, 0, 0);
    VEC3 dev_ln_sigma(0, 0, 0);
    FLOAT tr = 0;
    FLOAT fr_norm = 0;
    FLOAT plastic_def = 0;
    for (uint i = 0; i < 3; ++i) {
      if (sigma(i) >= 1e-15) {
	IS_DEF(sigma(i));
	ln_sigma(i) = log(sigma(i))/*volume correction*/ + vp/3.0;
	IS_DEF(ln_sigma(i));
	tr += ln_sigma(i);
	fr_norm += pow(ln_sigma(i), 2);
      }
    }
    fr_norm = sqrt(fr_norm);
    bool null = true;
    for (uint i = 0; i < 3; ++i) {
      dev_ln_sigma(i) = ln_sigma(i) - tr/3.0; //3 -> dimension
      if (dev_ln_sigma(i) != 0) {
	null = false;
      }
    }
    if (null || tr > 0) {
      T = VEC3(1, 1, 1);
      plastic_def = fr_norm;
    } else {
      FLOAT yield = fr_norm + (3*mpm_conf::lambda_+2*mpm_conf::mu_)/(2*mpm_conf::mu_)*tr*alpha; //3 -> dimension
      if (yield < 0) {
	T = sigma;
	plastic_def = 0;
      } else {
	for (uint i = 0; i < 3; ++i) {
	  T(i) = exp(ln_sigma(i) - yield*dev_ln_sigma(i)/fr_norm);
	}
	plastic_def = yield;
      }
    }
    hardenning += plastic_def;
  } else if (mpm_conf::plastic_mode_ == 1) {
    double smax = mpm_conf::stretch_max(0);
    double smin = mpm_conf::stretch_min(0);
    
      for (uint i = 0; i < 3; ++i) {
	T(i) = sigma(i);
	if (sigma(i) < smin) {
	  T(i) = smin;
	} else if (sigma(i) > smax) {
	  T(i) = smax;
	}
      }
     } else if (mpm_conf::plastic_mode_ == 2) {
    
    FLOAT smax = mpm_conf::stretch_max(0);
    for (uint i = 0; i < 3; ++i) {
      // if (sigma(i) > 1) {
      // 	T(i) = 1;
      // } else {
	T(i) = sigma(i);
	//}
   }

   FLOAT diff = T(1) - T(2) - smax;
   if (diff > 0 && T(1) > 1) {

    T(1) = T(1) - diff;
     // T(1) = T(1) - diff/2.0;
     // T(2) = T(2) + diff/2.0;
   }
   
   diff = T(0) - T(1) - smax;
   if (diff > 0 && T(0) > 1) {
     T(0) = T(0) - diff;
     // T(0) = T(0) - diff/2.0;
     // T(1) = T(1) + diff/2.0;
   }

   diff = T(0) - T(2) - smax;
   if (diff > 0  && T(0) > 1) {
     T(0) =T(0) - diff;
     // T(0) = T(0) - diff/2.0;
     // T(2) = T(2) + diff/2.0;
   }

  }
  
}

 //anisotropy
void Particule::setAnisotropyAxes(VEC3 x, VEC3 y, VEC3 z) {
  rotation.col(0) = x;
  rotation.col(2) = x.cross(y);
  rotation.col(1) = rotation.col(2).cross(x);

  for (uint i = 0; i < 3; ++i) {
    rotation.col(i).normalize();
  }
}

void Particule::setAnisotropyRotation(MAT3 rot) {
  rotation = rot;
}

void Particule::setAnisotropyValues(FLOAT vx, FLOAT vy, FLOAT vz) {
  valx = vx;
  valy = vy;
  valz = vz;
}




void Particule::rotate(MAT3 rot) {
  IS_DEF(rot(0, 0));
  rotation = rot*rotation;
}

MAT3 Particule::getRotation() const {
   return rotation;
}

VEC3 Particule::getAnisotropy() const {
  return VEC3(valx, valy, valz);
}

MAT3 Particule::getMixRot() const {
   return mix_rot;
}

// void Particule::anisotropicProject(VEC3 sigma, VEC3 &T, MAT3 V) {
//   VEC3 mult = mpm_conf::anisotropy_values_;
//   VEC3 T_iso(0, 0, 0);
//   VEC3 sigma_iso;
//   VEC3 coef;
//   for (uint i = 0; i < 3; ++i) {
//     VEC3 v = V.col(i);
//     VEC3 v_iso;
//     for (uint k = 0; k < 3; ++k) {
//       v_iso(k) = v(k) * mult(k);
//     }
//     v_iso.normalize();
//     for (uint k = 0; k < 3; ++k) {
//       v(k) = v_iso(k) / mult(k);
//     }
//     coef(i)= v.norm();
//   }
  
//   for (uint i = 0; i < 3; ++i) {
//     sigma_iso(i) = sigma(i) * coef(i);
//   }
  
//   project(sigma_iso, T_iso);

//   for (uint i = 0; i < 3; ++i) {
//     T(i) = T_iso(i) / coef(i);
//   }
// }




// void Particule::anisotropicProject(VEC3 sigma, VEC3 &T, MAT3 V) {
 
//   // MAT3 rotF = rotation.transpose()*F_e;
//     // JacobiSVD<MATX> svd(rotF, ComputeThinU | ComputeThinV);
//     // MAT3 U = svd.matrixU();
//     // MAT3 V = svd.matrixV();
//     // VEC3 sigma = svd.singularValues();

//   if (mpm_conf::plastic_mode_ == 1) {
//   VEC3 smax = mpm_conf::stretch_max;
//    VEC3 smin = mpm_conf::stretch_min;

//   // TEST(sigma(0) >= sigma(1));
//   // TEST(sigma(1) >= sigma(2));
//   //  INFO(3,sigma);
//    //  INFO(3, "sigma : "<< sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
//   for (uint i = 0; i < 3; ++i) {
//     VEC3 v = sigma(i)*V.col(i);
//     VEC3 vmin;
//     VEC3 vmax;
//     for (uint k = 0; k < 3; ++k) {
//       vmax(k) = v(k) / smax(k);
//       vmin(k) = v(k) / smin(k);
//     }
//     if (vmax.squaredNorm() > 1) {
//       vmax.normalize();
//       for (uint k = 0; k < 3; ++k) {
//   	v(k) = vmax(k) * smax(k);
//       }
//       T(i) = v.norm();
//     } else if (vmin.squaredNorm() < 1) {
//       vmin.normalize();
//       for (uint k = 0; k < 3; ++k) {
//   	v(k) = vmin(k) * smin(k);
//       }
//       T(i) = v.norm();
//     } else {
//       T(i) = sigma(i);
//     }
//   }
//   } else if (mpm_conf::plastic_mode_ == 2) {
//     VEC3 smax = mpm_conf::stretch_max;
//   //  VEC3 smin = Vector3d(1, 1, 1) - mpm_conf::stretch_min;
//    // IS_DEF(sigma(0));
//     for (uint i = 0; i < 3; ++i) {
//       // if (sigma(i) > 1) {
//       //   T(i) = 1;
//       // } else {
//        T(i) = sigma(i);
//        //}
//    }
  
//    VEC3 lim;// = 0.0005;
//    for (uint i = 0; i < 3; ++i) {
//      VEC3 v = V.col(i);
//      for (uint k = 0; k < 3; ++k) {
//        v(k) = v(k)*smax(k);
//      }
//      lim(i) = v.norm();
//      IS_DEF(lim(i));
//    }
//    FLOAT diff = T(1) - T(2) - lim(0);
//    if (diff > 0) {
//      //     //T(1) = sigma(1) - diff;
//      T(1) = T(1) - diff/2.0;
//      T(2) = T(2) + diff/2.0;
//    }

//    // v = U.col(2);
//    // for (uint k = 0; k < 3; ++k) {
//    //   v(k) = v(k)*smax(k);
//    // }
//    // lim = v.norm();
   
//    diff = T(0) - T(1) - lim(2);
//    if (diff > 0) {
//      //     //T(1) = sigma(1) - diff;
//      T(0) = T(0) - diff/2.0;
//      T(1) = T(1) + diff/2.0;
//    }

//    // v = U.col(1);
//    // for (uint k = 0; k < 3; ++k) {
//    //   v(k) = v(k)*smax(k);
//    // }
//    //  lim = v.norm();
//      diff = T(0) - T(2) - lim(1);
//    if (diff > 0) {
//      //     //T(1) = sigma(1) - diff;
//      T(0) = T(0) - diff/2.0;
//      T(2) = T(2) + diff/2.0;
//    }
   
//     // INFO(3, "sigma : "<< sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
//     // INFO(3, "T : "<< T(0)<<" "<<T(1)<<" "<<T(2));
//     // INFO(3, "lim : "<< lim(0)<<" "<<lim(1)<<" "<<lim(2));
//     IS_DEF(T(0, 0));

//   } else {
//     TEST(false);
//   }
  
 

void Particule::computeEnergySecondDer(VEC3 sigma, MAT3 U, MAT3 V) {
  // Tensor aux(3);
 
  // for (uint i = 0; i < 3; ++i) {
  //   for (uint j = 0; j < 3; ++j) {
  //     aux(i, i, j, j) = energySecondDer(sigma, i, j);
  //     if (i != j) {
  // 	FLOAT dij = (energy_der(i) - energy_der(j))/(sigma(i) - sigma(j));
  // 	FLOAT sij = (energy_der(i) + energy_der(j))/(sigma(i) + sigma(j));
  // 	if (sigma(i) == sigma(j)) {
  // 	  dij = 1;
  // 	}
  // 	IS_DEF(dij);
  // 	IS_DEF(sij);
  // 	aux(i, j, i, j) = 0.5*(dij + sij);
  // 	aux(i, j, j, i) = 0.5*(dij - sij);
  //     }
  //   }
  // }

  //  for (uint i = 0; i < 3; ++i) {
  //   for (uint j = 0; j < 3; ++j) {
  //     for (uint k = 0; k < 3; ++k) {
  // 	 for (uint l = 0; l < 3; ++l) {
  // 	   energy_second_der(i, j, k, l) = 0;
	   
  // 	   for (uint r = 0; r < 3; ++r) {
  // 	     for (uint s = 0; s < 3; ++s) {
  // 	       for (uint u = 0; u < 3; ++u) {
  // 		 for (uint v = 0; v < 3; ++v) {
  // 		   energy_second_der(i, j, k, l) += v0*aux(r, s, u, v)*U(i, r)*V(j, s)*U(k, u)*V(l, v);
  // 		   IS_DEF(aux(r, s, u, v));
  // 		   //  INFO(3, energy_second_der(i, j, k, l));
  // 		 }
  // 	       }
  // 	     }
  // 	   }
  // 	   IS_DEF(energy_second_der(i, j, k, l));

  // 	 }
  //     }
  //   }
  //  }
  Tensor C = mat2TensorOrtho(mpm_conf::tangent_stiffness);
  energy_second_der = rotateTensor(C, rotation);
  //  INFO(3, "energy second der "<<energy_second_der);
}

// d Y_ii / d sigma_jj with Y = engergy derivarive
/** TODO **/
FLOAT Particule::energySecondDer(VEC3 sigma, uint i, uint j) {
  TEST(false);
  FLOAT d = 0;
  if (mpm_conf::elastic_mode_ == 1) {
    FLOAT det = sigma(0)*sigma(1)*sigma(2);
    if (i == j) {
      d = 2*mpm_conf::mu_ + mpm_conf::lambda_*pow(sigma((i+1)%3)*sigma((i+2)%3), 2);
    } else if (i > j) {
      d =  mpm_conf::lambda_*(sigma((i+1)%3)*sigma((i+2)%3)*sigma((j+1)%3)*sigma((j+2)%3) + (det - 1)*sigma((i+1)%3));
    } else {
      d =  mpm_conf::lambda_*(sigma((i+1)%3)*sigma((i+2)%3)*sigma((j+1)%3)*sigma((j+2)%3) + (det - 1)*sigma((j+1)%3));
    }
  } else  if (mpm_conf::elastic_mode_ == 0) {
    if (i == j) {
      d = 2 * mpm_conf::mu_*(1 - log(sigma(i))) + mpm_conf::lambda_*(sigma(i) - (log(sigma(0)) + log(sigma(1)) +log(sigma(2))));
      d /= sigma(i)*sigma(i);
    } else {
      d = mpm_conf::lambda_/(sigma(i)*sigma(j));
    }
  } else {
    TEST(false);
  }
  IS_DEF(d);
  return d;
}
    
MAT3 Particule::getOrientation() {
  return orientation;
}

// void Particule::addToMesh(std::list<VEC3> & points, std::list<VEC3> & normals,
// 			   std::list<VEC2> & tex_points, std::list<unsigned int> & connectivity) {
//   MAT3 D;
//   D << valx, 0, 0,
//     0, valy, 0,
//     0, 0, valz;
//   MAT3 S = rotation*D;

//   Sphere sp(0.01, m_shader);
//   sp.addToMesh(S, points, normals, tex_points, connectivity);
// }


void Particule::exportMitsuba(std::ofstream &file) {
  if ((rotation.col(0).cross(rotation.col(1))).dot(rotation.col(2)) < 0) {
    rotation.col(0) = -rotation.col(0);
  }
  ANGLE_AXIS rot(rotation);
  VEC3 axe = rot.axis();
  FLOAT angle = rot.angle()/M_PI*180.0;

  file<<"<shape type=\"instance\">\n";
  file<<"<ref id=\"particle\"/>\n";
  file<<"<transform name=\"toWorld\">\n";
  // file<<"<scale x=\""<<valx<<"\" y=\""<<valy<<"\" z=\""<<valz<<"\"/>\n";
  //  file<<"<scale x=\"0.015\" y=\"0.015\" z=\"0.015\"/>\n";
  //file<<"<scale x=\"0.005\" y=\"0.005\" z=\"0.005\"/>\n";
    file<<"<scale x=\"0.01\" y=\"0.01\" z=\"0.05\"/>\n";
    // file<<"<scale y=\"2\"/>\n";
  file<<"<rotate x=\""<<axe(0)<<"\" y=\""<<axe(1)<<"\" z=\""<<axe(2)<<"\" angle=\""<<angle<<"\"/>\n";
  file<<"<translate x=\""<<10*pos(0)<<"\" y=\""<<10*pos(1)<<"\" z=\""<<10*pos(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"</shape>\n";
}


void Particule::realToFictitious(const MAT3 & real_strain, MAT3 & fic_strain) {
  MAT3 rotE = rotation.transpose()*real_strain*rotation;
  fic_strain = innerProduct(anisotropy_strain, rotE);
}

void Particule::fictitiousToReal(const MAT3 & fic_strain, const MAT3 & fic_stress,
				 MAT3 & real_strain, MAT3 & real_stress) {
  MAT3 rotE = innerProduct(inv_anisotropy_strain, fic_strain);
  real_strain = rotation*rotE*rotation.transpose();
  MAT3 rotSig = innerProduct(inv_anisotropy_stress, fic_stress);
  real_stress = rotation*rotSig*rotation.transpose();
}


void Particule::updateAnisotropy(const MAT3 & t) {
  SelfAdjointEigenSolver<MatrixXd> es;
  MAT3 W = 0.5/mpm_conf::cheat_damping_*(t - t.transpose()); // skew-sym part of velocity grad
  MAT3 D = 0.5/mpm_conf::cheat_damping_*(t + t.transpose()); // sym part of velocity grad, strain rate tensor
  D = rotation.transpose()*D*rotation;
  FLOAT width = 1, length = 1.0;
  FLOAT l = (length/width - 1)/(length/width + 1); //ellongation of the ellipsoidal object

  MAT3 Pi2 = 0.5*(MAT3::Identity() - orientation);
  // MAT3 der_Pi2 = W*Pi2 + Pi2*W.transpose() + l*(D*Pi2 + Pi2*D - 2*innerProduct(D, outerProduct(Pi2, Pi2)));
  MAT3 der_Pi2 = -l*(D*Pi2 + Pi2*D - 2*innerProduct(D, outerProduct(Pi2, Pi2)));
  Pi2 += mpm_conf::dt_ * der_Pi2;
  orientation = MAT3::Identity() - 2*Pi2;
    
  VEC3 a_orient(1, 1, 1);
  VEC3 eigenvec;
  MAT3 diag = MAT3::Identity();
  es.compute(orientation);
  if (es.info() == Success) {
    VEC3 ev = es.eigenvalues();
    MAT3 evec = es.eigenvectors();
    for (uint i = 0; i < 3; ++i) {
      if (ev(i) < 0.01) {
	ev(i) =  0.01;
      } else if (ev(i) > 0.98){
	ev(i) =  0.98;
      }
      diag(i, i) = ev(i);
    }
    orientation = evec*diag*evec.inverse();
     
    FLOAT norm = std::fabs(es.eigenvalues()(0)) + std::fabs(es.eigenvalues()(1)) + std::fabs(es.eigenvalues()(2));
    orientation /= norm;
    eigenvec(0) = std::fabs(es.eigenvalues()(0));
    eigenvec(1) = std::fabs(es.eigenvalues()(1));
    eigenvec(2) = std::fabs(es.eigenvalues()(2));

    VEC3 c(1, 1, 1);
    MAT3 axes_o = MAT3::Identity();
    for (uint i = 0; i < 3; ++i) {
      if (eigenvec(i) < 1.0/3.0) {
	c(i) = 1 - eigenvec(i)*3.0;
	a_orient(i) = (1-c(i))*1 +c(i) * mpm_conf::anisotropy_values_(0);
      } else {
	c(i) =  (eigenvec(i) - 1.0/3.0)*3.0/2.0;
	a_orient(i) = (1-c(i))*1 +c(i) * mpm_conf::anisotropy_values_(2);
      }
    }
     
    VEC3 a;
    VEC3 c_new;
    VEC3 used(0, 0, 0);
    bool ok = true;
    MAT3 axes;
    for (uint i = 0; i < 3; ++i) {
      uint dir;
      FLOAT max = -1;
      for (uint j = 0; j < 3; ++j) {
	FLOAT ps =  es.eigenvectors().col(j).dot(axes_o.col(i));
	if (fabs(ps) > max && used(j) == 0) {
	  max = fabs(ps);
	  dir = j;
	}
      }
      ok = ok && fabs(max-1) < 0.2;
      used(dir) = 1;
      if (es.eigenvectors().col(dir).dot(axes_o.col(i)) < 0) {
	axes.col(i) = -es.eigenvectors().col(dir);
      } else {
	axes.col(i) = es.eigenvectors().col(dir);
      }
      c_new(i) = eigenvec(dir);
      a(i) = a_orient(dir);
    }

    // axes.col(0) = axes.col(1).cross(axes.col(2));
    // axes.col(1) = axes.col(2).cross(axes.col(0));
   
    for (uint i = 0; i < 3; ++i) {
      axes.col(i).normalize();
    }
    ANGLE_AXIS aa(axes);
    axes = aa.toRotationMatrix();
    if (ok && fabs(aa.angle()) < 0.1) {
      mix_rot = axes;
      rotation = axes*rotation;

      valx = c_new(0);
      valy = c_new(1);
      valz = c_new(2);
      setAnisotropyTensor(a);

      orientation = MAT3::Identity();
      orientation(0, 0) = c_new(0);
      orientation(1, 1) = c_new(1);
      orientation(2, 2) = c_new(2);
    }
  }
}
