#include "Obstacle.hpp"
#include "mpm_conf.hpp"
#include "Scene.hpp"
#include "error.hpp"

Obstacle::Obstacle(int shader) : Object(shader) {
  friction = mpm_conf::friction_coef_;
  rotation = MAT3::Identity();
}

void Obstacle::animate() {
  for (auto& m : motions) {
    apply(m);
  }
}

void Obstacle::animate(FLOAT t) {
  for (auto& m : motions) {
    if (m.isRunning(t)) {
      apply(m);
    }
  }
}

FLOAT Obstacle::getFriction() const {
  return friction;
}

void Obstacle::setFriction(FLOAT f) {
  friction = f;
}

void Obstacle::setMotions(std::list<Motion> lm) {
  motions = lm;
}

void Obstacle::addMotion(Motion m) {
  motions.push_back(m);
}

bool Obstacle::isMoving() const {
  return !motions.empty();
}
