#include "OpenBoxObstacle.hpp"
#include "error.hpp"
#include "PlaneObstacle.hpp"

// const float OpenBoxObstacle::vertices[90] = {-1, -1, -1,   1, -1, -1,   1, 1, -1,     // Face 1
// 				   -1, -1, -1,   -1, 1, -1,   1, 1, -1,     // Face 1

// 				   1, -1, 1,   1, -1, -1,   1, 1, -1,       // Face 2
// 				   1, -1, 1,   1, 1, 1,   1, 1, -1,         // Face 2

// 				   -1, -1, 1,   1, -1, 1,   1, -1, -1,      // Face 3
// 				   -1, -1, 1,   -1, -1, -1,   1, -1, -1,    // Face 3

// 				   // -1, -1, 1,   1, -1, 1,   1, 1, 1,        // Face 4
// 				   // -1, -1, 1,   -1, 1, 1,   1, 1, 1,        // Face 4

// 				   -1, -1, -1,   -1, -1, 1,   -1, 1, 1,     // Face 5
// 				   -1, -1, -1,   -1, 1, -1,   -1, 1, 1,     // Face 5

// 				   -1, 1, 1,   1, 1, 1,   1, 1, -1,         // Face 6
// 				   -1, 1, 1,   -1, 1, -1,   1, 1, -1};

// const float OpenBoxObstacle::normals[90] =  {0.0, 0.0, -1.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 1
// 				   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 1

// 				   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 2
// 				   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 2

// 				   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,           // Face 3
// 				   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,   0.0, -1.0, 0.0,           // Face 3

// 				   // 0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 4
// 				   // 0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 4

// 				   -1.0, 0.0, 0.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 5
// 				   -1.0, 0.0, 0.0,   0.0, 0.0, -1.0,   0.0, 0.0, -1.0,           // Face 5

// 				   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 6
// 				   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0};

// const  float OpenBoxObstacle::coordTexture[60] =  {0, 0,   1, 0,   1, 1,
// 					0, 0,   0, 1,   1, 1,

// 					0, 0,   1, 0,   1, 1,
// 					0, 0,   0, 1,   1, 1,

// 					0, 0,   1, 0,   1, 1,
// 					0, 0,   0, 1,   1, 1,

// 					// 0, 0,   1, 0,   1, 1,
// 					// 0, 0,   0, 1,   1, 1,

// 					0, 0,   1, 0,   1, 1,
// 					0, 0,   0, 1,   1, 1,

// 					0, 0,   1, 0,   1, 1,
// 					0, 0,   0, 1,   1, 1};


OpenBoxObstacle::OpenBoxObstacle(VEC3 min_p, VEC3 max_p, int shader) : Obstacle(shader), min_pos(min_p), max_pos(max_p) {
  v = std::vector<VEC3>(3);
  v[0] = VEC3(1, 0, 0);
  v[1] = VEC3(0, 1, 0);
  v[2] = VEC3(0, 0, 1);

 for (uint i = 0; i < 3; ++i) {
   size(i) = max_pos(i) - min_pos(i);
   // v[i] = VEC3(0, 0, 0);
   // v[i](i) = 1;
 }

 VEC3 half = 0.5*(max_pos - min_pos);
 middle = min_pos + half;
}

OpenBoxObstacle::~OpenBoxObstacle() {}

//void OpenBoxObstacle::animate() {}
void OpenBoxObstacle::apply(Motion m) {
   if (m.rotation_center_def) {
    VEC3 diff = middle - m.center;
    diff = m.rotation*diff;
    middle = m.center + diff;
      } else {
    middle += m.translation;
    size *= m.scale;
   }
  for (uint i = 0; i < 3 ; ++i) {
    v[i] = m.rotation*v[i];
    v[i].normalize();
  }
}
  
