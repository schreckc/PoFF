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
  Object(shader), m(mass), v(vol), v0(vol), pos(p), normal(n), vel(velo), F_p(MAT3::Identity(3, 3)), F_e(MAT3::Identity(3, 3)) {
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
  return v;
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
  vel = v;
   FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(pos(0)/h), (int)(pos(1)/h), (int)(pos(2)/h));
  B = b;
  F_e *= (MAT3::Identity() + mpm_conf::dt_*t);

  //ellipse += mpm_conf::dt_*(ellipse*t.transpose() + t*ellipse);

  //INFO(3, "vel "<<vel(0)<<", "<<vel(1)<<", "<<vel(2));
  //INFO(3, "pos "<<pos(0)<<", "<<pos(1)<<", "<<pos(2));
  // INFO(3, cell);//((int)(p(0)/Grid::spacing))<<", "<< ((int)(p(1)/Grid::spacing))<<", "<< ((int)(p(2)/Grid::spacing)));
  //INFO(3, F_e);
   
  // EigenSolver<MAT3> solver(F_e);
  // INFO(3, solver.eigenvalues());
  //  VEC3 ev = solver.eigenvalues();

  //  rotation = MAT3::Identity();
  //IS_DEF(rotation(0, 0));
  if (std::isnan(F_e(0, 0)) || std::isinf(F_e(0,0))) {
    F_e = MAT3::Identity();
  }
  MAT3 rotF = rotation.transpose()*F_e;
  // IS_DEF(F_e(0, 0));
  // IS_DEF(rotF(0, 0));
  JacobiSVD<MATX> svd(rotF, ComputeThinU | ComputeThinV);
  MAT3 U = svd.matrixU();
  MAT3 V = svd.matrixV();
  VEC3 T(0, 0, 0);
  VEC3 sigma = svd.singularValues();
  // INFO(3, "sigma "<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2)<<"\n"<<T(0)<<" "<<T(1)<<" "<<T(2));
  if (std::isnan(sigma(0)) || std::isinf(sigma(0))) {
    sigma = VEC3(1, 1, 1);
  }
  IS_DEF(sigma(0));
  if (mpm_conf::plasticity_) {  
    
    //project(sigma, T);
     anisotropicProject(sigma, T, U);
    //    INFO(3, "sigma "<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2)<<"\n"<<T(0)<<" "<<T(1)<<" "<<T(2));
    
    MAT3 inv_T = MAT3::Zero();
    MAT3 T_m =  MAT3::Zero();
    MAT3 sigma_m = MAT3::Zero();
    double det_Fp = 1;
  
    for (uint i = 0; i < 3; ++i) {
      T_m(i, i) = T(i);
      TEST(T(i) != 0);
      if (T(i) != 0) {
  	inv_T(i, i) = 1.0/T(i);
  	det_Fp *= sigma(i)*inv_T(i, i);
      } else {
  	det_Fp = 1;
      }
      sigma_m(i, i) = sigma(i);
    }
    F_e = rotation*U*(T_m)*V.transpose();
    F_p = rotation*V*inv_T*sigma_m*V.transpose()*F_p;


    if (mpm_conf::mode_ == 0) {
      // hardenning += plastic_def;
      // FLOAT h0 = 35, h1 = 0, h2 = 0.02, h3 = 10;
      FLOAT fric_angle = mpm_conf::hardenning_param_(0) + (mpm_conf::hardenning_param_(1)*hardenning - mpm_conf::hardenning_param_(3))*exp(-mpm_conf::hardenning_param_(2)*hardenning);
      fric_angle = fric_angle*M_PI/180; //convert radian
      alpha = sqrt(2.0/3.0)*(2*sin(fric_angle))/(3-sin(fric_angle));
    } else if (mpm_conf::mode_ == 1) {
      alpha = 1;//exp(mpm_conf::hardenning_param_(3)*(1-det_Fp));
    }

    //  INFO(3, "hardenning "<< alpha);

    // // INFO(3, F_p);
    IS_DEF(rotation(0,0));
    IS_DEF(T(0));
    IS_DEF(energyDerivative(T)(0, 0));
    IS_DEF(F_e(0, 0));
    forceIncrement = v0*rotation*U*energyDerivative(T)*V.transpose()*F_e.transpose();
    if (std::isnan(forceIncrement(0, 0)) || std::isinf(forceIncrement(0, 0))) {
      forceIncrement = MAT3::Zero();
    }
    IS_DEF(forceIncrement(0, 0));
  } else {
    forceIncrement = v0*U*energyDerivative(sigma)*V.transpose()*F_e.transpose();
  }
  
  // INFO(3, "force incr\n "<<forceIncrement);
}

