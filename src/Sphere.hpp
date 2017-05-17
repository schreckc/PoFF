#ifndef NO_GRAPHICS_

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Object.hpp"
#include <glm/gtx/transform.hpp>
    
class Sphere : public Object {
private :
  float m_size;
  float cr, cg, cb;
  //  GLfloat* colors;
  const static uint parallels_count = 4;
  const static uint meridians_count = 5;
  const static uint size_array = 18*meridians_count + 18*meridians_count* (parallels_count-1);
 
  static GLfloat* vertices; //= new GLfloat[Sphere::size_array];
  static GLfloat* normals;// = new GLfloat[Sphere::size_array];
  //  static GLfloat* colors_sphere;// = new GLfloat[Sphere::size_array];
  
public :
  
  Sphere(float s = 1, int shader = -1);
  ~Sphere();

  void setColor(float r, float g, float b);
  
  void animate();
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);

  static void create_array();
  static void create_vertex(float theta, float phi, uint &index);
  //void delete_array();

};
  
#endif
#endif
