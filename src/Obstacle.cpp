#include "Obstacle.hpp"
#include "mpm_conf.hpp"
#include "Scene.hpp"
#include "error.hpp"

Obstacle::Obstacle(int shader) : Object(shader) {
  friction = mpm_conf::friction_coef_;
}

void Obstacle::animate() {
  for (auto& m : motions) {
    if (m.isRunning(Scene::SCENE->getTime())) {
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
