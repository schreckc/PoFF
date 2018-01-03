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
  Object(shader), m(mass), v(vol), v0(vol), pos(p), normal(n), vel(velo), F_p(MAT3::Identity(3, 3)), F_e(MAT3::Identity(3, 3)), F(MAT3::Identity(3, 3)) {
  FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(p(0)/h), (int)(p(1)/h), (int)(p(2)/h));

  //  F_e *= 0.9;
  
  //  INFO(3, velo);//((int)(p(0)/Grid::spacing))<<", "<< ((int)(p(1)/Grid::spacing))<<", "<< ((int)(p(2)/Grid::spacing)));
  // m_model_view = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)));
  B = MAT3::Zero();
  forceIncrement = MAT3::Zero();
  hardenning = 0;
  FLOAT fric_angle = mpm_conf::hardenning_param_(0) + mpm_conf::hardenning_param_(3); //h0 + h3
  fric_angle = fric_angle*M_PI/180; 
  //FLOAT fric_angle = 0.15*M_PI;
  alpha = sqrt(2.0/3.0)*(2*sin(fric_angle))/(3-sin(fric_angle));

  if (normal.squaredNorm() == 0) {
    normal = VEC3((FLOAT)rand()/(FLOAT)RAND_MAX - 0.5, (FLOAT)rand()/(FLOAT)RAND_MAX - 0.5, (FLOAT)rand()/(FLOAT)RAND_MAX - 0.5);
    normal.normalize();
  }

  // axex = VEC3(1, 0, 0);
  // axey = VEC3(0, 1, 0);
  // axez = VEC3(0, 0, 1);

  axes = MAT3::Identity();

  valx = 1;
  valy = 1;
  valz = 1;

  ellipse << 1, 0, 0,
    0, 1, 0,
    0, 0, 1;

  rotation << 1, 0, 0,
    0, 1, 0,
    0, 0, 1;

  color = VEC3(1, 1, 1);

  // orientation << 1.0/3.0, 0, 0,
  //  0, 1.0/3.0, 0,
  //   0, 0, 1.0/3.0;
   // orientation << 0.21, 0, 0,
   //    0, 0.19, 0,
   //   0, 0, 0.6;
    // orientation << 0.1, 0, 0,
    //   0, 0.1, 0,
    //   0, 0, 0.8;

       orientation << 0.05, 0, 0,
         0, 0.01, 0,
         0, 0, 0.94;
  // orientation <<  mpm_conf::anisotropy_values_(0), 0, 0,
  //   0,  mpm_conf::anisotropy_values_(1), 0,
  //   0, 0,  mpm_conf::anisotropy_values_(2);

      
  anisotropy_strain = mpm_conf::anisotropy_strain_;
  inv_anisotropy_strain = mpm_conf::inv_anisotropy_strain_;
    anisotropy_stress = mpm_conf::anisotropy_stress_;
  inv_anisotropy_stress = mpm_conf::inv_anisotropy_stress_;

  valx = 0.1;
  valy = 0.1;
  valz = 1;
   //  VEC3 a;
   // a(0) = mpm_conf::anisotropy_values_(0);
   // a(1) = mpm_conf::anisotropy_values_(1);
   // a(2) = mpm_conf::anisotropy_values_(2);

   // setAnisotropyTensor(a);

  prevD = MAT3::Identity();
  for (uint i = 0; i < 3; ++i) {
    prevD(i, i) = mpm_conf::anisotropy_values_(i);
  }
  fixed = false;
}


void Particule::animate() {
  //  m_model_view = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)));
}

#ifndef NO_GRAPHICS_ 
void Particule::draw(glm::mat4 m, int s) {
  /* draw spheres */
  m_model_view = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)));
  if (mpm_conf::display_sphere_) {
      //   MAT3 F =  F_e*F_p;
      //   HouseholderQR<MAT3> decomp(F);
      // 	MAT3 Q = decomp.householderQ();

      // 	// MAT3 Q = 0.5*(F + F.transpose());

    glm::mat4 cur_model = m * m_model_view;
    
    //   if (mpm_conf::anisotropy_on) {
    //  axes = rotation*orientation;
    // VEC3 x = axes.col(0);//rotation/*axes*/.col(0);
    // VEC3 y = axes.col(1);//rotation/*axes*/.col(1);
    // VEC3 z = axes.col(2);//rotation/*axes*/.col(2);

    

    
     VEC3 x = /*axes.col(0);*/rotation/*axes*/.col(0);
     VEC3 y = /*axes.col(1);*/rotation/*axes*/.col(1);
     VEC3 z = /*axes.col(2);*/rotation/*axes*/.col(2);


    
      glm::mat3 R;
      R[0] = glm::vec3(x[0], x[1], x[2]);
      R[1] = glm::vec3(y[0], y[1], y[2]);
      R[2] = glm::vec3(z[0], z[1], z[2]);


      
      valx = 0.2; valy = 0.2;  valz = 0.8;
      //  INFO(3, "val "<<valx<<" "<<valy<<" "<<valz);
      //valx = 1; valy = 1;  valz = 1;
      glm::mat3 D;
      D[0] = glm::vec3(valx, 0, 0);
      D[1] = glm::vec3(0, valy, 0);
      D[2] = glm::vec3(0, 0, valz);

      glm::mat3 S = R*D;
      //glm::mat3 S = R;
      glm::mat4 S4 = glm::mat4(S);
       cur_model = cur_model * S4;
      //}

    
      Sphere sp(0.01, m_shader);
      if (fixed) {
      	sp.setColor(0, 0, 0);
      } else {
      	sp.setColor(color(0), color(1), color(2));
      }
      int cur_shader = m_shader;
      if (m_shader == -1) {
        cur_shader = s;
      }
   
      sp.draw(cur_model, cur_shader);

  
  // //   glLineWidth(3.0f);
  // //   Shader *tmp = m_shader;
  // //   // m_shader = Scene::SCENE->getShader(2);
  // // VEC3 n = F_e*F_p*normal;
  // // n.normalize();
  // // GLfloat n_line[6] = {0, 0, 0, 0.05*n(0), 0.05*n(1), 0.05*n(2)};
  // // GLfloat n_color[6] = {0, 0, 0, 0, 0, 0};
  // //   enableShader();
  // //   setMVP(m, s);
  
  // //  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, n_line);
  // //  glEnableVertexAttribArray(0);

  // //  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, n_color);
  // //  glEnableVertexAttribArray(1);

  // //  glDrawArrays(GL_LINES, 0, 2);

  // //  glDisableVertexAttribArray(0);
  // //  glDisableVertexAttribArray(1);

  // //  disableShader();
  // //  m_shader = tmp;


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
  // INFO(3, "vel "<<vel(0)<<", "<<vel(1)<<", "<<vel(2));
  // INFO(3, "pos "<<pos(0)<<", "<<pos(1)<<", "<<pos(2));

  /* draw velocity */
  // glLineWidth(3.0f);
  // m_shader = Scene::SCENE->getShader(2);
  // GLfloat vel_line[6] = {0, 0, 0, mpm_conf::dt_*vel(0), mpm_conf::dt_*vel(1), mpm_conf::dt_*vel(2)};
  // GLfloat vel_color[6] = {0, 0, 0, 0, 0, 0};
  // enableShader();
  // setMVP(m, s);
  
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vel_line);
  // glEnableVertexAttribArray(0);

  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vel_color);
  // glEnableVertexAttribArray(1);

  // glDrawArrays(GL_LINES, 0, 2);

  // glDisableVertexAttribArray(0);
  // glDisableVertexAttribArray(1);

  // disableShader();
  //  m_shader = cur_shader;

  // /* draw points */
  GLfloat vertex[6] = {0, 0, 0};//{pos(0), pos(1), pos(2)};
  GLfloat color[6] = {0, 0, 0};
  GLfloat normal[6] = {1, 0, 0};
  //  glPointSize(10.0f);
  glLineWidth(50.0f);

  enableShader();
  setMVP(m, s);
  // glm::mat4 vp;
  // Scene::SCENE->getProjView(vp);
  // glm::mat4 mvp = vp * m_model_view;
  // glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertex);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, color);
  glEnableVertexAttribArray(1);

  //   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, normal);
  // glEnableVertexAttribArray(2);

  glDrawArrays(GL_POINTS, 0, 1);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  //  glDisableVertexAttribArray(2);

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

