#include "Cylinder.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

#include "Scene.hpp"

 GLfloat* Cylinder::vertices = new GLfloat[Cylinder::size_array];
 GLfloat* Cylinder::normals = new GLfloat[Cylinder::size_array];

Cylinder::Cylinder(float r, float l, Shader* shader): Object(shader) {
    ray = r;
    m_texture = NULL;
    m_model_view = scale(m_model_view, glm::vec3(ray, ray, l));
    setColor(1, 0, 0);
  }

 Cylinder::~Cylinder() {
   //  delete[] colors;
  }

void Cylinder::setColor(float r, float g, float b) {
   cr = r;
   cg = g;
   cb = b;
  // colors = new GLfloat[size_array];
  // for (uint i = 0; i < size_array; i+= 3) {
  //   colors[i] = cr;
  //   colors[i+1] = cg;
  //   colors[i+2] = cb;
  //   //   std::cout <<"("<<vertices_sphere[i]<<", "<<vertices_sphere[i+1]<<", "<<vertices_sphere[i+2]<<") "<< std::endl;
  // }
}
  
void Cylinder::animate() {
}

void Cylinder::draw(glm::mat4 m, Shader *s) {
  GLfloat colors[size_array];
  for (uint i = 0; i < size_array; i+= 3) {
    colors[i] = cr;
    colors[i+1] = cg;
    colors[i+2] = cb;
    //std::cout <<"("<<vertices[i]<<", "<<vertices[i+1]<<", "<<vertices[i+2]<<") "<< std::endl;
    // std::cout <<"n ("<<normals_sphere[i]<<", "<<normals_sphere[i+1]<<", "<<normals_sphere[i+2]<<") "<< std::endl;
  }
  //  std::cout<<"draw CYL"<<std::endl;
  enableShader();
  setMVP(m, s);
  
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, normals);
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLES, 0, size_array/3);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    disableShader();
  }

void Cylinder::create_array() {
  GLfloat theta_prec = 0;
  GLfloat theta_step = 2.0*M_PI / (FLOAT)count;

  GLfloat theta = 0;
  uint index = 0;

  for (uint i = 0; i < count; ++i) {
    theta_prec = theta;
    theta += theta_step;
    create_vertex(theta_prec, -1, index);
    create_vertex(theta, -1, index);
    create_vertex(theta_prec, 1, index);
    create_vertex(theta, 1, index);
    create_vertex(theta_prec, 1, index);
    create_vertex(theta, -1, index);
    //  std::cout<<"theta ;"<<theta<<std::endl;
  }
}

void Cylinder::create_vertex(float theta, float l, uint &index) {
  vertices[index] = cos(theta);
  vertices[index+1] = sin(theta);
  vertices[index+2] = l;

  normals[index] = cos(theta);
  normals[index+1] = sin(theta);
  normals[index+2] = 0;
  index += 3;
}
