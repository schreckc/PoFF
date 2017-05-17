#include "PlaneObstacle.hpp"

PlaneObstacle::PlaneObstacle(FLOAT p, VEC3 n, int shader) : Obstacle(shader) {
  pos = p;
  normal = n;
  normal.normalize();
}

void PlaneObstacle::animate() {

}

#ifndef NO_GRAPHICS_ 
void PlaneObstacle::draw(glm::mat4 m, int s) {
  GLfloat vertices[18] = {-0.1, -0.1, pos,   -0.1, 1, pos,    1, 1, pos,
			  1, 1, pos,  -0.1, -0.1, pos,   1, -0.1, pos};
  GLfloat normals[18] = {0, 0, 1,  0, 0, 1,  0, 0, 1,
			 0, 0, 1,  0, 0, 1,  0, 0, 1};
  GLfloat colors[18] = {0.9, 0.9, 1.0,   0.9, 0.9, 1.0,  0.9, 0.9, 1.0,
			0.9, 0.9, 1.0,   0.9, 0.9, 1.0,  0.9, 0.9, 1.0};
  enableShader();
   setMVP(m, s);
  
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    disableShader();
}
#endif

FLOAT PlaneObstacle::getPosition() const {
  return pos;
}

VEC3 PlaneObstacle::getNormal(VEC3 v) const {
  return normal;
}

VEC3 PlaneObstacle::getNormal() const {
  return normal;
}

FLOAT PlaneObstacle::distance(VEC3 v) const {
  // FLOAT nv = v.norm();
  // VEC3 dir = v/nv;
  // FLOAT costheta = dir.dot(normal);
  // FLOAT dist= 0;
  // if (nv != 0 && costheta != 0) {
  //   dist = nv - pos/costheta;
  // }
  return v(2) - pos;
}

void PlaneObstacle::getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &n) const {
  dist = p(2) - pos;
  n = normal;
}