#ifndef NO_GRAPHICS_ 
void OpenBoxObstacle::draw(glm::mat4 m, int s) {
    for (uint i = 0; i < 3 ; ++i) {
      VEC3 f1 = middle + 0.5*size(i)*v[i];
      VEC3 f2 = middle - 0.5*size(i)*v[i];

      PlaneObstacle po2(f2, -v[i], size((i+1)%3), size((i+2)%3), v[(i+1)%3], v[(i+2)%3], m_shader);
      po2.draw(m, s);

      if (i != 2) {
	PlaneObstacle po1(f1, v[i], size((i+1)%3), size((i+2)%3), v[(i+1)%3], v[(i+2)%3], m_shader);
	po1.draw(m, s);
      }
      
    }
    
   //  float couleurs[90] = {1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 1
  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 1

  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 2
  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 2

  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 3
  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 3

  // 		// 1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 4
  // 		// 1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 4

  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 5
  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 5

  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0,           // Face 6
  // 		1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, 1.0};

  //  float OpenBoxObstacle::vertices[90] = {-1, -1, -1,   1, -1, -1,   1, 1, -1,     // Face 1
  // 				   -1, -1, -1,   -1, 1, -1,   1, 1, -1,     // Face 1

  // 				   1, -1, 1,   1, -1, -1,   1, 1, -1,       // Face 2
  // 				   1, -1, 1,   1, 1, 1,   1, 1, -1,         // Face 2

  // 				   -1, -1, 1,   1, -1, 1,   1, -1, -1,      // Face 3
  // 				   -1, -1, 1,   -1, -1, -1,   1, -1, -1,    // Face 3

  // 				   // -1, -1, 1,   1, -1, 1,   1, 1, 1,        // Face 4
  // 				   // -1, -1, 1,   -1, 1, 1,   1, 1, 1,        // Face 4

  // 				   -1, -1, -1,   -1, -1, 1,   -1, 1, 1,     // Face 5
  // 				   -1, -1, -1,   -1, 1, -1,   -1, 1, 1,     // Face 5

  // 				   -1, 1, 1,   1, 1, 1,   1, 1, -1,         // Face 6
  // 				   -1, 1, 1,   -1, 1, -1,   1, 1, -1};

    
  //    m_shader = 1;
  //  m_texture = 3;
  //   enableShader();
  //   setMVP(m, s);
  //   // glm::mat4 vp;
  //   // Scene::SCENE->getProjView(vp);
  //   // glm::mat4 mvp = vp * m_model_view;
  //   //   glUniformMatrix4fv(glGetUniformLocation(m_shader->getProgramID(), "mvp"), 1, GL_FALSE, value_ptr(mvp));
      
  //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  //   glEnableVertexAttribArray(0);

  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, couleurs);
  // glEnableVertexAttribArray(1);

  // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, normals);
  // glEnableVertexAttribArray(2);

  // glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, coordTexture);
  // glEnableVertexAttribArray(3);

  // glDrawArrays(GL_TRIANGLES, 0, 30);

  // glDisableVertexAttribArray(3);
  // glDisableVertexAttribArray(2);
  // glDisableVertexAttribArray(1);
  // glDisableVertexAttribArray(0);

  // disableShader();
 
}
#endif

VEC3 OpenBoxObstacle::getNormal(VEC3 p) const {
  VEC3 normal;
 VEC3 dir = p - middle;
  VEC3 proj = p;
  FLOAT d_min = 10;
  FLOAT i_min = 0;
  FLOAT out = 1;
  if (v[2].dot(dir) <= 0.5*size(2)) {
  for (uint i = 0; i < 3; ++i) {
    FLOAT d =  v[i].dot(dir);
     if (d > 0.5*size(i)) {
       proj += -(d -  0.5*size(i))*v[i];
       out = -1;
     } else if (d < -0.5*size(i)) {
       proj += (-d -  0.5*size(i))*v[i];
       out = -1;
       // INFO(3, "d "<<d <<" "<<i);

     }
     d =  v[i].dot(proj - middle);
    if ( d < 0) {
      d = -0.5*size(i) - d;
    } else {
      d = 0.5*size(i) - d;
    }
    if ( fabs(d) < fabs(d_min)) {
      d_min = d;
      i_min = i;
    }
  }
  // INFO(3, "p "<<p(0)<<" "<<p(1)<<" "<<p(2));
  // INFO(3, "proj "<< proj(0)<<" "<<proj(1)<<" "<<proj(2));
  
  proj += d_min*v[i_min];

  // INFO(3, "d_min "<<d_min<<" "<<i_min);
  // INFO(3, "proj "<< proj(0)<<" "<<proj(1)<<" "<<proj(2));
  //  INFO(3, "p "<<p(0)<<" "<<p(1)<<" "<<p(2));
  
     normal = out*(p - proj);
  // if (normal.norm() == 0) {
  //   normal = v[i_min];
  // } else {
    normal.normalize();
    // }
  } else {
    normal = v[2];
  }
    return normal;
}


