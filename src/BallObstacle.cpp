#include "BallObstacle.hpp"
#include "error.hpp"
#include "Sphere.hpp"

BallObstacle::BallObstacle(VEC3 p, FLOAT r, int shader) : Obstacle(shader){
  pos = p;
  ray = r;
}

BallObstacle::~BallObstacle() {
}

void BallObstacle::apply(Motion m) {
  pos += m.translation;
  ray *= m.scale;
  //  ray -= 0.0005;
  //  INFO(3, "ray "<<ray);
}

#ifndef NO_GRAPHICS_ 
void BallObstacle::draw(glm::mat4 m, int s) {
  
  m_model_view = translate(glm::mat4(1.0f), glm::vec3(pos(0), pos(1), pos(2)));
 Sphere sp(ray, m_shader);
 int cur_shader = m_shader;
    if (m_shader == -1) {
      cur_shader = s;
    }
    glm::mat4 cur_model = m * m_model_view;
    sp.draw(cur_model, cur_shader);
}
#endif

VEC3 BallObstacle::getPosition() const {
  return pos;
}

FLOAT BallObstacle::getRay() const {
  return ray;
}


VEC3 BallObstacle::getNormal(VEC3 p) const {
   VEC3 n = p - pos;
   n.normalize();
   return n;
}

FLOAT BallObstacle::distance(VEC3 v) const {
  VEC3 n = v - pos;
  return n.norm() - ray;
}

void BallObstacle::getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const {
  VEC3 n = p - pos;
  FLOAT norm = n.norm();
  assert(norm != 0);
  n /= norm;

  normal = n;
  dist = norm - ray;
}

void BallObstacle::exportMitsuba(std::ofstream & file) const {
  TEST(false); //TODO
}