MAT3 Particule::getDeformationPlastic() const {
  return F_p;
}

void Particule::setDeformationPlastic(MAT3 F) {
  F_p = F;
}

MAT3 Particule::getDeformation() const {
  return F_e*F_p;
}

const MAT3& Particule::getForceIncrement() const {
  // JacobiSVD<MatrixXf> svd(F_e, ComputeThinU | ComputeThinV);
  // MAT3 U = svd.matrixU();
  // MAT3 V = svd.matrixV();
  // VEC3 T(0, 0, 0);
  
  // VEC3 sigma = svd.singularValues();
  // FLOAT plastic_def;
  // project(sigma, alpha, T, plastic_def);
  
  // MAT3 inv_T = MAT3::Zero();
  // MAT3 T_m =  MAT3::Zero();
  // MAT3 sigma_m = MAT3::Zero();
  // for (uint i = 0; i < 3; ++i) {
  //   if (T(i) != 0) {
  //     inv_T(i, i) = 1.0/T(i);
  //     T_m(i, i) = T(i);
  //   }
  //   sigma_m(i, i) = sigma(i);
  // }
  // // INFO(3, F_p);
  
  // forceIncrement = v0*U*energyDerivative(sigma)*V.transpose()*F_e.transpose();
  IS_DEF(forceIncrement(0, 0));
  return forceIncrement;
}

const Tensor& Particule::getSecondEnergyDer() const {
  return energy_second_der;
}

FLOAT Particule::weight(Vector3i node) {
  FLOAT w = 1;
  //  INFO(3, cell<<"\n|\n"<<node<<"\n");
  FLOAT absx = 0;
  for (uint i = 0; i < 3; ++i) {
    if (cell(i) == node(i) || cell(i) == node(i) - 1) {
      absx = fabs(pos(i)/mpm_conf::grid_spacing_ -  (FLOAT)node(i));
      w *= 0.5*pow(absx, 3) - pow(absx, 2) + 2.0/3.0;
    } else if (cell(i) == node(i) + 1 || cell(i) == node(i) - 2) {
      //      absx = fabs(pos(i) - (FLOAT)i*Grid::spacing)/Grid::spacing;
      absx = fabs(pos(i)/mpm_conf::grid_spacing_ -  (FLOAT)node(i));
      w *= 1.0/6.0*pow(2 - absx, 3);
    } else {
      w = 0;
      break;
    }
  }
  
  // VEC3 u;
  // for (uint i = 0; i < 3; ++i) {
  //   u(i) = pos(i) - mpm_conf::grid_spacing_*node(i);
  // }
  // for (uint i = 0; i < 3; ++i) {
  //   FLOAT ux = u(i)/mpm_conf::grid_spacing_;
  //   if (fabs(ux) > 2) {
  //     w = 0;
  //   } else if (fabs(ux) > 1) {
  //     w *= 1.0/6.0*pow(2 - fabs(ux), 3);
  //   } else {
  //     w *= 0.5*pow(fabs(ux), 3) - pow(fabs(ux), 2) +2.0/3.0;
  //   }
  // }
  
  //INFO(3, pos<<"\n|\n"<<node<<"\n"<<w<<" "<<absx<<"\n");
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
      // FLOAT sgnx = 1;
      // if (ux < 0) {
      // 	sgnx = -1;
      // }
      // gradn(i) = -0.5*sgnx*pow(2 - fabs(ux), 2);
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
  
  // VEC3 n(0, 0, 0);
  // VEC3 gradn(0, 0, 0);
  // for (uint i = 0; i < 3; ++i) {
  //   if (cell(i) == node(i) || cell(i) == node(i) - 1) {
  //     FLOAT x = pos(i)/mpm_conf::grid_spacing_ - node(i);
  //     FLOAT absx = fabs(x);
  //     //     INFO(3,"absx "<<(pos(i) - node(i)*mpm_conf::grid_spacing_)/mpm_conf::grid_spacing_<<" "<<absx);
  //     // INFO(3,"absx "<<x<<" "<<pos(i)/mpm_conf::grid_spacing_);
  //     FLOAT sgnx = 1;
  //     if (cell(i) < node(i)) {
  // 	sgnx = -1;
  //     }
  //     n(i) = 0.5*pow(absx, 3) - pow(absx, 2) + 2.0/3.0;      
  //     gradn(i) = 1.5*sgnx*pow(absx, 2) - 2.0*x;
  //   } else if (cell(i) == node(i) + 1 || cell(i) == node(i) - 2) {
  //     FLOAT x = pos(i)/mpm_conf::grid_spacing_ - node(i);
  //     FLOAT absx = fabs(x);
  //     FLOAT sgnx = 1;
  //     if (cell(i) < node(i)) {
  // 	sgnx = -1;
  //     }
  //     n(i) = 1.0/6.0*pow(2 - absx, 3);
  //     gradn(i) = -0.5*sgnx*pow(2 - absx, 2);
  //   } else {
  //     break;
  //     // n(i) = 0;
  //     // gradn(i) = 0;
  //   }
  // }
  IS_DEF(n(0));
  IS_DEF(gradn(0));
  return 1.0/mpm_conf::grid_spacing_*VEC3(gradn(0)*n(1)*n(2), gradn(1)*n(0)*n(2), gradn(2)*n(1)*n(0));
  //return VEC3(gradn(0)*n(1)*n(2), gradn(1)*n(0)*n(2), gradn(2)*n(1)*n(0)); 
}

