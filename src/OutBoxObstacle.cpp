#include "OutBoxObstacle.hpp"
#include "error.hpp"

OutBoxObstacle::OutBoxObstacle(VEC3 min_p, VEC3 max_p, FLOAT thick, int shader) : Obstacle(shader), min_pos(min_p), max_pos(max_p), thickness(thick) {

}

OutBoxObstacle::~OutBoxObstacle() {}

//void BoxObstacle::animate() {}
void OutBoxObstacle::apply(Motion m) {
  min_pos += m.translation;
  max_pos += m.translation;

  VEC3 half = 0.5*(max_pos - min_pos);
  VEC3 middle = min_pos + 0.5*half;
  half *= m.scale;
  thickness *= m.scale;
  min_pos = middle - half;
  max_pos = middle + half;
}
  
#ifndef NO_GRAPHICS_ 
void OutBoxObstacle::draw(glm::mat4 m, int s) {
  
   GLfloat ext_vert[72];
   //for (uint l = 0; l < 3; ++l) {
    ext_vert[0] = min_pos(0);
    ext_vert[1] = min_pos(1);
    ext_vert[2] = min_pos(2);
    ext_vert[3] = max_pos(0);
    ext_vert[4] = min_pos(1);
    ext_vert[5] = min_pos(2);

    ext_vert[6] = min_pos(0);
    ext_vert[7] = min_pos(1);
    ext_vert[8] = min_pos(2);
    ext_vert[9] = min_pos(0);
    ext_vert[10] = max_pos(1);
    ext_vert[11] = min_pos(2);

    ext_vert[12] = min_pos(0);
    ext_vert[13] = min_pos(1);
    ext_vert[14] = min_pos(2);
    ext_vert[15] = min_pos(0);
    ext_vert[16] = min_pos(1);
    ext_vert[17] = max_pos(2);

    ext_vert[18] = max_pos(0);
    ext_vert[19] = min_pos(1);
    ext_vert[20] = min_pos(2);
    ext_vert[21] = max_pos(0);
    ext_vert[22] = min_pos(1);
    ext_vert[23] = max_pos(2);

    ext_vert[24] = max_pos(0);
    ext_vert[25] = min_pos(1);
    ext_vert[26] = min_pos(2);
    ext_vert[27] = max_pos(0);
    ext_vert[28] = max_pos(1);
    ext_vert[29] = min_pos(2);

    ext_vert[30] = min_pos(0);
    ext_vert[31] = max_pos(1);
    ext_vert[32] = min_pos(2);
    ext_vert[33] = max_pos(0);
    ext_vert[34] = max_pos(1);
    ext_vert[35] = min_pos(2);

    ext_vert[36] = min_pos(0);
    ext_vert[37] = max_pos(1);
    ext_vert[38] = min_pos(2);
    ext_vert[39] = min_pos(0);
    ext_vert[40] = max_pos(1);
    ext_vert[41] = max_pos(2);

    
    ext_vert[42] = min_pos(0);
    ext_vert[43] = min_pos(1);
    ext_vert[44] = max_pos(2);
    ext_vert[45] = min_pos(0);
    ext_vert[46] = max_pos(1);
    ext_vert[47] = max_pos(2);

    ext_vert[48] = min_pos(0);
    ext_vert[49] = min_pos(1);
    ext_vert[50] = max_pos(2);
    ext_vert[51] = max_pos(0);
    ext_vert[52] = min_pos(1);
    ext_vert[53] = max_pos(2);

    ext_vert[54] = max_pos(0);
    ext_vert[55] = max_pos(1);
    ext_vert[56] = max_pos(2);
    ext_vert[57] = max_pos(0);
    ext_vert[58] = min_pos(1);
    ext_vert[59] = max_pos(2);

    ext_vert[60] = max_pos(0);
    ext_vert[61] = max_pos(1);
    ext_vert[62] = max_pos(2);
    ext_vert[63] = min_pos(0);
    ext_vert[64] = max_pos(1);
    ext_vert[65] = max_pos(2);

    ext_vert[66] = max_pos(0);
    ext_vert[67] = max_pos(1);
    ext_vert[68] = max_pos(2);
    ext_vert[69] = max_pos(0);
    ext_vert[70] = max_pos(1);
    ext_vert[71] = min_pos(2);
    //  }
  GLfloat ext_col[72];
  for (uint l = 0; l < 72; l+=3) {
    ext_col[l] = 0;
    ext_col[l+1] = 0;
    ext_col[l+2] = 1.0;
  }

  glLineWidth(3.0f);

  enableShader();
  setMVP(m, s);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ext_vert);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, ext_col);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_LINES, 0, 24);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  disableShader();

   //for (uint l = 0; l < 3; ++l) {
    ext_vert[0] = - thickness + min_pos(0);
    ext_vert[1] = - thickness + min_pos(1);
    ext_vert[2] = - thickness + min_pos(2);
    ext_vert[3] = thickness + max_pos(0);
    ext_vert[4] = - thickness + min_pos(1);
    ext_vert[5] = - thickness + min_pos(2);

    ext_vert[6] = - thickness + min_pos(0);
    ext_vert[7] = - thickness + min_pos(1);
    ext_vert[8] = - thickness + min_pos(2);
    ext_vert[9] = - thickness + min_pos(0);
    ext_vert[10] = thickness + max_pos(1);
    ext_vert[11] = - thickness + min_pos(2);

    ext_vert[12] = - thickness + min_pos(0);
    ext_vert[13] = - thickness + min_pos(1);
    ext_vert[14] = - thickness + min_pos(2);
    ext_vert[15] = - thickness + min_pos(0);
    ext_vert[16] = - thickness + min_pos(1);
    ext_vert[17] = max_pos(2);

    ext_vert[18] = thickness + max_pos(0);
    ext_vert[19] = - thickness + min_pos(1);
    ext_vert[20] = - thickness + min_pos(2);
    ext_vert[21] = thickness + max_pos(0);
    ext_vert[22] = - thickness + min_pos(1);
    ext_vert[23] =  max_pos(2);

    ext_vert[24] = thickness + max_pos(0);
    ext_vert[25] = - thickness + min_pos(1);
    ext_vert[26] = - thickness + min_pos(2);
    ext_vert[27] = thickness + max_pos(0);
    ext_vert[28] = thickness + max_pos(1);
    ext_vert[29] = - thickness + min_pos(2);

    ext_vert[30] = - thickness + min_pos(0);
    ext_vert[31] = thickness + max_pos(1);
    ext_vert[32] = - thickness + min_pos(2);
    ext_vert[33] = thickness + max_pos(0);
    ext_vert[34] = thickness + max_pos(1);
    ext_vert[35] = - thickness + min_pos(2);

    ext_vert[36] = - thickness + min_pos(0);
    ext_vert[37] = thickness + max_pos(1);
    ext_vert[38] = - thickness + min_pos(2);
    ext_vert[39] = - thickness + min_pos(0);
    ext_vert[40] = thickness + max_pos(1);
    ext_vert[41] =  max_pos(2);

    
    ext_vert[42] = - thickness + min_pos(0);
    ext_vert[43] = - thickness + min_pos(1);
    ext_vert[44] = max_pos(2);
    ext_vert[45] = - thickness + min_pos(0);
    ext_vert[46] = thickness + max_pos(1);
    ext_vert[47] = max_pos(2);

    ext_vert[48] = - thickness + min_pos(0);
    ext_vert[49] = - thickness + min_pos(1);
    ext_vert[50] = max_pos(2);
    ext_vert[51] = thickness + max_pos(0);
    ext_vert[52] = - thickness + min_pos(1);
    ext_vert[53] = max_pos(2);

    ext_vert[54] = thickness + max_pos(0);
    ext_vert[55] = thickness + max_pos(1);
    ext_vert[56] = max_pos(2);
    ext_vert[57] = thickness + max_pos(0);
    ext_vert[58] = - thickness + min_pos(1);
    ext_vert[59] = max_pos(2);

    ext_vert[60] = thickness + max_pos(0);
    ext_vert[61] = thickness + max_pos(1);
    ext_vert[62] = max_pos(2);
    ext_vert[63] = - thickness + min_pos(0);
    ext_vert[64] = thickness + max_pos(1);
    ext_vert[65] = max_pos(2);

    ext_vert[66] = thickness + max_pos(0);
    ext_vert[67] = thickness + max_pos(1);
    ext_vert[68] = max_pos(2);
    ext_vert[69] = thickness + max_pos(0);
    ext_vert[70] = thickness + max_pos(1);
    ext_vert[71] = - thickness + min_pos(2);
    //  }
  for (uint l = 0; l < 72; l+=3) {
    ext_col[l] = 0;
    ext_col[l+1] = 0;
    ext_col[l+2] = 1.0;
  }

  glLineWidth(3.0f);

  enableShader();
  setMVP(m, s);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ext_vert);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, ext_col);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_LINES, 0, 24);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  disableShader();
  //  INFO(3, "BOX\n"<<min_pos<<"\n\n"<<max_pos);
}
#endif

