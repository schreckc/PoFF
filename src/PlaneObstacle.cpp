#include "PlaneObstacle.hpp"
#include "error.hpp"
#include "utils.hpp"

PlaneObstacle::PlaneObstacle(VEC3 p, VEC3 n, FLOAT l, FLOAT w, int shader) : Obstacle(shader) {
  pos = p;
  normal = n;
  normal.normalize();
  length = l;
  width = w;
}

// void PlaneObstacle::animate() {
//   rotate(-0.05*mpm_conf::dt_, VEC3(0, 1, 0));
// }

void PlaneObstacle::apply(Motion m) {
  pos += m.translation;
  normal = m.rotation*normal;
  length *= m.scale;
   width *= m.scale;
   //   INFO(3, "scale "<<scale);
}
  
#ifndef NO_GRAPHICS_ 
void PlaneObstacle::draw(glm::mat4 m, int s) {
  FLOAT size = 1;
  VEC3 v1 = VEC3(1, 0, 0);
  VEC3 v2 = VEC3(0, 1, 0);
  if (normal(0) != 0 || normal(1) != 0) {
    v1 = normal.cross(VEC3(0, 0, 1));
    v2 = normal.cross(v1);
    v2.normalize();
    v1.normalize();
    // INFO(3, v1);
    // INFO(3, v2);
  }
  FLOAT l = 1, w = 1;
  if (length != 0) {
    l = length;
  }
  if (width != 0) {
    w = width;
  }
  VEC3 c0 = pos + 0.5*(l*v1 + w*v2);
  VEC3 c1 = pos + 0.5*(l*v1 - w*v2);
  VEC3 c2 = pos + 0.5*(-l*v1 - w*v2);
  VEC3 c3 = pos + 0.5*(-l*v1 + w*v2);
  // GLfloat vertices[18] = {-0.1, -0.1, pos,   -0.1, 1, pos,    1, 1, pos,
  // 			  1, 1, pos,  -0.1, -0.1, pos,   1, -0.1, pos};
  // GLfloat normals[18] = {0, 0, 1,  0, 0, 1,  0, 0, 1,
  // 			 0, 0, 1,  0, 0, 1,  0, 0, 1};
  GLfloat vertices[18] = {c0(0), c0(1), c0(2), c1(0), c1(1), c1(2), c2(0), c2(1), c2(2),
			  c2(0), c2(1), c2(2), c3(0), c3(1), c3(2), c0(0), c0(1), c0(2)};
   GLfloat normals[18] = {normal(0), normal(1), normal(2), normal(0), normal(1), normal(2), normal(0), normal(1), normal(2),
			  normal(0), normal(1), normal(2), normal(0), normal(1), normal(2), normal(0), normal(1), normal(2)};
  
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

VEC3 PlaneObstacle::getPosition() const {
  return pos;
}

VEC3 PlaneObstacle::getNormal(VEC3 v) const {
  return normal;
}

VEC3 PlaneObstacle::getNormal() const {
  return normal;
}

FLOAT PlaneObstacle::distance(VEC3 v) const {
  VEC3 v1 = VEC3(1, 0, 0);
  VEC3 v2 = VEC3(0, 1, 0);
  if (normal(0) != 0 || normal(1) != 0) {
    v1 = normal.cross(VEC3(0, 0, 1));
    v2 = normal.cross(v1);
    v2.normalize();
    v1.normalize();
  }
  VEC3 proj = utils::projectionOrtho(v, pos, v1, v2);
  FLOAT dist = 10;
  if (length == 0 || (fabs(v1.dot(proj-pos)) < 0.5*length && fabs(v2.dot(proj-pos)) < 0.5*width)) {
    dist = (proj - v).norm();
    //    INFO(3, "dist "<<dist);
    if (normal.dot(v-pos) < 0) {
      dist = -dist;
    }
  }
  //  TEST(false)
   // FLOAT nv = v.norm();
   // VEC3 dir = v/nv;
   // FLOAT costheta = dir.dot(normal);
   // FLOAT dist= 0;
   // if (nv != 0 && costheta != 0) {
   //   dist = nv*costheta - pos;
   // }
  return dist;
}

void PlaneObstacle::getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &n) const {
  // //dist = p(2) - pos;
  //  FLOAT nv = p.norm();
  //  VEC3 dir = p/nv;
  //  FLOAT costheta = dir.dot(normal);
  //  dist = 0;
  //  if (nv != 0 && costheta != 0) {
  //    dist = nv*costheta - pos;
  //  }
   VEC3 v1 = VEC3(1, 0, 0);
  VEC3 v2 = VEC3(0, 1, 0);
  if (normal(0) != 0 || normal(1) != 0) {
    v1 = normal.cross(VEC3(0, 0, 1));
    v2 = normal.cross(v1);
    v2.normalize();
    v1.normalize();
  }
  VEC3 proj = utils::projectionOrtho(p, pos, v1, v2);
  dist = 10;
  if (length == 0 || (fabs(v1.dot(proj-pos)) < 0.5*length && fabs(v2.dot(proj-pos)) < 0.5*width)) {
    dist = (proj - p).norm();
    if (normal.dot(p-pos) < 0) {
      dist = -dist;
    }
   }

    //INFO(3, "dist "<<dist);
  n = normal;
}

void PlaneObstacle::rotate(FLOAT angle, VEC3 axe) {
  VEC3 n = utils::rotation(normal, angle, axe);
  normal = n;
  //  INFO(3, normal);
}
