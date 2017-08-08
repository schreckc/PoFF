#include "Particule.hpp"
#include "Sphere.hpp"
#include "Scene.hpp"
#include "Grid.hpp"
#include "Simulation.hpp"
#include "error.hpp"

#include <iostream>

Particule::Particule(int shader) : Object(shader) {
  pos = VEC3(0, 0, 0);
}

Particule::Particule(FLOAT mass, FLOAT vol, VEC3 p, VEC3 n, VEC3 velo, int shader) :
  Object(shader), m(mass), v(vol), v0(vol), pos(p), normal(n), vel(velo), F_p(MAT3::Identity(3, 3)), F_e(MAT3::Identity(3, 3)), F(MAT3::Identity(3, 3)) {
  FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(p(0)/h), (int)(p(1)/h), (int)(p(2)/h));

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

  axex = VEC3(1, 0, 0);
  axey = VEC3(0, 1, 0);
  axez = VEC3(0, 0, 1);

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
 
    
    VEC3 x = rotation*axex;
    VEC3 y = rotation*axey;
    VEC3 z = rotation*axez;
    
      //  x.normalize();
      //  y.normalize();
      //  z.normalize();
   
      glm::mat3 R;
      R[0] = glm::vec3(x[0], x[1], x[2]);
      R[1] = glm::vec3(y[0], y[1], y[2]);
      R[2] = glm::vec3(z[0], z[1], z[2]);
    
      glm::mat3 D;
      D[0] = glm::vec3(valx, 0, 0);
      D[1] = glm::vec3(0, valy, 0);
      D[2] = glm::vec3(0, 0, valz);
    

      glm::mat3 S = R*D;
      glm::mat4 S4 = glm::mat4(S);

    // // //       HouseholderQR<MAT3> decomp(ellipse);
    // MAT3 Q =  ellipse;//decomp.householderQ();
    // glm::mat3 S(Q(0, 0), Q(1, 0), Q(2, 0),
    // 		Q(0, 1), Q(1, 1), Q(2, 1),
    // 		Q(0, 2), Q(1, 2), Q(2, 2));
    // glm::mat4 S4 = glm::mat4(S);

    // MAT3 Q;
    // Q <<1, 0, 0,
    //   0, 0, -1,
    //   0, 1, 0;
    // Q *= ellipse;
    //  glm::mat3 S(Q(0, 0), Q(1, 0), Q(2, 0),
    //      		Q(0, 1), Q(1, 1), Q(2, 1),
    //      		Q(0, 2), Q(1, 2), Q(2, 2));
    //        glm::mat4 S4 = glm::mat4(S);

    //INFO(3, x<<"\n\n"<<y<<"\n\n"<<z);
    
    Sphere sp(0.01, m_shader);
    sp.setColor(color(0), color(1), color(2));
    int cur_shader = m_shader;
    if (m_shader == -1) {
      cur_shader = s;
    }
    glm::mat4 cur_model = m * m_model_view * S4;
    
    sp.draw(cur_model, cur_shader);

  
  //   glLineWidth(3.0f);
  //   Shader *tmp = m_shader;
  //   // m_shader = Scene::SCENE->getShader(2);
  // VEC3 n = F_e*F_p*normal;
  // n.normalize();
  // GLfloat n_line[6] = {0, 0, 0, 0.05*n(0), 0.05*n(1), 0.05*n(2)};
  // GLfloat n_color[6] = {0, 0, 0, 0, 0, 0};
  //   enableShader();
  //   setMVP(m, s);
  
  //  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, n_line);
  //  glEnableVertexAttribArray(0);

  //  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, n_color);
  //  glEnableVertexAttribArray(1);

  //  glDrawArrays(GL_LINES, 0, 2);

  //  glDisableVertexAttribArray(0);
  //  glDisableVertexAttribArray(1);

  //  disableShader();
  //  m_shader = tmp;
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
  glLineWidth(5.0f);

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