void Particule::setAnisotropyTensor(VEC3 a) {
  // INFO(3, "anisotrpy eigenval "<<eigenvec(0)<<" "<<eigenvec(1)<<" "<<eigenvec(2));
  // INFO(3, "anisotrpy coef "<<c1<<" "<<c2<<" "<<c3);
  // INFO(3, " ---------------------------------------------------------------- anisotrpy val "<<a(0)<<" "<<a(1)<<" "<<a(2));
 //  // }
 //  // TEST(a(2) >= 0.49999 && a(2) <= 1.00001);
  
 // constitutive elasticity tensor
  FLOAT E1 = mpm_conf::young_vec_(0);
  FLOAT E2 = mpm_conf::young_vec_(1);
  FLOAT E3 = mpm_conf::young_vec_(2);
  FLOAT nu12 = mpm_conf::poisson_vec_(2);
  FLOAT nu13 = mpm_conf::poisson_vec_(1);
  FLOAT nu23 = mpm_conf::poisson_vec_(0);

  // INFO(3, "YOUNG "<<E1<<" "<<E2<<" "<<E3);
  // INFO(3, "poisson "<<nu23<<" "<<nu13<<" "<<nu12);

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

   // map from anisotropic to isotropc space
   // Tensor  anisotropy_stress, inv_anisotropy_stress;
   // Tensor  anisotropy_strain, inv_anisotropy_strain;
   
   for (uint i = 0; i < 3; ++i) {
     for (uint j = 0; j < 3; ++j) {
     //   anisotropy_stress_(i, j, i, j) = anisotropy_values_(i);
     //   inv_anisotropy_stress_(i, j, i, j) = 1.0/anisotropy_values_(i);
     // }
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
   // INFO(3, "I"<< Tensor());
   // INFO(3, "C"<< C);
   // INFO(3, "A:C"<< aux);

   // INFO(3, "inv_C:C\n"<< tensor2MatOrtho(innerProduct(inv_C, C)));
   
   
   aux = innerProduct(inv_anisotropy_stress, C_iso);
   inv_anisotropy_strain = innerProduct(inv_C, aux);

}



void Particule::setAnisotropyTensor(MAT3 a) {
  // INFO(3, "anisotrpy eigenval "<<eigenvec(0)<<" "<<eigenvec(1)<<" "<<eigenvec(2));
  // INFO(3, "anisotrpy coef "<<c1<<" "<<c2<<" "<<c3);
  // INFO(3, " ---------------------------------------------------------------- anisotrpy val "<<a(0)<<" "<<a(1)<<" "<<a(2));
 //  // }
 //  // TEST(a(2) >= 0.49999 && a(2) <= 1.00001);
  
 // constitutive elasticity tensor
  FLOAT E1 = mpm_conf::young_vec_(0);
  FLOAT E2 = mpm_conf::young_vec_(1);
  FLOAT E3 = mpm_conf::young_vec_(2);
  FLOAT nu12 = mpm_conf::poisson_vec_(2);
  FLOAT nu13 = mpm_conf::poisson_vec_(1);
  FLOAT nu23 = mpm_conf::poisson_vec_(0);

  // INFO(3, "YOUNG "<<E1<<" "<<E2<<" "<<E3);
  // INFO(3, "poisson "<<nu23<<" "<<nu13<<" "<<nu12);

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

   // map from anisotropic to isotropc space
   // Tensor  anisotropy_stress, inv_anisotropy_stress;
   // Tensor  anisotropy_strain, inv_anisotropy_strain;
   
   // for (uint i = 0; i < 3; ++i) {
   //   for (uint j = 0; j < 3; ++j) {
   //   //   anisotropy_stress_(i, j, i, j) = anisotropy_values_(i);
   //   //   inv_anisotropy_stress_(i, j, i, j) = 1.0/anisotropy_values_(i);
   //   // }
       
   //     anisotropy_stress(i, j, i, j) = 0.5*a(i)*a(j);
   //      anisotropy_stress(i, j, j, i) = 0.5*a(i)*a(j);
   // 	inv_anisotropy_stress(i, j, i, j) = 0.5/a(i)/a(j);
   // 	inv_anisotropy_stress(j, i, i, j) = 0.5/a(i)/a(j);
   //    }
   //   anisotropy_stress(i, i, i, i) = a(i)*a(i);
   //    inv_anisotropy_stress(i, i, i, i) = 1.0/a(i)/a(i);

   // }

   // INFO(3, "A\n"<<a);
   // INFO(3, "A\n"<<a.inverse());


   anisotropy_stress = Tensor(a);
   inv_anisotropy_stress = Tensor(a.inverse());

   //   INFO(3, "ANIS "<< innerProduct(anisotropy_stress,inv_anisotropy_stress));
   
   
   Tensor C_iso = mat2TensorOrtho(tangent_stiffness_iso);
   Tensor inv_C_iso = mat2TensorOrtho(inverse_tangent_stiffness_iso);
   Tensor C = mat2TensorOrtho(tangent_stiffness);
   Tensor inv_C = mat2TensorOrtho(inverse_tangent_stiffness);
   
   Tensor aux = innerProduct(anisotropy_stress, C);
   anisotropy_strain = innerProduct(inv_C_iso, aux);
   // INFO(3, "I"<< Tensor());
   // INFO(3, "C"<< C);
   // INFO(3, "A:C"<< aux);

   // INFO(3, "inv_C:C\n"<< tensor2MatOrtho(innerProduct(inv_C, C)));
   
   
   aux = innerProduct(inv_anisotropy_stress, C_iso);
   inv_anisotropy_strain = innerProduct(inv_C, aux);

}

void Particule::update(VEC3 & p, VEC3 & v, MAT3 & b, MAT3 & t) {
  VEC3 prev_pos = pos;
  if (!fixed) {
  IS_DEF(t(0, 0));
  if (mpm_conf::method_ == mpm_conf::apic_) {
    pos = p;
  } else {
    pos += mpm_conf::dt_*v;
  }

  IS_DEF(pos(0));
  IS_DEF(pos(1));
  IS_DEF(pos(2));
 
    // /***** TEST ****/
  // rotation = MAT3::Identity();
  //  INFO(3, "rotatio \n"<< rotation);
  
    //   INFO(3, F_e);
  
     // INFO(3, "vel prev\n"<<vel);
     // INFO(3, "vel new\n"<<v);
   // VEC3 n(-1, 0, 0);
   // VEC3 d = (pos- VEC3(0.5, 0.5, 0.45));
   // vel = 20*d.cross(n);

  vel = v;
   
  } else {
    vel = VEC3(0, 0, 0);
  }
   FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(pos(0)/h), (int)(pos(1)/h), (int)(pos(2)/h));
  B = b;
  F_e *= (MAT3::Identity() + mpm_conf::dt_*t);
  //INFO(3, "volume correction "<<vp<<" "<<log(F_p.determinant()));
  //volume correction
  if (F_e.determinant() != 0) {
    // IS_DEF(F_e.determinant());
    vp += log(F_e.determinant());
  }
  //  IS_DEF(vp);
  if (std::isnan(vp)) {
      vp = 0;
    }

  //  F = F_e*F_p;
  
  //INFO(3, "grad v2\n"<<t);
 //  VEC3 dir = pos - VEC3(pos(0), 0.5, 0.45);
 // dir.normalize();
 //INFO(3, "dir "<<dir(0)<<" "<<dir(1)<<" "<<dir(2));

 SelfAdjointEigenSolver<MatrixXd> es;
 // EigenSolver<MatrixXd> es(t+t.transpose());
 //INFO(3, "D\n"<<t + t.transpose());
   
 // INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
 //    //    INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
 //   INFO(3, "eigen vec \n"<<es.eigenvectors().real());
 //   // INFO(3, "eigen vec \n"<<es.eigenvectors().imag());
  
    // // update orientation

 
 /* MAT3 W = 0.5/mpm_conf::cheat_damping_*(t - t.transpose()); // skew-sym part of velocity grad
 MAT3 D = 0.5/mpm_conf::cheat_damping_*(t + t.transpose()); // sym part of velocity grad, strain rate tensor
  D = rotation.transpose()*D*rotation;
 FLOAT width = 1, length = 1;
 FLOAT l = (length/width - 1)/(length/width + 1); //ellongation of the ellipsoidal object


  // rotation += mpm_conf::dt_ *(W*rotation);// + l*D*rotation);
  //   for (uint i = 0; i < 3; ++i) {
  //      rotation.col(i).normalize();
  //    }
  //     rotation.col(2) = rotation.col(0).cross(rotation.col(1));
  //     rotation.col(1) = rotation.col(2).cross(rotation.col(0));
 
 // //  // INFO(3, "D\n"<<D);
   //  INFO(3, "W\n"<<W);

 // //    // W << 0, 0, 0,
 // //    //  0, 0, 0,
 // //    //  0, 0, 0;
 // //  // D << 0, 0, 0,
 // //  //   0, 0, 20,
 // //  //   0, 20, 0;
 // //            // rotation = utils::rotation(mpm_conf::dt_*100, VEC3(1, 0, 0));

  //   INFO(3, "rotation*roation\n"<< rotation*rotation.transpose());


   
   

   //   INFO(3, "rotation*roation\n"<< rotation*rotation.transpose());
   // es.compute(rotation);
   // rotation = es.eigenvectors().real()*es.eigenvectors().real().transpose();
   // INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
   // INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
   //  INFO(3, "eigen vec \n"<<es.eigenvectors().real());
   // FLOAT norm = std::fabs(es.eigenvalues()(0).real()) + std::fabs(es.eigenvalues()(1).real()) + std::fabs(es.eigenvalues()(2).real());
   // rotation /= norm;
   // rotation *= 3.0;

   

 // //   // //  //  for (uint i = 0; i < 3; ++i) {
 // // // //  //     rotation.col(i).normalize();
 // // // //  //  }
 // // // //   //  rotation.col(2) = rotation.col(0).cross(rotation.col(1));
 // // // //   //  rotation.col(1) = rotation.col(2).cross(rotation.col(0));
 // // // // // axes = rotation;
 
 // // // // // //  // INFO(3, "oritentation\n"<<orientation);
 // // // // // orientation = rotation*orientation*rotation.transpose();
 
 // // // // //  // es.compute(orientation);
 // // // // //  //     //    INFO(3, "eigen val prev\n"<<es.eigenvalues().real());
 // // // // //  // MAT3 Diag = es.pseudoEigenvalueMatrix();
 // // // // //  // MAT3 P = es.pseudoEigenvectors();
 MAT3 Pi2 = 0.5*(MAT3::Identity() - orientation);//0.5*P*(MAT3::Identity() - Diag)*P.transpose();
 // MAT3 der_Pi2 = W*Pi2 + Pi2*W.transpose() + l*(D*Pi2 + Pi2*D - 2*innerProduct(D, outerProduct(Pi2, Pi2)));
 MAT3 der_Pi2 = -l*(D*Pi2 + Pi2*D - 2*innerProduct(D, outerProduct(Pi2, Pi2)));
  Pi2 += mpm_conf::dt_ * der_Pi2;
 // // // //  //  INFO(3, "der \n"<<der_Pi2);
 // // // // //    // INFO(3, " W*Pi2\n"<< W*Pi2 + Pi2*W);
  
 // // // // //  // EigenSolver<MatrixXd> es2(Pi2);
 // // // // //  // MAT3 Diag2 = es2.pseudoEigenvalueMatrix();
 // // // // //  // MAT3 P2 = es2.pseudoEigenvectors();
  orientation = MAT3::Identity() - 2*Pi2;//P2*(MAT3::Identity() - 2*Diag2)*P2.transpose();
    
 // // // // //   //  MAT3 init_o;
 // // // // //   //    init_o << 0.05, 0, 0,
 // // // // //   //     0, 0.05, 0,
 // // // // //   //     0, 0, 0.9;
  // INFO(3, "orientation\n"<<orientation);
 // // // // //   //   INFO(3, "rotatoted init  orientation\n"<<rotation*init_o*rotation.transpose())
   
 VEC3 a_orient(1, 1, 1);
 VEC3 eigenvec;
 MAT3 diag = MAT3::Identity();
 es.compute(orientation);
 if (es.info() == Success) {
   VEC3 ev = es.eigenvalues();//.real();
   MAT3 evec = es.eigenvectors();//.real();
     for (uint i = 0; i < 3; ++i) {
       if (ev(i) < 0.01) {
	 ev(i) =  0.01;
       } else if (ev(i) > 0.98){
	 ev(i) =  0.98;
       }
       diag(i, i) = ev(i);
     }
     orientation = evec*diag*evec.inverse();
     
     // FLOAT norm = std::fabs(ev(0))+std::fabs(ev(1))+std::fabs(ev(2));
     FLOAT norm = std::fabs(es.eigenvalues()(0)) + std::fabs(es.eigenvalues()(1)) + std::fabs(es.eigenvalues()(2));
     orientation /= norm;
 
 // // // //     orientation *= 3.0;

 // //   es.compute(orientation);
 // //   // INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
 // INFO(3, "eigen vec \n"<<es.eigenvectors());

 // // //     //setAnisotropyTensor(orientation);
    
  //  //   rotation = P2;
   // eigenvec(0) = 1 - 2*Diag2(0, 0);//std::fabs(es.eigenvalues()(0).real());
   // eigenvec(1) = 1 - 2*Diag2(1, 1);//std::fabs(es.eigenvalues()(1).real());
   // eigenvec(2) = 1 - 2*Diag2(2, 2);//std::fabs(es.eigenvalues()(2).real());
     eigenvec(0) = std::fabs(es.eigenvalues()(0));
     eigenvec(1) = std::fabs(es.eigenvalues()(1));
     eigenvec(2) = std::fabs(es.eigenvalues()(2));
     //  INFO(3, "eigen vec\n"<<es.eigenvectors().real());
  // INFO(3, "eigen val\n"<<es.eigenvalues().real());
  //  INFO(3, "eigen val imag \n"<<es.eigenvectors().imag());
  //  //   INFO(3, "axes vec\n"<<axes.col(2));
  // // FLOAT norm = eigenvec(0) + eigenvec(1) + eigenvec(2);
  // // orientation /= norm;
  // // es.compute(orientation);
  // //rotation = es.eigenvectors().real();
  // // VEC3 no;
  // // VEC3 n = axes.col(i);

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
     
   VEC3 a(0.123, 0.123, 0.123);
   VEC3 c_new(0.123, 0.123, 0.123);
   VEC3 used(0, 0, 0);
   bool ok = true;
   for (uint i = 0; i < 3; ++i) {
     uint dir;
     FLOAT max = -1;
     for (uint j = 0; j < 3; ++j) {
     //  FLOAT ps =  P2.col(j).dot(axes.col(i));es.eigenvectors().col(j).real().dot(axes.col(i));
     FLOAT ps =  es.eigenvectors().col(j).dot(axes_o.col(i));
       if (fabs(ps) > max && used(j) == 0) {
   	max = fabs(ps);
   	dir = j;
       }
     }
  // if (i == 2) {
  //    INFO(3, "eigen vec\n"<<es.eigenvectors().col(dir).real());
     //  INFO(3, "----> ps : "<<max);
  //  }
      ok = ok && fabs(max-1) < 0.2;
         used(dir) = 1;
     if (es.eigenvectors().col(dir).dot(axes_o.col(i)) < 0) {
 	 axes.col(i) = -es.eigenvectors().col(dir);
       } else {
 	 axes.col(i) = es.eigenvectors().col(dir);
       }
     c_new(i) = eigenvec(dir);
     // a(i) = es.eigenvalues()(dir).real();
     a(i) = a_orient(dir);
     //INFO(3, "cnew "<<c_new(i));
   }

   // axes.col(0) = axes.col(1).cross(axes.col(2));
   // axes.col(1) = axes.col(2).cross(axes.col(0));
   
    for (uint i = 0; i < 3; ++i) {
      axes.col(i).normalize();
    }
  
    //INFO(3, "\naxes\n"<<axes);
 //    INFO(3, "\naxes*axes\n"<<axes*axes.transpose());
 //  // INFO(3, "\nrotation\n"<<rotation);
 //  //  INFO(3, "\nrotation\n"<<rotation*rotation.transpose());
   if (ok) {
     rotation = axes*rotation;
     //     INFO(3, "val "<<a(0)<<" "<<a(1)<<" "<<a(2));
     
     // INFO(3, "\nrotation\n"<<rotation);
      valx = c_new(0);//a(0);
      valy = c_new(1);//a(1);
      valz = c_new(2);//a(2);
      //  INFO(3, "val "<<valx<<" "<<valy<<" "<<valz);
      setAnisotropyTensor(a);
     //INFO(3, "orientation\n"<<orientation);
     orientation = MAT3::Identity();
     orientation(0, 0) = c_new(0);
     orientation(1, 1) = c_new(1);
     orientation(2, 2) = c_new(2);
   }
 }
 /****/

  
  // INFO(3, "\nrotation*rotation\n"<<rotation*rotation.transpose());
 


 
  
  // // HouseholderQR<MAT3> decomp(orientation);
  // // MAT3 Q = decomp.householderQ();
  // // axes = Q;
  // // INFO(3, "Q\n"<<Q);
   
  
    // INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
    // // //INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
    // INFO(3, "eigen vec \n"<<es.pseudoEigenvectors().real());
    // // // INFO(3, "eigen vec \n"<<es.eigenvectors().imag());
   // //rotation = axes;

  //  INFO(3, "val "<<valx<<" "<<valy<<" "<<valz);
 
   // orientation = rotation.transpose()*orientation*rotation;
   
  // if (eigenvec(0) > eigenvec(1) && eigenvec(0) > eigenvec(2)) {
  //   c3 = (eigenvec(0) - 1.0/3.0)*3.0/2.0;
  //   c1 = eigenvec(1)*3;
  //   c2 = eigenvec(2)*3;
  //   //    no = es.eigenvectors().col(0).real();
  // } else if (eigenvec(1) > eigenvec(2)) {
  //   c3 = (eigenvec(1) - 1.0/3.0)*3.0/2.0;
  //   c1 = eigenvec(2)*3;
  //   c2 = eigenvec(0)*3;
  //   // no = es.eigenvectors().col(1).real();
  // } else {
  //   c3 = (eigenvec(2) - 1.0/3.0)*3.0/2.0;
  //   c1 = eigenvec(0)*3;
  //   c2 = eigenvec(1)*3;
  //   // no = es.eigenvectors().col(2).real();
  // }

  // c1 = (eigenvec(0) - 1.0/3.0)*3.0/2.0;
  // c2 = (eigenvec(1) - 1.0/3.0)*3.0/2.0;
  // c3 = (eigenvec(2) - 1.0/3.0)*3.0/2.0;
   
  // a(0) = (1-c(0))*1 +c1* mpm_conf::anisotropy_values_(0);//(1-c1)*1 +c1* mpm_conf::anisotropy_values_(2);//1 + c1*(mpm_conf::anisotropy_values_(0) - 1);
  // a(1) = (1-c2)*1 +c2* mpm_conf::anisotropy_values_(1);//(1-c2)*1 +c2* mpm_conf::anisotropy_values_(2);//1 + c2*(mpm_conf::anisotropy_values_(1) - 1);
  // a(2) = (1-c3)*1 +c3* mpm_conf::anisotropy_values_(2);


     // a(0) = mpm_conf::anisotropy_values_(0);
     // a(1) = mpm_conf::anisotropy_values_(1);
     // a(2) = mpm_conf::anisotropy_values_(2);

    //    INFO(3, "val "<<a(0)<<" "<<a(1)<<" "<<a(2));
    //setAnisotropyTensor(a);

  
 //  // for (uint i = 0; i < 3; ++i) {
 //  //   if (a(i) < mpm_conf::anisotropy_values_(2)) {
 //  //     a(i) = mpm_conf::anisotropy_values_(2);
 //  //   } else if (a(i) > 1) {
 //  //     a(i) = 1;
 //  //   }
 //  // }

 //  // INFO(3, "max orient "<<no(0)<<" "<<no(1)<<" "<<no(2));
 //  // INFO(3, "normal "<<n(0)<<" "<<n(1)<<" "<<n(2));
  
 // //  // if (a(2) <= 0.49999 || a(2) >= 1.00001) {
  //  INFO(3, "anisotrpy eigenval "<<eigenvec(0)<<" "<<eigenvec(1)<<" "<<eigenvec(2));
 //  // INFO(3, "anisotrpy coef "<<c1<<" "<<c2<<" "<<c3);
  //  INFO(3, " ---------------------------------------------------------------- anisotrpy val "<<a(0)<<" "<<a(1)<<" "<<a(2));
 // //  // }
 // //  // TEST(a(2) >= 0.49999 && a(2) <= 1.00001);
 // INFO(3, "oritentaion\n"<<orientation);



  // D = l*D;
  // INFO(3, "D\n"<<D);
  //INFO(3, "\nR\n"<<rotation);
  
 
  
  if (std::isnan(F_e(0, 0)) || std::isinf(F_e(0,0))) {
    F_e = MAT3::Identity();
  }
  MAT3 symF = 0.5*(F_e + F_e.transpose());
  MAT3 skewF = 0.5*(F_e - F_e.transpose());
  MAT3 strain = F_e;
  
  if (mpm_conf::anisotropy_on) {
    // MAT3 rotF = rotation.transpose()*(F_e);//*rotation;
    // MAT3 symF = 0.5*(rotF + rotF.transpose());
    // skewF = 0.5*(rotF - rotF.transpose());
    // isoF = innerProduct(anisotropy_strain, symF) - innerProduct(anisotropy_strain,  MAT3::Identity()) + MAT3::Identity();


    MAT3 rotF = rotation.transpose()*symF*rotation;
    strain = innerProduct(anisotropy_strain, rotF) - innerProduct(anisotropy_strain,  MAT3::Identity()) + MAT3::Identity();
    // INFO(3, "strain\n"<<strain<<"\n\nF_e\n"<<F_e);
  } 
  
  
  JacobiSVD<MATX> svd(strain, ComputeThinU | ComputeThinV);
  MAT3 U = svd.matrixU();
  MAT3 V = svd.matrixV();
  VEC3 T(0, 0, 0);
  VEC3 sigma = svd.singularValues();
  MAT3 der = MAT3::Zero();

  //  INFO(3,"sigma"<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));

  if (std::isnan(sigma(0)) || std::isinf(sigma(0))) {
    sigma = VEC3(1, 1, 1);
  }
  IS_DEF(sigma(0));
  
  /**  elsastoplastic */
  if (mpm_conf::plastic_mode_ != 3) {  
    
    // if (mpm_conf::anisotropy_on) {
    //    anisotropicProject(sigma, T, V);
    // } else {
    //    project(pow(F_p.determinant(), 1.0/3.0)*sigma, T);
    IS_DEF(sigma(0));
   IS_DEF(sigma(1));
   IS_DEF(sigma(2));
   
    project(sigma, T);
    //    INFO(3,"sigma"<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
      //    }
    IS_DEF(T(0));
   IS_DEF(T(1));
   IS_DEF(T(2));
    computeEnergyDerivative(T);
    
    MAT3 inv_T = MAT3::Zero();
    MAT3 T_m =  MAT3::Zero();
    MAT3 sigma_m = MAT3::Zero();
    double det_Fp = 1;
      
    for (uint i = 0; i < 3; ++i) {
      T_m(i, i) = T(i);
      der(i, i) = energy_der(i);
      TEST(T(i) != 0);
      if (T(i) != 0) {
  	inv_T(i, i) = 1.0/T(i);
  	det_Fp *= sigma(i)*inv_T(i, i);
      } else {
  	det_Fp = 1;
      }
      sigma_m(i, i) = sigma(i);
    }
    if (mpm_conf::anisotropy_on) {
      MAT3 isoF = U*(T_m)*V.transpose() - MAT3::Identity();
      MAT3 anisF = innerProduct(inv_anisotropy_strain, isoF) +  MAT3::Identity();
      F_e = rotation*anisF*rotation.transpose() + skewF;//*rotation.transpose();
      F_p = F_e.inverse()*F;//rotation*V*inv_T*sigma_m*V.transpose()* rotation.transpose()*F_p;
    } else {
      F_e = U*(T_m)*V.transpose();
      F_p = V*inv_T*sigma_m*V.transpose()*F_p;
    }
    F = F_e*F_p;
    //    EigenSolver<MatrixXd> es;//(F_p);// - 1.0/3.0*F_p.trace()*MAT3::Identity());
  //    INFO(3, "F_p\n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
  //    es.compute(F_p - 1.0/3.0*F_p.trace()*MAT3::Identity());
  // INFO(3, "F_p 2 \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
  //   // INFO(3, "F_p hydrostatic "<<1.0/3.0*F_p.trace());

    // MAT3 I = MAT3::Identity();
    // MAT3 R = utils::rotation(0.7, VEC3(0, 1, 0)); 

    // MAT3 M = R;
  
    // INFO(3, "M \n"<<M);
    
    // EigenSolver<MatrixXd> es(M);
    // INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
    // INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
    // INFO(3, "eigen vec \n"<<es.eigenvectors().real());
    // INFO(3, "eigen vec \n"<<es.eigenvectors().imag());
    
    
   //  MAT3 rotFp = rotation.transpose()*F_p*rotation;
   //  rotFp = 0.5*(rotFp*rotFp.transpose() - MAT3::Identity());
   //  rotFp = 10*(rotFp - 1.0/3.0*rotFp.trace()* MAT3::Identity());// + MAT3::Identity();

   //  EigenSolver<MatrixXd> es(rotFp);
   //  //     INFO(3, "F_p 3 \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real()<<"-->"<<es.eigenvalues()(0).real()+es.eigenvalues()(1).real()+es.eigenvalues()(2).real());
   //   // 	  INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
   //   // INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
    
   //  //  Tensor up_anis_strain = transformTensor(anisotropy_strain, rotFp);
   //  //  Tensor up_anis_stress = transformTensor(anisotropy_stress, rotFp);

   //  // INFO(3, "Fp\n"<<F_p);
   //  // INFO(3, "prev Fp\n"<<prevD);
    
   //   MAT3 D = MAT3::Identity();
   //   for (uint i = 0; i < 3; ++i) {
   //     D(i, i) = mpm_conf::anisotropy_values_(i);
   //   }
   //   // MAT3 prevD = D;
   //   D = rotFp+D;  

   //   bool feasible = true;
   //   // EigenSolver<MatrixXd> es(D);
   //   es.compute(D);
   //   //INFO(3, "D  "<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());//<<"-->"<<es.eigenvalues()(0).real()+es.eigenvalues()(1).real()+es.eigenvalues()(2).real());
   //   // INFO(3, "eigen vec \n"<<es.eigenvectors().real());
   //     VEC3 ev = es.eigenvalues().real();
   //     MAT3 evec = es.eigenvectors().real();

   //     MAT3 diag = MAT3::Identity();
   
   //     for (uint i = 0; i < 3; ++i) {
   // 	 if (ev(i) < mpm_conf::anisotropy_values_(2)) {
   // 	   ev(i) =  mpm_conf::anisotropy_values_(2);
   // 	 } else if (ev(i) > mpm_conf::anisotropy_values_(0)){
   // 	   ev(i) =  mpm_conf::anisotropy_values_(0);
   // 	 }
   // 	 diag(i, i) = ev(i);
   //     }
              
   //     D = evec*diag*evec.inverse();
   //      //prevD = D;
   //     setAnisotropyTensor(D);

   //     // for (uint i = 0; i < 3; ++i) {
   //    //   FLOAT ev = es.eigenvalues()(i).real();
   //    //   if (ev < mpm_conf::anisotropy_values_(2) || ev > mpm_conf::anisotropy_values_(0)) {
   //    // 	 feasible = false;
   //    // 	 break;
   //    //   }
   //    // }
   //    // if (feasible) {
   //    // 	setAnisotropyTensor(D);
   //    // 	prevD = D;
   //    //  } else {
   //    //   D = prevD;
   //    // }
    
   //     //	MAT3 rot = MAT3::Identity();
	
   //   es.compute(D);
   //  VEC3 val(42, 42, 42);
   //  //    MAT3 o = MAT3::Identity();
   //     for (uint i = 0; i < 3; ++i) {
   //   uint dir = 0;
   //   FLOAT max = 0;
   //   for (uint j = 0; j < 3; ++j) {
   //     FLOAT ps = es.eigenvectors().col(j).real().dot(axes.col(i));
   //     if (fabs(ps) >= fabs(max)) {
   // 	max = ps;
   // 	dir = j;
   //     }
   //   }
   //   // rot.col(i) = es.eigenvectors().col(dir).real();
   //   //  if (max < 0) {
   //   //    rot.col(i) = -rot.col(i);
   //   //  }
   //   val(i) =  es.eigenvalues()(dir).real();
   // }
    //    INFO(3, "Val  "<<valx<<" "<<valy<<" "<<valz);//<<" ---> "<<es.eigenvalues()(0).real()+es.eigenvalues()(1).real()+es.eigenvalues()(2).real());

   //     // rot.col(2) = rot.col(0).cross(rot.col(1));
   //     // rot.col(1) = rot.col(2).cross(rot.col(0));
   //     // for (uint i = 0; i < 3; ++i) {
   //     // 	 rot.col(i).normalize();
   //     // }
   //     //      rot =  es.eigenvectors().real();
   //            // INFO(3, "rot \n"<<rot);
   // 	      // INFO(3, "axes \n"<<rot);
   //     valx = val(0);//es.eigenvalues()(0).real();
   //     valy = val(1);//es.eigenvalues()(1).real();
   //     valz = val(2);//es.eigenvalues()(2).real();
     
   //     //       rotation = rot;
       
     //   FLOAT angle = 1.12;
     //   VEC3 axe(1, 0, 0);
     //   D2 = D2*utils::rotation(angle, axe);
     // MAT3 X;
     //  X << 1, 2, 3,
     //    2, 4, 5,
     //    3, 5, 6;

      // INFO(3, "test \n"<< innerProduct(Tensor(D2), X));
      // INFO(3, "test 2\n"<< D2* X);
      
	//	INFO(3, "roation 1\n"<<D);
      // INFO(3, "roation 2\n"<<innerProduct(transformTensor(mpm_conf::anisotropy_stress_, rotFp), MAT3::Identity()));
    
    //    INFO(3, "F_p\n"<<F_p);
    
    //     INFO(3, "stress"<<mpm_conf::anisotropy_stress_);
     // INFO(3, "strain"<<up_anis_strain);

    if (mpm_conf::plastic_mode_ == 0) {
       FLOAT fric_angle = mpm_conf::hardenning_param_(0) +
	 (mpm_conf::hardenning_param_(1)*hardenning - mpm_conf::hardenning_param_(3))*exp(-mpm_conf::hardenning_param_(2)*hardenning);
       fric_angle = fric_angle*M_PI/180; //convert radian
       alpha = sqrt(2.0/3.0)*(2*sin(fric_angle))/(3-sin(fric_angle));
    } else if (mpm_conf::plastic_mode_ == 1) {
      alpha = exp(mpm_conf::hardenning_param_(3)*(1-det_Fp));
    }

    IS_DEF(rotation(0,0));
    IS_DEF(T(0));
    IS_DEF(F_e(0, 0));
    // volume correction
    // INFO(3, "volume correction "<<vp<<" "<<log(F_p.determinant()));
    if (F_e.determinant() != 0) {
      //  IS_DEF(F_e.determinant());
      vp -= log(F_e.determinant());
    }
    if (std::isnan(vp)) {
      vp = 0;
    }
    
  } else { // no plastic
    computeEnergyDerivative(sigma);
    for (uint i = 0; i < 3; ++i) {
      der(i, i) = energy_der(i);
    }
  }
  if (mpm_conf::elastic_mode_ != 2) {
    if (mpm_conf::anisotropy_on) {
      MAT3 iso_sigma = U*der*V.transpose()*strain.transpose();
      MAT3 anis_sigma = innerProduct(inv_anisotropy_stress, iso_sigma);
      //forceIncrement = v0*rotation*U*der*V.transpose()*rotation.transpose()*F_e.transpose();
      forceIncrement = v0*rotation*anis_sigma*rotation.transpose();
    } else {
      forceIncrement = v0*U*der*V.transpose()*F_e.transpose();
    }
  } else {
    forceIncrement = v0*F_e.determinant()*linearElasticity();
  }
  if (std::isnan(forceIncrement(0, 0)) || std::isinf(forceIncrement(0, 0)) ) { //|| forceIncrement.maxCoeff() < 1e-15) {
    forceIncrement = MAT3::Zero();
  }
  if (mpm_conf::implicit_) {
    computeEnergySecondDer(sigma, U, V);
  }
  
  // INFO(3, "force incr\n"<< forceIncrement);
  
  //  if (fixed) {
 //     pos = prev_pos;
 // }

}

void Particule::initVolume(FLOAT d) {
  density = d;
  v0 = m/d;
  //INFO(3, "denisty part, vol "<< density <<" "<<v0<<" "<<m/d);
  //  INFO(3,"volume "<<v0<<" "<<d);
}

void Particule::computeEnergyDerivative(VEC3 sigma) {
  //INFO(3, "sigma energy der "<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
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
    // INFO(3, "sigma "<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
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
  
  MAT3 rotF = rotation.transpose()*(F_e);//*rotation;
  MAT3 symF = 0.5*(rotF + rotF.transpose());
  MAT3 isoF = symF -  MAT3::Identity();
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
  //   sigma = sigma*rotF.transpose();
  // INFO(3, "\nsigma\n"<<sigma);
  // INFO(3, "\nsigma rotated back\n"<<rotation.transpose()*sigma*rotation);
  // Tensor inv_anisotropy_stress;
  // for (uint i = 0; i < 3; ++i) {
  //   for (uint j = 0; j < 3; ++j) {
  //     inv_anisotropy_stress(i, j, i, j) = 1.0/mpm_conf::anisotropy_values_(i);
  //   }
  // }

  MAT3 out = rotation*sigma*rotation.transpose();
  if (mpm_conf::anisotropy_on) {
    MAT3 anis_sigma = innerProduct(inv_anisotropy_stress, sigma);
    out = rotation*anis_sigma*rotation.transpose();
  }
  return out;
}


void Particule::project(VEC3 sigma, VEC3 & T) {
  // INFO(3, "PROJECT "<<mpm_conf::plastic_mode_);
  if (mpm_conf::plastic_mode_ == 0) {
    VEC3 ln_sigma(0, 0, 0);
    VEC3 dev_ln_sigma(0, 0, 0);
    FLOAT tr = 0;
    FLOAT fr_norm = 0;
    FLOAT plastic_def = 0;
    for (uint i = 0; i < 3; ++i) {
      if (sigma(i) != 0) {
	ln_sigma(i) = log(sigma(i));///*volume correction*/ + vp/3.0;
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
      // INFO(3, "case 2 "<<fr_norm<<" "<<tr<<"\n"<<ln_sigma);
      T = VEC3(1, 1, 1);
      plastic_def = fr_norm;
    } else {
      FLOAT yield = fr_norm + (3*mpm_conf::lambda_+2*mpm_conf::mu_)/(2*mpm_conf::mu_)*tr*alpha; //3 -> dimension
      if (yield < 0) {
	//   INFO(3, "case 1");
	T = sigma;
	plastic_def = 0;
      } else {
	//   INFO(3, "case 3");
	for (uint i = 0; i < 3; ++i) {
	  T(i) = exp(ln_sigma(i) - yield*dev_ln_sigma(i)/fr_norm);
	}
	plastic_def = yield;
      }
    }
    hardenning += plastic_def;
  } else if (mpm_conf::plastic_mode_ == 1) {
    double smax = mpm_conf::stretch_max(0);//1 + 7.5e-3;
    double smin = mpm_conf::stretch_min(0);//1 - 2.5e-2;
    //if (density > 0.9*mpm_conf::density_) {
    //INFO(3,"density "<<mpm_conf::density_<<"   denstity local "<<density);
    
      for (uint i = 0; i < 3; ++i) {
	T(i) = sigma(i);
	if (sigma(i) < smin) {
	  T(i) = smin;
	  //INFO(3, "SIGMIN");
	  //  T(i) = 1;
	} else if (sigma(i) > smax) {
	  T(i) = smax;
	  //	  INFO(3, "SIGMAX");
	  //T(i) = 1;
	}
      }
      //         INFO(3, "T "<<T);
     } else if (mpm_conf::plastic_mode_ == 2) {
    
    FLOAT smax = mpm_conf::stretch_max(0);
    for (uint i = 0; i < 3; ++i) {
      // if (sigma(i) > 1) {
      // 	T(i) = 1;
      // } else {
	T(i) = sigma(i);
	//}
   }
    //    INFO(3, "T "<<T(1) - T(2)<<" "<<smax);
   FLOAT diff = T(1) - T(2) - smax;
   if (diff > 0 && T(1) > 1) {
     //  INFO(3, "T1"<<T);

    T(1) = T(1) - diff;
     // T(1) = T(1) - diff/2.0;
     // T(2) = T(2) + diff/2.0;
   }
   
   diff = T(0) - T(1) - smax;
   if (diff > 0 && T(0) > 1) {
     //INFO(3, "T2 "<<T);
     T(0) = T(0) - diff;
     // T(0) = T(0) - diff/2.0;
     // T(1) = T(1) + diff/2.0;
   }

   diff = T(0) - T(2) - smax;
   if (diff > 0  && T(0) > 1) {
     //INFO(3, "T3 "<<T);
     T(0) =T(0) - diff;
     // T(0) = T(0) - diff/2.0;
     // T(2) = T(2) + diff/2.0;
   }

  }
  
}



 //anisotropy
void Particule::setAnisotropyAxes(VEC3 x, VEC3 y, VEC3 z) {
  axes.col(0) = x;
  axes.col(2) = x.cross(y);
  axes.col(1) = axes.col(2).cross(x);

  for (uint i = 0; i < 3; ++i) {
    axes.col(i).normalize();
  }
  // axey.normalize();
  // axez.normalize();

  MAT3 R = axes;
  // R.col(0) = axex;
  // R.col(1) = axey;
  // R.col(2) = axez;

  //TODO check this
  rotation = R;
    
  MAT3 D;
  D.col(0) << valx, 0, 0;
  D.col(1) << 0, valy, 0;
  D.col(2) << 0, 0, valz;

  // ellipse = R.transpose()*D*R;
  ellipse = R*D;

}

void Particule::setAnisotropyRotation(MAT3 rot) {
   // axex = rot*axex;
   // axey = rot*axey;
   // axez = rot*axez;

  //INFO(3, "rotation\n"<<rot);
  
  rotation = rot;
  //  orientation = rot*orientation*rot.transpose();

   // for (uint i = 0; i < 3; ++i) {
   //   // orientation.col(i) = rot*orientation.col(i);//*rot.transpose();
   //   axes.col(i) = rot*axes.col(i);
   // }
  // MAT3 D;
  // D.col(0) << valx, 0, 0;
  // D.col(1) << 0, valy, 0;
  // D.col(2) << 0, 0, valz;

  // // ellipse = R.transpose()*D*R;
  // ellipse = rot*D;
}

void Particule::setAnisotropyValues(FLOAT vx, FLOAT vy, FLOAT vz) {
  valx = vx;
  valy = vy;
  valz = vz;
    
  MAT3 D;
  D.col(0) << valx, 0, 0;
  D.col(1) << 0, valy, 0;
  D.col(2) << 0, 0, valz;

  //  ellipse = R.transpose()*D*R;
  ellipse = rotation*D;

  //  VEC3 a;
  // a(0) = mpm_conf::anisotropy_values_(0);
  // a(1) = mpm_conf::anisotropy_values_(1);
  // a(2) = mpm_conf::anisotropy_values_(2) + 0.2;

  // setAnisotropyTensor(a);
}




void Particule::rotate(MAT3 rot) {
  IS_DEF(rot(0, 0));
  ellipse = rot*ellipse;

  // for (uint i = 0; i < 3; ++i) {
  //   axes.col(i) = rot*axes.col(i);
  // }
  
  // axex = rot*axex;
  // axey = rot*axey;
  // axez = rot*axez;

  rotation = rot*rotation;
  //  orientation = rot*orientation*rot.transpose();;
}

MAT3 Particule::getRotation() const {
   return rotation;
  //  return axes;
  //   return orientation;
}

VEC3 Particule::getAnisotropy() const {
  return VEC3(valx, valy, valz);
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




void Particule::anisotropicProject(VEC3 sigma, VEC3 &T, MAT3 V) {
 
  // MAT3 rotF = rotation.transpose()*F_e;
    // JacobiSVD<MATX> svd(rotF, ComputeThinU | ComputeThinV);
    // MAT3 U = svd.matrixU();
    // MAT3 V = svd.matrixV();
    // VEC3 sigma = svd.singularValues();

  if (mpm_conf::plastic_mode_ == 1) {
  VEC3 smax = mpm_conf::stretch_max;
   VEC3 smin = mpm_conf::stretch_min;

  // TEST(sigma(0) >= sigma(1));
  // TEST(sigma(1) >= sigma(2));
  //  INFO(3,sigma);
   //  INFO(3, "sigma : "<< sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
  for (uint i = 0; i < 3; ++i) {
    VEC3 v = sigma(i)*V.col(i);
    VEC3 vmin;
    VEC3 vmax;
    for (uint k = 0; k < 3; ++k) {
      vmax(k) = v(k) / smax(k);
      vmin(k) = v(k) / smin(k);
    }
    if (vmax.squaredNorm() > 1) {
      vmax.normalize();
      for (uint k = 0; k < 3; ++k) {
  	v(k) = vmax(k) * smax(k);
      }
      T(i) = v.norm();
    } else if (vmin.squaredNorm() < 1) {
      vmin.normalize();
      for (uint k = 0; k < 3; ++k) {
  	v(k) = vmin(k) * smin(k);
      }
      T(i) = v.norm();
    } else {
      T(i) = sigma(i);
    }
  }
  } else if (mpm_conf::plastic_mode_ == 2) {
    VEC3 smax = mpm_conf::stretch_max;
  //  VEC3 smin = Vector3d(1, 1, 1) - mpm_conf::stretch_min;
   // IS_DEF(sigma(0));
    for (uint i = 0; i < 3; ++i) {
      // if (sigma(i) > 1) {
      //   T(i) = 1;
      // } else {
       T(i) = sigma(i);
       //}
   }
  
   VEC3 lim;// = 0.0005;
   for (uint i = 0; i < 3; ++i) {
     VEC3 v = V.col(i);
     for (uint k = 0; k < 3; ++k) {
       v(k) = v(k)*smax(k);
     }
     lim(i) = v.norm();
     IS_DEF(lim(i));
   }
   FLOAT diff = T(1) - T(2) - lim(0);
   if (diff > 0) {
     //     //T(1) = sigma(1) - diff;
     T(1) = T(1) - diff/2.0;
     T(2) = T(2) + diff/2.0;
   }

   // v = U.col(2);
   // for (uint k = 0; k < 3; ++k) {
   //   v(k) = v(k)*smax(k);
   // }
   // lim = v.norm();
   
   diff = T(0) - T(1) - lim(2);
   if (diff > 0) {
     //     //T(1) = sigma(1) - diff;
     T(0) = T(0) - diff/2.0;
     T(1) = T(1) + diff/2.0;
   }

   // v = U.col(1);
   // for (uint k = 0; k < 3; ++k) {
   //   v(k) = v(k)*smax(k);
   // }
   //  lim = v.norm();
     diff = T(0) - T(2) - lim(1);
   if (diff > 0) {
     //     //T(1) = sigma(1) - diff;
     T(0) = T(0) - diff/2.0;
     T(2) = T(2) + diff/2.0;
   }
   
    // INFO(3, "sigma : "<< sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
    // INFO(3, "T : "<< T(0)<<" "<<T(1)<<" "<<T(2));
    // INFO(3, "lim : "<< lim(0)<<" "<<lim(1)<<" "<<lim(2));
    IS_DEF(T(0, 0));

  } else {
    TEST(false);
  }
  
 
  // // //0.0005 wokrs well with falling_cube_cylinder
  //   FLOAT diff = T(1) - T(2) - 0.0005;
  //   if (diff > 0) {
  //     //T(1) = sigma(1) - diff;
  //     T(1) = T(1) - diff/2.0;
  //     T(2) = T(2) + diff/2.0;
  //   }

  //   diff = T(0) - T(1) - 0.0005;
  //   if (diff > 0) {
  //     //T(0) = sigma(0) - diff;
  //     T(0) = T(0) - diff/2.0;
  //     T(1) = T(1) + diff/2.0;
  //   }

  //   diff = T(0) - T(2) - 0.0005;
  //   if (diff > 0) {
  //     //T(0) = sigma(0) - diff;
  //     T(0) = T(0) - diff/2.0;
  //     T(2) = T(2) + diff/2.0;
  //   }
}


// inline FLOAT Particule::energySecondDer(uint i, uint j, uint k, uint l) const {
//   uint ind = 27*i+9*k+3*j+l;
//   return energy_second_der[ind];
// }

// inline FLOAT & Particule::energySecondDer(uint i, uint j, uint k, uint l) {
//   uint ind = 27*i+9*k+3*j+l;
//   return energy_second_der[ind];
// }

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
FLOAT Particule::energySecondDer(VEC3 sigma, uint i, uint j) {
  TEST(false);
  FLOAT d = 0;
  // TEST(sigma(i) > 0);
  //TEST(sigma(2) > 0);
  // TEST(sigma(j) > 0);
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

void Particule::fix() {
  fixed = true;
  
}


void Particule::addToMesh(std::list<VEC3> & points, std::list<VEC3> & normals,
			   std::list<VEC2> & tex_points, std::list<unsigned int> & connectivity) {
  MAT3 D;
  D << valx, 0, 0,
    0, valy, 0,
    0, 0, valz;
  MAT3 S = rotation*D;

  Sphere sp(0.01, m_shader);
  sp.addToMesh(S, points, normals, tex_points, connectivity);
}


void Particule::exportMitsuba(std::ofstream &file) {
  ANGLE_AXIS rot(rotation);
  VEC3 axe = rot.axis();
  FLOAT angle = rot.angle()/M_PI*180;
  // file<<"<shape type=\"sphere\">\n";
  // //  file<<"<point name=\"center\" x=\""<<10*pos(0)<<"\" y=\""<<10*pos(1)<<"\" z=\""<<10*pos(2)<<"\"/>\n";
  // file<<"<point name=\"center\" x=\"0\" y=\"0\" z=\"0\"/>\n";
  // file<<"<transform name=\"toWorld\">\n";
  // //  file<<"<scale x=\""<<valx<<"\" y=\""<<valy<<"\" z=\""<<valz<<"\"/>\n";
  // file<<"<scale x=\"0.1\" y=\"0.1\" z=\"1\"/>\n";
  // file<<"<translate x=\""<<10*pos(0)<<"\" y=\""<<10*pos(1)<<"\" z=\""<<10*pos(2)<<"\"/>\n";
  // file<<"</transform>";
  // file<<"<ref name=\"bsdf\" id=\"particle\"/>\n";
  // file<<"</shape>\n";

  file<<"<shape type=\"instance\">\n";
  file<<"<ref id=\"particle\"/>\n";
  file<<"<transform name=\"toWorld\">\n";
  // file<<"<scale x=\""<<valx<<"\" y=\""<<valy<<"\" z=\""<<valz<<"\"/>\n";
  file<<"<scale x=\"0.02\" y=\"0.02\" z=\"0.02\"/>\n";
  file<<"<rotate x=\""<<axe(0)<<"\" y=\""<<axe(1)<<"\" z=\""<<axe(2)<<"\" angle=\""<<angle<<"\"/>\n";
  file<<"<translate x=\""<<10*pos(0)<<"\" y=\""<<10*pos(1)<<"\" z=\""<<10*pos(2)<<"\"/>\n";
  file<<"</transform>\n";
  // file<<"<ref name=\"bsdf\" id=\"particle\"/>\n";
  file<<"</shape>\n";
}


