#include "Object.hpp"
#include <cassert>
#include "Scene.hpp"

#ifndef NO_GRAPHICS_ 
#include "Texture.hpp"
#include "Shader.hpp"
#include "Scene.hpp"

using namespace glm;
#endif


Object::Object(int shader, int texture) {
  m_texture = texture;
  m_shader = shader;
#ifndef NO_GRAPHICS_ 
  m_model_view = glm::mat4(1.0f);
#endif
}

int Object::getTexture() const {
  return m_texture;
}

int Object::getShader() const {
  return m_shader;
}

#ifndef NO_GRAPHICS_ 
glm::mat4 Object::modelView() const {
  return m_model_view;
}
  
glm::mat4 & Object::modelView()  {
  return m_model_view;
}

void Object::setMVP(glm::mat4 m, int s) {
  int cur_shader = m_shader;
  if (m_shader == -1) {
    cur_shader = s;
  }
  assert(cur_shader != -1);
  Scene::SCENE->getShader(cur_shader)->enable();

  glm::mat4 vp;
  Scene::SCENE->getProjView(vp);
  glm::mat4 mvp = vp * m * m_model_view;
  glUniformMatrix4fv(glGetUniformLocation(Scene::SCENE->getShader(cur_shader)->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));

  if (m_texture != -1) {
    glBindTexture(GL_TEXTURE_2D, Scene::SCENE->getTexture(m_texture)->getID());
  }
}

void Object::enableShader() {
  // if (m_shader != NULL) {
  //   m_shader->enable();
  //   // glm::mat4 vp;
  //   // Scene::SCENE->getProjView(vp);
  //   // glm::mat4 mvp = vp * m_model_view;
  //   // glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));
  // }

}

void Object::disableShader() {
  if (m_texture != -1) {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  if (m_shader != -1) {
    Scene::SCENE->getShader(m_shader)->disable();
  }
}

#endif
