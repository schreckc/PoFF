#ifndef OBJECT_HPP
#define OBJECT_HPP

#ifndef  NO_GRAPHICS_ 

#include <glm/glm.hpp>
#ifdef WIN32
#include <GL/glew.h>

#else
#define GL3_PROTOTYPES 1
#include <GLES3/gl3.h>

#endif

 #endif

//#include "Texture.hpp"
//#include "Shader.hpp"

class Object {
  
protected:
  int m_texture;
  int m_shader;
  //glm::mat4 m_model_view;

public:
Object(int shader = -1, int texture = -1);

  virtual void animate() = 0;

#ifndef  NO_GRAPHICS_ 
  virtual void draw( glm::mat4 m = glm::mat4(1.0f), int s = -1) = 0;
  void setMVP(glm::mat4 m, Shader *s); 
  
  void enableShader();
  void disableShader();
  
  glm::mat4 modelView() const;
  glm::mat4 & modelView();  
#endif

  int getTexture() const;
  int getShader() const;
};

#endif
