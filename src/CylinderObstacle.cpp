#include "CylinderObstacle.hpp"
#include "Cylinder.hpp"
#include "error.hpp"
#include "utils.hpp"
#include <fstream>

CylinderObstacle::CylinderObstacle(VEC3 p, VEC3 d, FLOAT r, int shader) : Obstacle(shader){
  pos = p;
  dir = d;
  ray = r;

  dir.normalize();
}

CylinderObstacle::~CylinderObstacle() {
}

void CylinderObstacle::apply(Motion m) {
  pos += m.translation;
  dir = m.rotation*dir;
  ray *= m.scale;

}

#ifndef NO_GRAPHICS_ 
void CylinderObstacle::draw(glm::mat4 m, int s) {
  Cylinder c(ray, 1, m_shader);

  VEC3 axe = dir.cross(VEC3(0, 0, 1));
  FLOAT norm = axe.norm();
  FLOAT angle = asin(norm);
  if (fabs(norm) > 0.0001) {
    axe /= norm;
  } else {
    axe = VEC3(0, 0, 1);
  }

  glm::vec3 axis((GLfloat)axe(0), (GLfloat)axe(1), (GLfloat)axe(2));
  
  m_model_view = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)))*
    rotate(glm::mat4(1.0f), (GLfloat)angle, axis);

   int cur_shader = m_shader;
    if (m_shader == -1) {
      cur_shader = s;
    }
    glm::mat4 cur_model = m * m_model_view;
    c.draw(cur_model, cur_shader);
}
#endif

VEC3 CylinderObstacle::getNormal(VEC3 p) const {
  VEC3 proj = utils::projectionOrtho(p, pos, dir);
  VEC3 n = p - proj;
  n.normalize();
  return n;
}

FLOAT CylinderObstacle::distance(VEC3 v) const {
  VEC3 proj = utils::projectionOrtho(v, pos, dir);
  VEC3 n = v - proj;
  return n.norm() - ray;
}

void CylinderObstacle::getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const {
  VEC3 proj = utils::projectionOrtho(p, pos, dir);
  VEC3 n = p - proj;
  FLOAT norm = n.norm();

  normal = n/norm;
  dist = norm - ray;
}

void CylinderObstacle::exportMitsuba(std::ofstream & file) const {
  VEC3 p0 = pos - 10*dir;
  VEC3 p1 = pos + 10*dir;

  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"</shape>\n";
}
