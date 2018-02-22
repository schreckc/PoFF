#include "OpenBoxObstacle.hpp"
#include "error.hpp"
#include "PlaneObstacle.hpp"


OpenBoxObstacle::OpenBoxObstacle(VEC3 min_p, VEC3 max_p, int shader) : Obstacle(shader), min_pos(min_p), max_pos(max_p) {
  v = std::vector<VEC3>(3);
  v[0] = VEC3(1, 0, 0);
  v[1] = VEC3(0, 1, 0);
  v[2] = VEC3(0, 0, 1);

 for (uint i = 0; i < 3; ++i) {
   size(i) = max_pos(i) - min_pos(i);
 }

 VEC3 half = 0.5*(max_pos - min_pos);
 middle = min_pos + half;
}

OpenBoxObstacle::~OpenBoxObstacle() {}

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

      if (i != 2 && i != 1) {
	PlaneObstacle po1(f1, v[i], size((i+1)%3), size((i+2)%3), v[(i+1)%3], v[(i+2)%3], m_shader);
	po1.draw(m, s);
      }
      
    }
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
  
  proj += d_min*v[i_min];
  normal = out*(p - proj);
  normal.normalize();
  
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
  proj += d_min*v[i_min];
  dist = out*(p - proj).norm();
  }
  return dist;
}

void OpenBoxObstacle::getCollisionValues(VEC3 p, FLOAT & dist, VEC3 &normal) const {

  VEC3 dir = p - middle;
  VEC3 proj = p;
  FLOAT d_min = 100;
  FLOAT i_min = 0;
  FLOAT out = 1;
  if (v[2].dot(dir) < 0.5*size(2) && v[1].dot(dir) < 0.5*size(1)) {
    for (uint i = 0; i < 3; ++i) {
      FLOAT d =  v[i].dot(dir);
      if (d > 0.5*size(i)) {
	proj += -(d -  0.5*size(i))*v[i];
	out = -1;
      } else if (d < -0.5*size(i)) {
	proj += (-d -  0.5*size(i))*v[i];
	out = -1;
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
  
    proj += d_min*v[i_min];
    dist = out*(p - proj).norm();
     
    normal = out*(p - proj);
    normal.normalize();

  } else {
    dist = 10;
    normal = VEC3(0, 0, 1);
  }
}

void OpenBoxObstacle::exportMitsuba(std::ofstream & file) const {
     for (uint i = 0; i < 3 ; ++i) {
      VEC3 f1 = middle + 0.51*size(i)*v[i];
      VEC3 f2 = middle - 0.51*size(i)*v[i];

      PlaneObstacle po2(f2, -v[i], 1.02*size((i+1)%3), 1.02*size((i+2)%3), v[(i+1)%3], v[(i+2)%3], m_shader);
      po2.exportMitsuba(file);

       if (i != 2 && i != 1) {
       	PlaneObstacle po1(f1, v[i], 1.02*size((i+2)%3), 1.02*size((i+1)%3), v[(i+2)%3], v[(i+1)%3], m_shader);
       	po1.exportMitsuba(file);
       }
      
    }
}