VEC3 OutBoxObstacle::getNormal(VEC3 p) const {
  VEC3 normal;
  VEC3 dist_to_min, dist_to_max;
  FLOAT min_d = p(0) - min_pos(0);
  int i_min = 0;
  int dir = 1;
  for (uint i = 0; i < 3; ++i) {
    dist_to_min(i) = p(i) - min_pos(i);
    dist_to_max(i) = p(i) - max_pos(i);
    if (fabs(dist_to_min(i)) < fabs(min_d)) {
      min_d = dist_to_min(i);
      i_min = i;
      dir = 1;
    }
    if (fabs(dist_to_max(i)) < fabs(min_d)) {
      min_d = dist_to_max(i);
      i_min = i;
      dir = -1;
    }
  }
  normal(i_min) = (FLOAT)dir*1.0;
  normal((i_min+1)%3) = 0.0;
  normal((i_min+2)%3) = 0.0;
    TEST(false); //TODO
  return normal;
}


FLOAT OutBoxObstacle::distance(VEC3 v) const {
 VEC3 dist_to_min, dist_to_max;
  FLOAT min_d = v(0) - min_pos(0);
   int dir = 1;
  for (uint i = 0; i < 3; ++i) {
    dist_to_min(i) = v(i) - min_pos(i);
    dist_to_max(i) = max_pos(i) - v(i);
    if (fabs(dist_to_min(i)) < fabs(min_d)) {
      min_d = dist_to_min(i);
      dir = 1;
    }
    if (fabs(dist_to_max(i)) < fabs(min_d)) {
      min_d = dist_to_max(i);
      dir = -1;
    }
  }
  TEST(false); //TODO
  return min_d;
}

void OutBoxObstacle::getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const {
  VEC3 dist_to_min, dist_to_max;
  FLOAT min_d = p(0) - min_pos(0);
  int i_min = 0;
  int dir = 1;
  for (uint i = 0; i < 3; ++i) {
    dist_to_min(i) = p(i) - min_pos(i);
    dist_to_max(i) = max_pos(i) - p(i);
    if (fabs(dist_to_min(i)) < fabs(min_d)) {
      min_d = dist_to_min(i);
      i_min = i;
      dir = 1;
    }
    if (fabs(dist_to_max(i)) < fabs(min_d)) {
      min_d = dist_to_max(i);
      i_min = i;
      dir = -1;
    }
  }
  normal(i_min) = (FLOAT)dir*1.0;
  normal((i_min+1)%3) = 0.0;
  normal((i_min+2)%3) = 0.0;
  dist = min_d;
  TEST(false); //TODO
}

// void OutObstacle::clostestPoint(VEC3 p, VEC3 & closest) const {


// }
