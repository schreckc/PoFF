#include "Subparticule.hpp"
#include "Sphere.hpp"
#include "error.hpp"
#include "Eigen/Geometry" 
#include <fstream>

Subparticule::Subparticule(int shader) : Object(shader) {
  pos = VEC3(0, 0, 0);
}

Subparticule::Subparticule(FLOAT m, VEC3 p, VEC3 velo, int shader) : Object(shader), mass(m), pos(p), vel(velo) {
  FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(p(0)/h), (int)(p(1)/h), (int)(p(2)/h));
  rotation = MAT3::Identity();
}

    
void Subparticule::animate() {}

  #ifndef NO_GRAPHICS_ 
void Subparticule::draw(glm::mat4 m, int s) {
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

      glm::mat3 D;
      D[0] = glm::vec3(0.2, 0, 0);
      D[1] = glm::vec3(0, 0.2, 0);
      D[2] = glm::vec3(0, 0, 0.2);

      glm::mat3 S = R*D;
      //glm::mat3 S = R;
      glm::mat4 S4 = glm::mat4(S);
       cur_model = cur_model * S4;
    
      Sphere sp(0.01, m_shader);
      sp.setColor(0, 1, 1);
      
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
  GLfloat vertex[6] = {0, 0, 0};
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
  
  // if (Scene::SCENE->running) {
  //    color = VEC3(1, 1, 1);
  // } else {
  //   color = VEC3(0.5, 1, 0.5);
  // }
}
  #endif
  
VEC3 Subparticule::getPosition() const {
  return pos;
}

void Subparticule::setPosition(VEC3 & p) {
  pos = p;
}

VEC3 Subparticule::getVelocity() const {
  return vel;
}

void Subparticule::setVelocity(VEC3 & velo) {
  vel = velo;
}

FLOAT Subparticule::getMass() const {
  return mass;
}

void Subparticule::setMass(FLOAT m) {
  mass = m;
}

Vector3i Subparticule::getCell() const {
  return cell;
}

void Subparticule::rotate(MAT3 & rot) {
  rotation = rot*rotation;
}

MAT3 Subparticule::getRotation() const {
  return rotation;
}

void Subparticule::update(VEC3 &p, VEC3 &v) {
  // INFO(3, " prev pos "<<pos(0)<<" "<<pos(1)<<" "<<pos(2));
  // INFO(3, "prev vel "<<vel(0)<<" "<<vel(1)<<" "<<vel(2));
  if (mpm_conf::method_ == mpm_conf::apic_) {
    pos = p;
  } else {
    pos += mpm_conf::dt_*v;
  }
  vel = v;
  // INFO(3, "pos "<<pos(0)<<" "<<pos(1)<<" "<<pos(2));
  // INFO(3, "vel "<<v(0)<<" "<<v(1)<<" "<<v(2));
  FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(pos(0)/h), (int)(pos(1)/h), (int)(pos(2)/h));   
}

FLOAT Subparticule::weight(Vector3i node) {
  FLOAT w = 1;
  FLOAT absx = 0;
  for (uint i = 0; i < 3; ++i) {
    if (cell(i) == node(i) || cell(i) == node(i) - 1) {
      absx = fabs(pos(i)/mpm_conf::grid_spacing_ -  (FLOAT)node(i));
      w *= 0.5*pow(absx, 3) - pow(absx, 2) + 2.0/3.0;
    } else if (cell(i) == node(i) + 1 || cell(i) == node(i) - 2) {
      absx = fabs(pos(i)/mpm_conf::grid_spacing_ -  (FLOAT)node(i));
      w *= 1.0/6.0*pow(2 - absx, 3);
    } else {
      w = 0;
      break;
    }
  }
  return w;
}

void Subparticule::eulerStep(VEC3 forces) {
  // INFO(3, "prev vel "<<vel(0)<<" "<<vel(1)<<" "<<vel(2));
  // INFO(3, "prev pos "<<pos(0)<<" "<<pos(1)<<" "<<pos(2));
  VEC3 a = forces/mass + mpm_conf::gravity_;
  pos += mpm_conf::dt_*vel;
  vel += mpm_conf::dt_*a;
  FLOAT h = mpm_conf::grid_spacing_;
  cell = Vector3i((int)(pos(0)/h), (int)(pos(1)/h), (int)(pos(2)/h));   
  // INFO(3, "new vel "<<vel(0)<<" "<<vel(1)<<" "<<vel(2));
  // INFO(3, "new pos "<<pos(0)<<" "<<pos(1)<<" "<<pos(2));
}

void Subparticule::exportMitsuba(std::ofstream &file) {
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
  //  file<<"<ref name=\"bsdf\" id=\"particle\"/>\n";
  file<<"</shape>\n";
}
