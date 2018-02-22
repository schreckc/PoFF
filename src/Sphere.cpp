
#include "Sphere.hpp"

#include <iostream>
#include <cassert>

#include "Scene.hpp"

#ifndef NO_GRAPHICS_ 

 GLfloat* Sphere::vertices = new GLfloat[Sphere::size_array];
 GLfloat* Sphere::normals = new GLfloat[Sphere::size_array];

#endif

Sphere::Sphere(float s, int shader): Object(shader) {
    m_size = s;

   #ifndef NO_GRAPHICS_
    m_model_view = scale(m_model_view, glm::vec3(s, s, s));
    m_texture = NULL;
    #endif
    setColor(1, 1, 1);
  }

 Sphere::~Sphere() {
  }

void Sphere::setColor(float r, float g, float b) {
   cr = r;
   cg = g;
   cb = b;
}
  
  void Sphere::animate() {
  }

#ifndef NO_GRAPHICS_ 

void Sphere::draw(glm::mat4 m, int s) {
  GLfloat colors[size_array];
  for (uint i = 0; i < size_array; i+= 3) {
    colors[i] = cr;
    colors[i+1] = cg;
    colors[i+2] = cb;
  }
 
  enableShader();
  setMVP(m, s);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLES, 0, size_array/3);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    disableShader();
  }




void Sphere::create_array() {
    GLfloat phi_prec = 0;
    GLfloat phi_step = M_PI / (parallels_count + 1);
    GLfloat theta_prec = 0;
    GLfloat theta_step = 2*M_PI / (meridians_count);

    GLfloat phi = phi_step;
    GLfloat theta = 0;
    uint index = 0;

    for (uint i = 0; i < meridians_count; ++i) {
      theta_prec = theta;
      theta += theta_step;
      create_vertex(theta_prec, phi, index);
      create_vertex(theta, phi, index);
      create_vertex(theta, phi_prec, index);
     }

    for (uint j = 0; j < parallels_count-1; ++j) {
      phi_prec = phi;
      phi += phi_step;
      theta = 0;
      for (uint i = 0; i < meridians_count; ++i) {
	theta_prec = theta;
	theta += theta_step;
	create_vertex(theta, phi_prec, index);
	create_vertex(theta_prec, phi_prec, index);
	create_vertex(theta, phi, index);
	create_vertex(theta_prec, phi, index);
	create_vertex(theta, phi, index);
	create_vertex(theta_prec, phi_prec, index);
      }
    }
    phi_prec = phi;
    phi = M_PI;
    theta = 0;
    for (uint i = 0; i < meridians_count; ++i) {
      theta_prec = theta;
      theta += theta_step;
      create_vertex(theta, phi_prec, index);
      create_vertex(theta_prec, phi_prec, index);
      create_vertex(theta, phi, index);
    }
  }

void Sphere::create_vertex(float theta, float phi, uint &index) {
  vertices[index] = cos(theta)*sin(phi);
  vertices[index+1] = sin(theta)*sin(phi);
  vertices[index+2] = cos(phi);
  normals[index] = cos(theta)*sin(phi);
  normals[index+1] = sin(theta)*sin(phi);
  normals[index+2] = cos(phi);
  index += 3;
}

#endif

// void Sphere::addToMesh(MAT3 S, std::list<VEC3> & points, std::list<VEC3> & normals,
// 			   std::list<VEC2> & tex_points, std::list<unsigned int> & connectivity) {

//   FLOAT phi_step = M_PI / (parallels_count + 1);
//   FLOAT theta_step = 2*M_PI / (meridians_count);

//     FLOAT phi = phi_step;
//     FLOAT theta = 0;
//     uint index = points.size();

//     addPoint(0, 0, points, normals, tex_points);
//       for (uint j = 1; j < parallels_count-1; ++j) {
//       phi += phi_step;
//       theta = 0;
//       for (uint i = 0; i < meridians_count; ++i) {
// 	theta += theta_step;
// 	addPoint(theta, phi, points, normals, tex_points);
//       }
//     }
//       addPoint(0, M_PI, points, normals, tex_points);
    

    
//     for (uint i = 0; i < meridians_count; ++i) {
//       connectivity.push_back(i+index+1);
//       connectivity.push_back((i+1)%meridians_count + index +1);
//       connectivity.push_back(index);
//     }

//     for (uint j = 0; j < parallels_count-1; ++j) {
//       for (uint i = 0; i < meridians_count; ++i) {
// 	connectivity.push_back(1+index+j*meridians_count+(i+1)%meridians_count);
// 	connectivity.push_back(1+index+j*meridians_count+i);
// 	connectivity.push_back(1+index+((j+1)%parallels_count)*meridians_count+(i+1)%meridians_count);
// 	connectivity.push_back(1+index+((j+1)%parallels_count)*meridians_count+i);
// 	connectivity.push_back(1+index+((j+1)%parallels_count)*meridians_count+(i+1)%meridians_count);
// 	connectivity.push_back(1+index+j*meridians_count+i);
//       }
//     }

//     for (uint i = 0; i < meridians_count; ++i) {
//       connectivity.push_back(1+index+(i+1)%meridians_count+(parallels_count-1)*meridians_count);
//       connectivity.push_back(1+index+i+(parallels_count-1)*meridians_count);
//       connectivity.push_back(index+ parallels_count*meridians_count);
//     }
// }


//  void Sphere::addPoint(float theta, float phi, std::list<VEC3> & points, std::list<VEC3> & normals,
// 			   std::list<VEC2> & tex_points) {
//    VEC3 v, n;
//    VEC2 tex;
//   v[0] = cos(theta)*sin(phi);
//   v[1] = sin(theta)*sin(phi);
//   v[2] = cos(phi);
//   // float n =  cos(theta)*sin(phi)*cos(theta)*sin(phi) + sin(theta)*sin(phi)*sin(theta)*sin(phi) + cos(phi)*cos(phi);
//   // n = sqrt(n);
//   //  std::cout<<"n  "<<n<<std::endl;
//   n[0] = cos(theta)*sin(phi);
//   n[1] = sin(theta)*sin(phi);
//   n[2] = cos(phi);

//   points.push_back(v);
//   normals.push_back(v);

//   // TODO compute texture coord

//   // TODO case of ellipsoid
// }
