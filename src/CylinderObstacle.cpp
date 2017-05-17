#include "CylinderObstacle.hpp"
#include "Cylinder.hpp"
#include "error.hpp"
#include "utils.hpp"
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/transform.hpp>

CylinderObstacle::CylinderObstacle(VEC3 p, VEC3 d, FLOAT r, int shader) : Obstacle(shader){
  pos = p;
  dir = d;
  ray = r;

  dir.normalize();
}

CylinderObstacle::~CylinderObstacle() {
}

void CylinderObstacle::animate() {
  //  ray -= 0.0005;
  //  INFO(3, "ray "<<ray);
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
  //  INFO(3, "angle "<<angle<<" "<<norm<<"\n"<<axe);
  
  m_model_view = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)))*
    rotate(glm::mat4(1.0f), (GLfloat)angle, axis);

   int cur_shader = m_shader;
    if (m_shader == -1) {
      cur_shader = s;
    }
    glm::mat4 cur_model = m * m_model_view;
    c.draw(cur_model, cur_shader);
    // INFO(3, "ray "<<ray);
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

  normal = n;
  dist = norm - ray;
}