void Particule::initVolume(FLOAT d) {
  density = d;
  v0 = m/d;
  //INFO(3, "denisty part, vol "<< density <<" "<<v0<<" "<<m/d);
  //  INFO(3,"volume "<<v0<<" "<<d);
}

MAT3 Particule::energyDerivative(VEC3 sigma) {
  IS_DEF(sigma(0));
  IS_DEF(sigma(1));
  IS_DEF(sigma(2));
  if (mpm_conf::mode_ == 0) {
    MAT3 invSigma = MAT3::Zero();
    MAT3 lnSigma = MAT3::Zero();
    FLOAT tr = 0;
    for (uint i = 0; i < 3; ++i) {
      FLOAT ev = sigma(i);
      if (ev != 0) {
	invSigma(i, i) = 1.0/ev;
	lnSigma(i, i) = log(ev);
	tr += log(ev);
	IS_DEF(invSigma(i, i));
	IS_DEF(lnSigma(i, i));
      }
    }
    return 2*mpm_conf::mu_*invSigma*lnSigma + mpm_conf::lambda_*tr*invSigma;
  } else if (mpm_conf::mode_ == 1) {
    //   INFO(3, "sigma "<<sigma(0)<<" "<<sigma(1)<<" "<<sigma(2));
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
    MAT3 der = MAT3::Zero();
    for (uint i = 0; i < 3; ++i) {
      IS_DEF(alpha);
      der(i, i) = 2*mpm_conf::mu_*alpha*(sigma(i)-1) + mpm_conf::lambda_*alpha*sigma((i+1)%3)*sigma((i+2)%3)*(det -1);
      if (std::isnan(der(i, i)) || std::isinf(der(i, i))) {
	der(i, i) = 0;
      }
      IS_DEF(der(i, i));
    }
    return der;
  }
  return MAT3::Zero();
}


void Particule::project(VEC3 sigma, VEC3 & T) {
  if (mpm_conf::mode_ == 0) {
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
  } else if (mpm_conf::mode_ == 1) {
    double smax = mpm_conf::hardenning_param_(1);//1 + 7.5e-3;
    double smin = mpm_conf::hardenning_param_(2);//1 - 2.5e-2;
    //if (density > 0.9*mpm_conf::density_) {
      //      INFO(3,"density "<<mpm_conf::density_<<"   denstity local "<<density);
    
      for (uint i = 0; i < 3; ++i) {
	T(i) = sigma(i);
	if (sigma(i) < smin) {
	  T(i) = smin;
	  //  T(i) = 1;
	} else if (sigma(i) > smax) {
	  T(i) = smax;
	  //T(i) = 1;
	}
      }
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

void Particule::anisotropicProject(VEC3 sigma, VEC3 &T, MAT3 U) {
 
  // MAT3 rotF = rotation.transpose()*F_e;
   // JacobiSVD<MATX> svd(rotF, ComputeThinU | ComputeThinV);
   // MAT3 U = svd.matrixU();
   // MAT3 V = svd.matrixV();
   // VEC3 sigma = svd.singularValues();

   VEC3 smax = mpm_conf::stretch_max;
   VEC3 smin = Vector3d(1, 1, 1) - mpm_conf::stretch_min;

  // // TEST(sigma(0) >= sigma(1));
  // // TEST(sigma(1) >= sigma(2));
  // //  INFO(3,sigma);
  
  // for (uint i = 0; i < 3; ++i) {
  //   VEC3 v = sigma(i)*U.col(i);
  //   VEC3 vmin;
  //   VEC3 vmax;
  //   for (uint k = 0; k < 3; ++k) {
  //     vmax(k) = v(k) / smax(k);
  //     vmin(k) = v(k) / smin(k);
  //   }
  //   if (vmax.squaredNorm() > 1) {
  //     vmax.normalize();
  //     for (uint k = 0; k < 3; ++k) {
  // 	v(k) = vmax(k) * smax(k);
  //     }
  //     T(i) = v.norm();
  //   } else if (vmin.squaredNorm() < 1) {
  //     vmin.normalize();
  //     for (uint k = 0; k < 3; ++k) {
  // 	v(k) = vmin(k) * smin(k);
  //     }
  //     T(i) = v.norm();
  //   } else {
  //     T(i) = sigma(i);
  //   }
  // }

   IS_DEF(sigma(0));
   for (uint i = 0; i < 3; ++i) {
      // if (sigma(i) > 1) {
      //   T(i) = 1;
      // } else {
       T(i) = sigma(i);
       //}
   }
  
   VEC3 lim;// = 0.0005;
   for (uint i = 0; i < 3; ++i) {
     VEC3 v = U.col(i);
     for (uint k = 0; k < 3; ++k) {
       v(k) = v(k)*smin(k);
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
   //   v(k) = v(k)*smin(k);
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
   //   v(k) = v(k)*smin(k);
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

