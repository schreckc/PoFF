#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include "Object.hpp"
#include <glm/gtx/transform.hpp>
    
class Cylinder : public Object {
private :
  float ray;
  float length;
  float cr, cg, cb;
  //  GLfloat* colors;
  const static uint count = 10;
  const static uint size_array = 18*count;
 
  static GLfloat* vertices; //= new GLfloat[Sphere::size_array];
  static GLfloat* normals;// = new GLfloat[Sphere::size_array];
  //  static GLfloat* colors_sphere;// = new GLfloat[Sphere::size_array];
  
public :
  
  Cylinder(float r = 1, float l = 1, Shader* shader = NULL);
  ~Cylinder();

  void setColor(float r, float g, float b);
  
  void animate();
  void draw(glm::mat4 m = glm::mat4(1.0f), Shader *s = NULL);

  static void create_array();
  static void create_vertex(float theta, float l, uint &index);
  //void delete_array();

};
  
#endif
