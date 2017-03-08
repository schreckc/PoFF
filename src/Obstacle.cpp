#include "Obstacle.hpp"
#include "mpm_conf.hpp"

Obstacle::Obstacle(Shader *shader) : Object(shader) {
  friction = mpm_conf::friction_coef_;
}

FLOAT Obstacle::getFriction() const {
  return friction;
}

void Obstacle::setFriction(FLOAT f) {
  friction = f;
}
