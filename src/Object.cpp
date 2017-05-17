#include "Object.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>

#include "Scene.hpp"

using namespace glm;

Object::Object(Shader *shader, Texture *texture) {
  m_texture = texture;
  m_shader = shader;
  m_model_view = glm::mat4(1.0f);
}

Texture* Object::getTexture() const {
  return m_texture;
}

Shader* Object::getShader() const {
  return m_shader;
}

glm::mat4 Object::modelView() const {
  return m_model_view;
}
  
glm::mat4 & Object::modelView()  {
  return m_model_view;
}

void Object::setMVP(glm::mat4 m, Shader *s) {
  Shader *cur_shader = m_shader;
  if (m_shader == NULL) {
    cur_shader = s;
    cur_shader->enable();
  }

  assert(cur_shader != NULL);
  glm::mat4 vp;
  Scene::SCENE->getProjView(vp);
  glm::mat4 mvp = vp * m * m_model_view;
  glUniformMatrix4fv(glGetUniformLocation(cur_shader->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));
  
}

void Object::enableShader() {
  if (m_shader != NULL) {
    m_shader->enable();
    // glm::mat4 vp;
    // Scene::SCENE->getProjView(vp);
    // glm::mat4 mvp = vp * m_model_view;
    // glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));
  }
  if (m_texture != NULL) {
    glBindTexture(GL_TEXTURE_2D, m_texture->getID());
  }
}

void Object::disableShader() {
  if (m_texture != NULL) {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  if (m_shader != NULL) {
    m_shader->disable();
  }
}