FLOAT OpenBoxObstacle::distance(VEC3 p) const {
  FLOAT dist;
 VEC3 dir = p - middle;
  VEC3 proj = p;
  FLOAT d_min = 10;
  FLOAT i_min = 0;
  FLOAT out = 1;
  if (v[2].dot(dir) <= 0.5*size(2)) {
  for (uint i = 0; i < 3; ++i) {
    FLOAT d =  v[i].dot(dir);
     if (d > 0.5*size(i)) {
       proj += -(d -  0.5*size(i))*v[i];
       out = -1;
     } else if (d < -0.5*size(i)) {
       proj += (-d -  0.5*size(i))*v[i];
       out = -1;
       // INFO(3, "d "<<d <<" "<<i);

     }
     d =  v[i].dot(proj - middle);
    if ( d < 0) {
      d = -0.5*size(i) - d;
    } else {
      d = 0.5*size(i) - d;
    }
    if ( fabs(d) < fabs(d_min)) {
      d_min = d;
      i_min = i;
    }
  }
  // INFO(3, "p "<<p(0)<<" "<<p(1)<<" "<<p(2));
  // INFO(3, "proj "<< proj(0)<<" "<<proj(1)<<" "<<proj(2));
  
  proj += d_min*v[i_min];

  // INFO(3, "d_min "<<d_min<<" "<<i_min);
  // INFO(3, "proj "<< proj(0)<<" "<<proj(1)<<" "<<proj(2));
  //  INFO(3, "p "<<p(0)<<" "<<p(1)<<" "<<p(2));
  
  dist = out*(p - proj).norm();
  }
  return dist;
}

void OpenBoxObstacle::getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const {
  

  
  // VEC3 dist_to_min, dist_to_max;
  // FLOAT min_d = p(0) - min_pos(0);
  // int i_min = 0;
  // int dir = 1;
  // for (uint i = 0; i < 3; ++i) {
  //   dist_to_min(i) = p(i) - min_pos(i);
  //   dist_to_max(i) = max_pos(i) - p(i);
  //   if (fabs(dist_to_min(i)) < fabs(min_d)) {
  //     min_d = dist_to_min(i);
  //     i_min = i;
  //     dir = 1;
  //   }
  //   if (fabs(dist_to_max(i)) < fabs(min_d)) {
  //     min_d = dist_to_max(i);
  //     i_min = i;
  //     dir = -1;
  //   }
  // }
  // normal(i_min) = (FLOAT)dir*1.0;
  // normal((i_min+1)%3) = 0.0;
  // normal((i_min+2)%3) = 0.0;
  // dist = min_d;

  VEC3 dir = p - middle;
  VEC3 proj = p;
  FLOAT d_min = 100;
  FLOAT i_min = 0;
  FLOAT out = 1;
  if (v[2].dot(dir) < 0.5*size(2)) {
    for (uint i = 0; i < 3; ++i) {
      FLOAT d =  v[i].dot(dir);
      if (d > 0.5*size(i)) {
	proj += -(d -  0.5*size(i))*v[i];
	out = -1;
      } else if (d < -0.5*size(i)) {
	proj += (-d -  0.5*size(i))*v[i];
	out = -1;
	// INFO(3, "d "<<d <<" "<<i);

      }
      d =  v[i].dot(proj - middle);
      if ( d < 0) {
	d = -0.5*size(i) - d;
      } else {
	d = 0.5*size(i) - d;
      }
      if ( fabs(d) < fabs(d_min)) {
	d_min = d;
	i_min = i;
      }
    }
    // INFO(3, "p "<<p(0)<<" "<<p(1)<<" "<<p(2));
    // INFO(3, "proj "<< proj(0)<<" "<<proj(1)<<" "<<proj(2));
  
    proj += d_min*v[i_min];

    // INFO(3, "d_min "<<d_min<<" "<<i_min);
    // INFO(3, "proj "<< proj(0)<<" "<<proj(1)<<" "<<proj(2));
    //  INFO(3, "p "<<p(0)<<" "<<p(1)<<" "<<p(2));
  
    dist = out*(p - proj).norm();
    //  INFO(3, "dist "<<dist);
     
    normal = out*(p - proj);
    // if (normal.norm() == 0) {
    //   normal = v[i_min];
    // } else {
    normal.normalize();
    // }
  } else {
    dist = 10;
    normal = VEC3(0, 0, 1);//v[2];
  }
}
