#ifndef NO_GRAPHICS_

#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "Object.hpp"

#include <glm/gtx/transform.hpp>

class Skybox : public Object {

private:
  float size;

  const static float vertices[108];
  const static float normals[108];
  const static float couleurs[108];
  const static float coordTexture[72];

  glm::vec3 position;

public:
  Skybox(float s = 1, int shader = -1, int texture = -1);
  ~Skybox();

  void animate();
  void draw(glm::mat4 m = glm::mat4(1.0f),  int s = -1);

  void setPosition(glm::vec3 pos);
};

#endif

#endif