FLOAT Particule::getVolume() const {
  return v0;
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


void Particule::setDensity(FLOAT d) {
  density = d;
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

MAT3 Particule::getDeformationPlastic() const {
  return F_p;
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

void Particule::update(VEC3 & p, VEC3 & v, MAT3 & b, MAT3 & t) {
  IS_DEF(t(0, 0));
  if (mpm_conf::method_ == mpm_conf::apic_) {
    pos = p;
  } else {
    pos += mpm_conf::dt_*v;
  }
  IS_DEF(pos(0));
  IS_DEF(pos(1));
  IS_DEF(pos(2));
  // INFO(3, "vel prev\n"<<vel);
  // INFO(3, "vel new\n"<<v);
  vel = v;
   FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(pos(0)/h), (int)(pos(1)/h), (int)(pos(2)/h));
  B = b;
  F_e *= (MAT3::Identity() + mpm_conf::dt_*t);

  
  if (std::isnan(F_e(0, 0)) || std::isinf(F_e(0,0))) {
    F_e = MAT3::Identity();
  }
  MAT3 rotF = rotation.transpose()*(F_e)*rotation;
  MAT3 rotFx = 0.5*(rotF + rotF.transpose());
  MAT3 skewF = 0.5*(rotF - rotF.transpose());
  MAT3 isoF = /*rotF  + MAT3::Identity();;*/innerProduct(mpm_conf::anisotropy_strain_, rotFx) - innerProduct(mpm_conf::anisotropy_strain_,  MAT3::Identity()) + MAT3::Identity();
  //  isoF = 0.5*(isoF + isoF.transpose());

  // MAT3 isoF2 = innerProduct(mpm_conf::anisotropy_strain_, rotF) - innerProduct(mpm_conf::anisotropy_strain_,  MAT3::Identity()) + MAT3::Identity();
  // isoF2 = 0.5*(isoF2 + isoF2.transpose());
  // if ((isoF - isoF2).maxCoeff() > 1e-15 ||(isoF - isoF2).minCoeff() < -1e-15 ) {
  //   INFO(3, "iso\n"<<isoF - isoF2);
  // }
  //  INFO(3, "iso 2\n"<<isoF2);
  // TEST(isoF == isoF2);
  
  JacobiSVD<MATX> svd(isoF, ComputeThinU | ComputeThinV);
  MAT3 U = svd.matrixU();
  MAT3 V = svd.matrixV();
  VEC3 T(0, 0, 0);
  VEC3 sigma = svd.singularValues();
  MAT3 der = MAT3::Zero();

  //INFO(3,"sigma"<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));

  if (std::isnan(sigma(0)) || std::isinf(sigma(0))) {
    sigma = VEC3(1, 1, 1);
  }
  IS_DEF(sigma(0));
  
  //elsastoplastic
  //INFO(3, "plastic "<<mpm_conf::plastic_mode_);
  if (mpm_conf::plastic_mode_ != 3) {  
    
    // if (mpm_conf::plastic_anisotropy_) {
    //   anisotropicProject(sigma, T, V);
    //  } else {
      project(sigma, T);
      // }
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
    MAT3 isoF = U*(T_m)*V.transpose() - MAT3::Identity();
    MAT3 anisF = /*isoF +  MAT3::Identity();*/innerProduct(mpm_conf::inv_anisotropy_strain_, isoF) +  MAT3::Identity();
    // INFO(3, "isoF\n"<<isoF  +  MAT3::Identity());
    // INFO(3, "anisoF\n"<<anisF);
    F_e = rotation*(anisF + skewF)*rotation.transpose();
    F_p = F_e.inverse()*F;//rotation*V*inv_T*sigma_m*V.transpose()* rotation.transpose()*F_p;
    F = F_e*F_p;

    if (mpm_conf::plastic_mode_ == 0) {
       FLOAT fric_angle = mpm_conf::hardenning_param_(0) + (mpm_conf::hardenning_param_(1)*hardenning - mpm_conf::hardenning_param_(3))*exp(-mpm_conf::hardenning_param_(2)*hardenning);
      fric_angle = fric_angle*M_PI/180; //convert radian
      alpha = sqrt(2.0/3.0)*(2*sin(fric_angle))/(3-sin(fric_angle));
    } else if (mpm_conf::plastic_mode_ == 1) {
      alpha = exp(mpm_conf::hardenning_param_(3)*(1-det_Fp));
    }

    //  INFO(3, "hardenning "<< alpha);

    // // INFO(3, F_p);
    IS_DEF(rotation(0,0));
    IS_DEF(T(0));
    IS_DEF(F_e(0, 0));
  } else { //plastic
    computeEnergyDerivative(sigma);
    MAT3 der = MAT3::Zero();
    for (uint i = 0; i < 3; ++i) {
      der(i, i) = energy_der(i);
    }
  }
  if (mpm_conf::elastic_mode_ != 2) {
    MAT3 iso_sigma = U*der*V.transpose()*isoF.transpose();
    MAT3 anis_sigma = /*iso_sigma;*/innerProduct(mpm_conf::inv_anisotropy_stress_, iso_sigma);
    //forceIncrement = v0*rotation*U*der*V.transpose()*rotation.transpose()*F_e.transpose();
    forceIncrement = v0*rotation*anis_sigma*rotation.transpose();
  } else {
    forceIncrement = v0*F_e.determinant()*linearElasticity();
  }
  if (std::isnan(forceIncrement(0, 0)) || std::isinf(forceIncrement(0, 0))) {
    forceIncrement = MAT3::Zero();
  }
  if (mpm_conf::implicit_) {
    computeEnergySecondDer(sigma, U, V);
  }
}

void Particule::initVolume(FLOAT d) {
  density = d;
  v0 = m/d;
  //INFO(3, "denisty part, vol "<< density <<" "<<v0<<" "<<m/d);
  //  INFO(3,"volume "<<v0<<" "<<d);
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

  MAT3 rotFx = rotation.transpose()*(F_e)*rotation;
  //MAT3 rotF = 0.5*(isoF + isoF.transpose());
  MAT3 rotF = 0.5*(rotFx + rotFx.transpose());
  MAT3 isoF = innerProduct(mpm_conf::anisotropy_strain_, rotF) - innerProduct(mpm_conf::anisotropy_strain_,  MAT3::Identity());
  rotF = isoF;//0.5*(isoF + isoF.transpose());
  
  //INFO(3, "\nrot F\n"<<rotF<<"\n\n"<<isoF);
  VECX strain(6);
  VECX stress(6);

  strain(0) = rotF(0, 0);
  strain(1) = rotF(1, 1);
  strain(2) = rotF(2, 2);
  strain(3) = rotF(1, 2);
  strain(4) = rotF(0, 2);
  strain(5) = rotF(0, 1);
   
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
  MAT3 anis_sigma = innerProduct(mpm_conf::inv_anisotropy_stress_, sigma);
  return rotation*anis_sigma*rotation.transpose();
}


void Particule::project(VEC3 sigma, VEC3 & T) {
  //INFO(3, "PROJECT");
  if (mpm_conf::plastic_mode_ == 0) {
    VEC3 ln_sigma(0, 0, 0);
    VEC3 dev_ln_sigma(0, 0, 0);
    FLOAT tr = 0;
    FLOAT fr_norm = 0;
    FLOAT plastic_def = 0;
    for (uint i = 0; i < 3; ++i) {
      if (sigma(i) != 0) {
	ln_sigma(i) = log(sigma(i));
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
      //     INFO(3, "T "<<T);
    //  } else {
    //   // INFO(3,"density "<<mpm_conf::density_<<"   denstity local "<<density);
    //   color = VEC3(0.5, 0.5, 1);
    //   for (uint i = 0; i < 3; ++i) {
    //    // 	 T(i) = sigma(i);
    //    // 	 if (sigma(i) < smin/10.0) {
    // 	T(i) = 1;
    // 	 // } else if (sigma(i) > smax/10.0) {
    // 	 //   //T(i) = smax;
    // 	 //   T(i) = 1;
    // 	//}
    //   }
    // }
      
  }
  
}



 //anisotropy
void Particule::setAnisotropyAxes(VEC3 x, VEC3 y, VEC3 z) {
  axex = x;
  axez = x.cross(y);
  axey = axez.cross(x);
  
  axex.normalize();
  axey.normalize();
  axez.normalize();

  MAT3 R;
  R.col(0) = axex;
  R.col(1) = axey;
  R.col(2) = axez;

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
  
  rotation = rot;
    
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
}




void Particule::rotate(MAT3 rot) {
  IS_DEF(rot(0, 0));
  ellipse = rot*ellipse;

  axex = rot*axex;
  axey = rot*axey;
  axez = rot*axez;

  rotation = rot*rotation;
}

MAT3 Particule::getRotation() const {
  return rotation;
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
    
