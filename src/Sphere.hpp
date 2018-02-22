#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Object.hpp"

#ifndef NO_GRAPHICS_
#include <glm/gtx/transform.hpp>
#endif

#include "mpm_conf.hpp"
#include <list>

class Sphere : public Object {
private :
  float m_size;
  float cr, cg, cb;

  const static uint parallels_count = 4;
  const static uint meridians_count = 5;
  const static uint size_array = 18*meridians_count + 18*meridians_count* (parallels_count-1);

#ifndef NO_GRAPHICS_
  static GLfloat* vertices;
  static GLfloat* normals;
#endif
public :
  
  Sphere(float s = 1, int shader = -1);
  ~Sphere();

  void setColor(float r, float g, float b);
  
  void animate();
#ifndef NO_GRAPHICS_
  void draw(glm::mat4 m = glm::mat4(1.0f), int s = -1);

  static void create_array();
  static void create_vertex(float theta, float phi, uint &index);

#endif
  
  // void addToMesh(MAT3 S, std::list<VEC3> & points, std::list<VEC3> & normals,
  // 		 std::list<VEC2> & tex_points, std::list<unsigned int> & connectivity);
  // void addPoint(float theta, float phi, std::list<VEC3> & points, std::list<VEC3> & normals,
  // 		  std::list<VEC2> & tex_points);
};
  

#endif
