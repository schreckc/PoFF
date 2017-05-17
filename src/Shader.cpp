#ifndef NO_GRAPHICS_ 

#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <vector>

bool Shader::load() {
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(m_vertex_shader.c_str(), std::ios::in);
  if (VertexShaderStream.is_open()) {
    std::string Line = "";
    while (getline(VertexShaderStream, Line)) {
      VertexShaderCode += "\n" + Line;
    }
    VertexShaderStream.close();
  } else {
    std::cout<<"Impossible to open "<<m_vertex_shader<<". Are you in the right directory ? Don't forget to read the FAQ !"<<std::endl;
    getchar();
    return false;
  }
  
  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(m_fragment_shader.c_str(), std::ios::in);
  if (FragmentShaderStream.is_open()) {
    std::string Line = "";
    while (getline(FragmentShaderStream, Line)) {
      FragmentShaderCode += "\n" + Line;
    }
    FragmentShaderStream.close();
  } else {
    std::cout<<"Impossible to open "<<m_fragment_shader<<". Are you in the right directory ? Don't forget to read the FAQ !"<<std::endl;
    getchar();
    return false;
  }
  
  GLint Result = GL_FALSE;
  int InfoLogLength;
  
  
  // Compile Vertex Shader
  std::cout<<"Compiling shader : " << m_vertex_shader << std::endl;
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);
  
  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }
    
  // Compile Fragment Shader
  std::cout<<"Compiling shader : "<< m_fragment_shader << std::endl;
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  printf("Linking program\n");
  m_id = glCreateProgram();
  glAttachShader(m_id, VertexShaderID);
  glAttachShader(m_id, FragmentShaderID);
  glLinkProgram(m_id);

  // Check the program
  glGetProgramiv(m_id, GL_LINK_STATUS, &Result);
  glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(m_id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }
  
  glDetachShader(m_id, VertexShaderID);
  glDetachShader(m_id, FragmentShaderID);
    
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);
  return true;
}

Shader::Shader() {}

Shader::Shader(std::string vertex_file_path, std::string fragment_file_path) :
  m_vertex_shader(vertex_file_path), m_fragment_shader(fragment_file_path) {
  load();
}


Shader::~Shader() {
  disable();
}

void Shader::enable() {
  glUseProgram(m_id);
}

void Shader::disable() {
  glUseProgram(0);
}

GLuint Shader::getProgramID() const {
  return m_id;
}

#endif
