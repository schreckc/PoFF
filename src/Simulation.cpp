#include "Simulation.hpp"
#include "Scene.hpp"
#include "error.hpp"
#include "PlaneObstacle.hpp"
#include "SphereObstacle.hpp"
#include "CylinderObstacle.hpp"
#include "BoxObstacle.hpp"
#include "OpenBoxObstacle.hpp"
#include "BallObstacle.hpp"
#include "Times.hpp"
#include "mpm_conf.hpp"
#include "utils.hpp"

#define POISSON_PROGRESS_INDICATOR 1
#include "PoissonGenerator.hpp"

Simulation::Simulation(int shader) : Object(shader) {
  import_ = false;
  export_ = false;
  load_conf_ = false;
  mitsuba = false;

  if (load_conf_) {
    mpm_conf::loadConf(conf_file);
  }
  scene_path = "first_scene.sc";
}

Simulation::~Simulation() {
  clear();
}

void Simulation::init() {
  nb_file_i = 0;
  nb_file_e = 0;
  nb_file_mitsuba = 0;

  if (load_conf_) {
    mpm_conf::loadConf(conf_file);
  }
  
  if (!import_ && !import_grid) {

    grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, 2);
    loadScene();

    grid.init(particules);
    grid.initCollision(obstacles);
    
    INFO(1, "Lame Parameters : lambda = "<<mpm_conf::lambda_<<"    mu = "<<mpm_conf::mu_<<"\n");
    INFO(1, "dt = "<<mpm_conf::dt_<<"\n");
    INFO(3, "obstacles = "<<obstacles.size()<<"\n");
  
  }
  TEST(!import_ || !import_grid) 
    if (import_) {
      loadScene();
      importSim();
    }
  if (import_grid) {  
    grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, 2);
    loadScene();
    grid.init(particules);
    importGrid();
  }
  if (export_) {
    exportSim();
  }
  if (export_grid) {
    exportGrid();
  }
  if (mitsuba) {
    exportMitsuba();
  }
  
  time = 0;
}

void Simulation::clearParticules() {
  for (auto& p : particules) {
    delete p;
  }
  for (auto& p : subparticules) {
    delete p;
  }
  particules.clear();
  subparticules.clear();
  
}
  
void Simulation::clear() {
  INFO(1, "Clear Simulation");
  clearParticules();
  for (auto& ob : obstacles) {
    delete ob;
  }
  obstacles.clear();
}

void Simulation::animate() {
  ++time;
  if (!import_ && !import_grid) {
    oneStep();
    for (auto &ob : obstacles) {
      ob->animate(time*mpm_conf::dt_);
    }
  } else {
    if (import_) {
      importSim();
    } else if (import_grid) {
      importGrid();
    }
#ifndef NO_GRAPHICS_
    for (uint s = 0; s < mpm_conf::replay_speed_; ++s) {
      for (auto &ob : obstacles) {
	ob->animate(Scene::SCENE->getTime());
      }
    } 
#endif
  }
  if (export_) {
    exportSim();
  }
  if (export_grid) {
    exportGrid();
  }
  if (mitsuba) {
    exportMitsuba();
  }
}

#ifndef NO_GRAPHICS_ 
void Simulation::draw(glm::mat4 m, int s) {  
  Times::TIMES->tick(Times::display_time_);
  uint cur_shader = m_shader;
  if (m_shader == -1) {
    cur_shader = s;
  }
  glm::mat4 cur_model = m * m_model_view;

  grid.draw(cur_model, cur_shader);

  enableShader();
  if (subparticules.empty()) {
    for (auto& p : particules) {
      p->draw(cur_model, cur_shader);
    }
  }
  for (auto& p : subparticules) {
    p->draw(cur_model, cur_shader);
  }
  for (auto& ob : obstacles) {
    ob->draw(cur_model, cur_shader);
  }
    
  disableShader();
  Times::TIMES->tock(Times::display_time_);
}
#endif

void Simulation::oneStep() {
  Times::TIMES->tick(Times::simu_time_);
  grid.nextStep();
  if (mpm_conf::implicit_) {
    grid.particulesToGridImplicite(particules);
  } else {
    grid.particulesToGrid(particules);
  }
  if (mpm_conf::smooth_vel_) {
    for (uint i = 0; i < 1; ++i) {
      grid.smoothVelocity();
    }
  }
   grid.collision(obstacles);
  grid.gridToSubparticules(subparticules);
  grid.gridToParticules(particules);

  /** if mixing is active, interpolate mixing rotation from particles to gride nodes then from grid nodes to subparticles **/
  //  grid.smoothRotation(particules, subparticules);
  
  grid.initCollision(obstacles);
  Times::TIMES->tock(Times::simu_time_);
}

void Simulation::importParticulesAll(std::ifstream & file) {
  TEST(false); //not implemented
  // particules.clear();
  // std::string line;
  // uint ir = 0, ia = 0, im = 0, is = 0, ife = 0, ic = 0;
  // while (getline(file, line)) {
  //    // if (i == 10) {
  //   //   i = 0;
  //     if (line.substr(0,2) == "v ") {
  // 	std::istringstream s(line.substr(2));
  // 	VEC3 v;
  // 	s >> v(0); 
  // 	s >> v(1); 
  // 	s >> v(2);
  // 	Particule *p = new Particule(1, 0.000005, v);
  // 	p->setColor(1, 0.5, 0.5);
  // 	particules.push_back(p);
  // 	// INFO(3, p->getPosition());
  //     } else if (line.substr(0,2) == "r ") {
  // 	std::istringstream s(line.substr(2));
  // 	//INFO(3, line);
  // 	MAT3 r;
  // 	for (uint i = 0; i < 9; ++i) {
  // 	  s >> r(i);
  // 	}
  // 	particules[ir]->setAnisotropyRotation(r);//setAnisotropyAxes(VEC3(r(0), r(3), r(6)), VEC3(r(1), r(4), r(7)), VEC3(r(2), r(5), r(8)));
  // 	++ir;
  // 	//INFO(3, r);
  //     } else if (line.substr(0,2) == "a ") {
  // 	std::istringstream s(line.substr(2));
  // 	//INFO(3, line);
  // 	FLOAT vx, vy, vz;
  // 	s >> vx >> vy >> vz;
  // 	particules[ia]->setAnisotropyValues(vx, vy, vz);
  // 	++ia;
  //     } else if (line.substr(0,2) == "m ") {
  // 	std::istringstream s(line.substr(2));
  // 	//INFO(3, line);
  // 	FLOAT m, v, d;
  // 	s >> m >> v >> d;
  // 	particules[im]->setMass(m);
  // 	particules[im]->setVolume(v);
  // 	particules[im]->setDensity(d);
  // 	++im;
  //     } else if (line.substr(0,2) == "s ") {
  // 	std::istringstream s(line.substr(2));
  // 	//INFO(3, line);
  // 	VEC3 v;
  // 	s >> v(0) >> v(1) >> v(2);
  // 	particules[is]->setVelocity(v);
  // 	++is;
  //     } else if (line.substr(0,2) == "f ") {
  // 	std::istringstream s(line.substr(2));
  // 	//INFO(3, line);
  // 	MAT3 Fp, Fe;
  // 	for (uint i = 0; i < 9; ++i) {
  // 	  s >> Fe(i);
  // 	}
  // 	for (uint i = 0; i < 9; ++i) {
  // 	  s >> Fp(i);
  // 	}
  // 	particules[ife]->setDeformationElastic(Fe);
  // 	particules[ife]->setDeformationPlastic(Fp);
  // 	++ife;
  //     } else if (line.substr(0,2) == "c ") {
  // 	std::istringstream s(line.substr(2));
  // 	//INFO(3, line);
  // 	FLOAT vp;
  // 	s >> vp;
  // 	particules[ic]->setVolumeCorrection(vp);
  // 	++ic;
  //     } else if (line[0] == '#') {
  // 	//INFO(3, "COMMENT "<<line);
  //     } else {
  // 	WARNING(false, "Imported file possibly corrupted", line);
  //     }
  //   // } else {
  //   //   ++i;
  //   // }
  // }
}

void Simulation::importGrid() {
  std::stringstream ss;
  ss <<import_path_grid<<nb_file_i<<".obj";
  std::string str(ss.str());
  std::ifstream file(str.c_str());
  INFO(1, "Import grid file \""<<str<<"\"  >>"<<mpm_conf::replay_speed_);
  if (file.good()) {
    grid.nextStep();
    grid.importGrid(file);
    grid.gridToSubparticules(subparticules);
    ++nb_file_i;

  }
  file.close();
}

void Simulation::exportGrid() const {
  if (nb_file_e % mpm_conf::export_step_ == 0) {
    std::stringstream ss;
    ss <<export_path_grid<<nb_file_e/mpm_conf::export_step_<<".obj";
    std::string str(ss.str());
    std::ofstream file(str.c_str());
    ERROR(file.good(), "cannot open file \""<<str<<"\"", "");
    INFO(1, "Export (grid) file \""<<str<<"\"");
    grid.exportGrid(file);
    file.close();
  }
  ++nb_file_e;
}

void Simulation::importParticules(std::ifstream & file) {
  particules.clear();
  std::string line;
  uint ir = 0, ia = 0;
  uint nb_line = 1;
  while (getline(file, line)) {
    if (line.substr(0,2) == "v ") {
      std::istringstream s(line.substr(2));
      VEC3 v;
      s >> v(0); 
      s >> v(1); 
      s >> v(2);
      Particule *p = new Particule(1, 0.000005, v);
      p->setColor(1, 0.5, 0.5);
      particules.push_back(p);
    } else if (line.substr(0,2) == "r ") {
      std::istringstream s(line.substr(2));
      MAT3 r;
      for (uint i = 0; i < 9; ++i) {
	s >> r(i);
      }
      particules[ir]->setAnisotropyRotation(r);
      ++ir;
    } else if (line.substr(0,2) == "a ") {
      std::istringstream s(line.substr(2));
      FLOAT vx, vy, vz;
      s >> vx >> vy >> vz;
      particules[ia]->setAnisotropyValues(vx, vy, vz);
      ++ia;
    } else if (line[0] == '#') {
    } else {
      WARNING(false, "Imported file possibly corrupted (line "<<nb_line<<")", line);
    }
    ++nb_line;
  }
}

void Simulation::importSubparticules(std::ifstream & file) {
  subparticules.clear();
  std::string line;
  uint ir = 0, ia = 0;
  uint nb_line = 1;
  while (getline(file, line)) {
    if (line.substr(0,2) == "v ") {
      std::istringstream s(line.substr(2));
      VEC3 v;
      s >> v(0); 
      s >> v(1); 
      s >> v(2);
      Subparticule *p = new Subparticule(1, v);
      subparticules.push_back(p);
    } else if (line.substr(0,2) == "r ") {
      std::istringstream s(line.substr(2));
      MAT3 r;
      for (uint i = 0; i < 9; ++i) {
	s >> r(i);
      }
      subparticules[ir]->rotate(r);
      ++ir;
    } else if (line[0] == '#') {
    } else {
      WARNING(false, "Imported file possibly corrupted (line "<<nb_line<<")", line);
    }
    ++nb_line;
  }
}

void Simulation::exportParticules(std::ofstream & file) const {
  file << "# particules \n";
  file << "# "<<particules.size()<<"\n";
  for (auto &p : particules) {
    VEC3 v = p->getPosition();
    file<<"v "<<v(0)<<" "<<v(1)<<" "<<v(2)<<"\n";
  }
  for (auto &p : particules) {
    MAT3 r = p->getRotation();
    file<<"r ";
    for (uint i = 0; i < 9; ++i) {
      file<<r(i)<<" ";
    }
    file<<"\n";
  }
  for (auto &p : particules) {
    VEC3 a = p->getAnisotropy();
    file<<"a "<<a(0)<<" "<<a(1)<<" "<<a(2)<<"\n";
  }
  
}

void Simulation::exportSubparticules(std::ofstream & file) const {
  file << "# subparticules \n";
  file << "# "<<subparticules.size()<<"\n";
  for (auto &p : subparticules) {
    VEC3 v = p->getPosition();
    file<<"v "<<v(0)<<" "<<v(1)<<" "<<v(2)<<"\n";
  }
  for (auto &p : subparticules) {
    MAT3 r = p->getRotation();
    file<<"r ";
    for (uint i = 0; i < 9; ++i) {
      file<<r(i)<<" ";
    }
    file<<"\n";
  }  
}


// void Simulation::exportMesh(std::ofstream & file) const {
//   std::list<VEC3> points;
//   std::list<VEC3> normals;
//   std::list<VEC2> tex_points;
//   std::list<unsigned int> connectivity;

//   for (auto &p : particules) {
//     p->addToMesh(points, normals, tex_points, connectivity);
//   }

//   for (auto &v : points) {
//     file<<"v "<<v(0)<<" "<<v(1)<<" "<<v(2)<<"\n";
//   }
//   for (auto &n : normals) {
//     file<<"vn "<<n(0)<<" "<<n(1)<<" "<<n(2)<<"\n";
//   }
//   for (auto &t : tex_points) {
//     file<<"vt "<<t(0)<<" "<<t(1)<<" "<<t(2)<<"\n";
//   }
//   std::list<unsigned int>::iterator it = connectivity.begin();
//   while (it != connectivity.end()) {
//     file<<"f ";
//     file<<*it<<"/"<<*it<<"/"<<*it<<" ";
//     ++it;
//     file<<*it<<"/"<<*it<<"/"<<*it<<" ";
//     ++it;
//     file<<*it<<"/"<<*it<<"/"<<*it<<"\n";
//     ++it;
//   }
  
// }

void Simulation::exportParticulesAll(std::ofstream & file) const {
  TEST(false); //not implemented
  // file << "# Recorded simulation (conf: "<<conf_file<<", scene: "<<scene_path<<", step: "<<t<< ")\n";
  // for (auto &p : particules) {
  //   VEC3 v = p->getPosition();
  //   file<<"v "<<v(0)<<" "<<v(1)<<" "<<v(2)<<"\n";
  // }
  // for (auto &p : particules) {
  //   MAT3 r = p->getRotation();
  //   file<<"r ";
  //   //    std::cout<<"r ";
  //   for (uint i = 0; i < 9; ++i) {
  //     file<<r(i)<<" ";
  //     //std::cout<<r(i)<<" ";
  //   }
  //   file<<"\n";
  // }
  // for (auto &p : particules) {
  //   VEC3 a = p->getAnisotropy();
  //   file<<"a "<<a(0)<<" "<<a(1)<<" "<<a(2)<<"\n";
  // }
  // for (auto &p : particules) {
  //   file<<"m "<<p->getMass()<<" "<<p->getVolume()<<" "<<p->getDensity()<<"\n";
  // }
  // for (auto &p : particules) {
  //   VEC3 s = p->getVelocity();
  //   file<<"s "<<s(0)<<" "<<s(1)<<" "<<s(2)<<"\n";
  // }
  // for (auto &p : particules) {
  //   file<<"f ";
  //   MAT3 Fe = p->getDeformationElastic();
  //   MAT3 Fp = p->getDeformationPlastic();
  //    for (uint i = 0; i < 9; ++i) {
  //     file<<Fe(i)<<" ";
  //    }
  //    for (uint i = 0; i < 9; ++i) {
  //      file<<Fp(i)<<" ";
  //    }
  // }
  // for (auto &p : particules) {
  //   file<<"c "<<p->getVolumeCorrection()<<"\n";
  // }
  
}

void Simulation::importSim() {
  std::stringstream ss;
  ss <<import_path<<nb_file_i<<".obj";
  std::string str(ss.str());
  std::ifstream file(str.c_str());
  INFO(1, "Import file \""<<str<<"\"  >>"<<mpm_conf::replay_speed_);
  if (file.good()) {
    clearParticules();
    importParticules(file);
    if ((int)nb_file_i >= -mpm_conf::replay_speed_) {
      nb_file_i += mpm_conf::replay_speed_;
    } else {
      nb_file_i = 0;
    }
  } else {
    if ((int)nb_file_i >= 1 && mpm_conf::replay_speed_ < 0 && (int)nb_file_i >= -mpm_conf::replay_speed_) {
      nb_file_i += mpm_conf::replay_speed_;
    }
  }
  file.close();
}

void Simulation::exportSim() const {
  if (nb_file_e % mpm_conf::export_step_ == 0) {
    std::stringstream ss;
    ss <<export_path<<nb_file_e/mpm_conf::export_step_<<".obj";
    std::string str(ss.str());
    std::ofstream file(str.c_str());
    ERROR(file.good(), "cannot open file \""<<str<<"\"", "");
    INFO(1, "Export file \""<<str<<"\"");
    if (subparticules.empty()) {
      exportParticules(file);
    } else {
      exportSubparticules(file);
    }
    file.close();
  }
  ++nb_file_e;
}

void Simulation::setLoad(std::string s) {
  conf_file = s;
  load_conf_ = true;
}

void Simulation::setExport(std::string s) {
  export_path = s;
  export_ = true;
}

void Simulation::setImport(std::string s) {
  import_path = s;
  import_ = true;
}

void Simulation::setExportGrid(std::string s) {
  export_path_grid = s;
  export_grid = true;
}

void Simulation::setImportGrid(std::string s) {
  import_path_grid = s;
  import_grid = true;
}

void Simulation::setExportMitsuba(std::string s) {
  mitsuba_path = s;
  mitsuba = true;
}


void Simulation::setScene(std::string s) {
  scene_path = s;
}

void Simulation::backward(uint n) {
  //  INFO(3, "n "<<n); 
  for (uint i = 0; i < n; ++i) {
    if (nb_file_i >= 1) {
      nb_file_i -= 1;
    } else {
      nb_file_i = 0;
      break;
    }
  }
}

void randomRotation(MAT3 & R) {
  VEC3 x((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
  VEC3 y((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
  VEC3 z((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
  z = x.cross(y);
  y = z.cross(x);
  x.normalize();
  y.normalize();
  z.normalize();
  R.col(0) = x;
  R.col(1) = y;
  R.col(2) = z;
}

void smallRandomRotation(MAT3 & R) {
  VEC3 dir((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
  FLOAT angle = 0.1*(FLOAT)rand()/(FLOAT)RAND_MAX;
  R = utils::rotation(angle, dir);
}

void Simulation::loadScene() {
  std::ifstream file(scene_path.c_str());
  std::string line;
  INFO(3, "load SCENE");

  while (getline(file, line)) {
    if (line.substr(0,11) == "<obstacles>") {
      getline(file, line);
      std::list<Motion> motions;
      while (line.substr(0,12) != "</obstacles>") {
	if (line.substr(0,9) == " <motion>") {
	  getline(file, line);
	  Motion m;
	  while (line.substr(0,10) != " </motion>") {
	    if (line.substr(0,9) == "  <begin>") {
	      std::istringstream s(line.substr(9));
	      s >> m.begin_time;
	    } else if (line.substr(0,7) == "  <end>") {
	      std::istringstream s(line.substr(7));
	      s >> m.end_time;
	    } else if (line.substr(0,9) == "  <scale>") {
	      std::istringstream s(line.substr(9));
	      s >> m.scale;
	      m.scale *= mpm_conf::dt_;
	    } else if (line.substr(0,15) == "  <translation>") {
	      std::istringstream s(line.substr(15));
	      for (uint i = 0; i < 3; ++i) {
		s >> m.translation(i);
	      }
	      m.translation *= mpm_conf::dt_;
	    } else if (line.substr(0,12) == "  <rotation>") {
	      getline(file, line);
	      MAT3 rotation = MAT3::Identity();
	      FLOAT angle = 0;
	      VEC3 axe;
	      while (line.substr(0,13) != "  </rotation>") {
		if (line.substr(0,8) == "   <axe>") {
		  std::istringstream s(line.substr(8));
		  for (uint i = 0; i < 3; ++i) {
		    s >> axe(i);
		  }
		  INFO(3, "AXE \n"<<axe);
		} else  if (line.substr(0,10) == "   <angle>") {
		  std::istringstream s(line.substr(10));
		  s >> angle;
		} else if (line.substr(0,11) == "   <center>") {
		  std::istringstream s(line.substr(11));
		  for (uint i = 0; i < 3; ++i) {
		    s >> m.center(i);
		  }
		  m.rotation_center_def = true;
		} else {
		  std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
		  exit(-1);
		}
		getline(file, line);
	      }
	      rotation = utils::rotation(angle*mpm_conf::dt_, axe);
	      m.rotation = rotation;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  motions.push_back(m);
	} else if (line.substr(0,9) == " <sphere>") {
	  VEC3 center(0, 0, 0);
	  FLOAT ray = 0;
	  FLOAT hr = 0.0;
	  VEC3 hn = VEC3(0, 0, 0);
	  FLOAT fric = mpm_conf::friction_coef_;
	  getline(file, line);
	  while (line.substr(0,10) != " </sphere>") {
	    INFO(3, "sphere "<<line);
	    if (line.substr(0,10) == "  <center>") {
	      std::istringstream s(line.substr(10));
	      for (uint i = 0; i < 3; ++i) {
		s >> center(i);
	      }
	    } else if (line.substr(0,7) == "  <ray>") {
	      std::istringstream s(line.substr(7));
	      s >> ray;
	    } else if (line.substr(0,15) == "  <hole normal>") {
	      std::istringstream s(line.substr(15));
	      for (uint i = 0; i < 3; ++i) {
		s >> hn(i);
	      }
	    } else if (line.substr(0,12) == "  <hole ray>") {
	      std::istringstream s(line.substr(12));
	      s >> hr;
	    } else if (line.substr(0,12) == "  <friction>") {
	      std::istringstream s(line.substr(12));
	      s >> fric;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  SphereObstacle *o = new SphereObstacle(center, ray, hr, hn);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end sphere

	} else if (line.substr(0,6) == " <box>") {
	  VEC3 min(0, 0, 0);
	  VEC3 max(0, 0, 0);
	  FLOAT fric = mpm_conf::friction_coef_;
	  getline(file, line);
	  while (line.substr(0,7) != " </box>") {
	    if (line.substr(0,11) == "  <min pos>") {
	      std::istringstream s(line.substr(11));
	      for (uint i = 0; i < 3; ++i) {
		s >> min(i);
	      }
	    } else if (line.substr(0,11) == "  <max pos>") {
	      std::istringstream s(line.substr(11));
	      for (uint i = 0; i < 3; ++i) {
		s >> max(i);
	      }
	    } else if (line.substr(0,12) == "  <friction>") {
	      std::istringstream s(line.substr(12));
	      s >> fric;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  BoxObstacle *o = new BoxObstacle(min, max);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end box

	} else if (line.substr(0,11) == " <open box>") {
	  VEC3 min(0, 0, 0);
	  VEC3 max(0, 0, 0);
	  FLOAT fric = mpm_conf::friction_coef_;
	  getline(file, line);
	  while (line.substr(0,12) != " </open box>") {
	    if (line.substr(0,11) == "  <min pos>") {
	      std::istringstream s(line.substr(11));
	      for (uint i = 0; i < 3; ++i) {
		s >> min(i);
	      }
	    } else if (line.substr(0,11) == "  <max pos>") {
	      std::istringstream s(line.substr(11));
	      for (uint i = 0; i < 3; ++i) {
		s >> max(i);
	      }
	    } else if (line.substr(0,12) == "  <friction>") {
	      std::istringstream s(line.substr(12));
	      s >> fric;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  OpenBoxObstacle *o = new OpenBoxObstacle(min, max);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end openbox

	}  else if (line.substr(0,8) == " <plane>") {
	  VEC3 pos(0.5, 0.5, 0);
	  VEC3 n(0, 0, 0);
	  FLOAT l = 0, w = 0;
	  FLOAT fric = mpm_conf::friction_coef_;
	  getline(file, line);
	  while (line.substr(0,9) != " </plane>") {
	    if (line.substr(0,7) == "  <pos>") {
	      std::istringstream s(line.substr(7));
	      s >> pos(2);
	    } else if (line.substr(0,12) == "  <position>") {
	      std::istringstream s(line.substr(12));
	      for (uint i = 0; i < 3; ++i) {
		s >> pos(i);
	      }
	    } else if (line.substr(0,10) == "  <normal>") {
	      std::istringstream s(line.substr(10));
	      for (uint i = 0; i < 3; ++i) {
		s >> n(i);
	      }
	    } else if (line.substr(0,12) == "  <friction>") {
	      std::istringstream s(line.substr(12));
	      s >> fric;
	    } else if (line.substr(0,10) == "  <length>") {
	      std::istringstream s(line.substr(10));
	      s >> l;
	    } else if (line.substr(0,9) == "  <width>") {
	      std::istringstream s(line.substr(9));
	      s >> w;
	    } else {
	      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  PlaneObstacle *o = new PlaneObstacle(pos, n, l, w);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end plane

	} else if (line.substr(0,11) == " <cylinder>") {
	  VEC3 pos(0, 0, 0);
	  VEC3 dir(0, 0, 0);
	  FLOAT r = 0;
	  FLOAT fric = mpm_conf::friction_coef_;
	  getline(file, line);
	  while (line.substr(0,12) != " </cylinder>") {
	    if (line.substr(0,7) == "  <pos>") {
	      std::istringstream s(line.substr(7));
	      for (uint i = 0; i < 3; ++i) {
		s >> pos(i);
	      }
	    } else if (line.substr(0,13) == "  <direction>") {
	      std::istringstream s(line.substr(13));
	      for (uint i = 0; i < 3; ++i) {
		s >> dir(i);
	      }
	    } else if (line.substr(0,7) == "  <ray>") {
	      std::istringstream s(line.substr(7));
	      s >> r;
	    } else if (line.substr(0,12) == "  <friction>") {
	      std::istringstream s(line.substr(12));
	      s >> fric;
	    } else {
	      std::cerr<<"Line not recognized in file WWWW\""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  CylinderObstacle *o = new CylinderObstacle(pos, dir, r, 0);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end cylinder

	} else if (line.substr(0,7) == " <ball>") {
	  VEC3 pos(0, 0, 0);
	  FLOAT r = 0;
	  FLOAT fric = mpm_conf::friction_coef_;
	  getline(file, line);
	  while (line.substr(0,8) != " </ball>") {
	    if (line.substr(0,7) == "  <pos>") {
	      std::istringstream s(line.substr(7));
	      for (uint i = 0; i < 3; ++i) {
		s >> pos(i);
	      }
	    } else if (line.substr(0,7) == "  <ray>") {
	      std::istringstream s(line.substr(7));
	      s >> r;
	    } else if (line.substr(0,12) == "  <friction>") {
	      std::istringstream s(line.substr(12));
	      s >> fric;
	    } else {
	      std::cerr<<"Line not recognized in file WWWW\""<<scene_path<<"\": "<<line<<std::endl;
	      exit(-1);
	    }
	    getline(file, line);
	  }
	  BallObstacle *o = new BallObstacle(pos, r, 0);
	  obstacles.push_back(o);
	  o->setFriction(fric);
	  o->setMotions(motions);
	  // end ball
	    
	} else {
	  std::cerr<<"Line not recognized in file FERWE\""<<scene_path<<"\": "<<line<<std::endl;
	  exit(-1);
	}
	getline(file, line);
      }
      // end obstacles
	
    } else if (line.substr(0,12) == "<particules>") {
      MAT3 rotation = MAT3::Identity();
      bool random = false;
      getline(file, line);
      uint nb_sub = 0;
      while (line.substr(0,13) != "</particules>") {
	if (!import_) {
	  if (line.substr(0,11) == " <rotation>") {
	    getline(file, line);
	    FLOAT angle = 0;
	    VEC3 axe;
	    while (line.substr(0,12) != " </rotation>") {
	      if (line.substr(0,7) == "  <axe>") {
		std::istringstream s(line.substr(7));
		for (uint i = 0; i < 3; ++i) {
		  s >> axe(i);
		}
		
	      } else  if (line.substr(0,9) == "  <angle>") {
		std::istringstream s(line.substr(9));
		s >> angle;
	      } else  if (line.substr(0,10) == "  <random>") {
		random = true;
	      } else {
		std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
		exit(-1);
	      }
	      getline(file, line);
	    }
	    rotation = utils::rotation(angle, axe);//*rotation;
	  } else if (line.substr(0,9) == " <cuboid>") {
	    FLOAT xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
	    uint nb_part;
	    VEC3 vel(0, 0, 0);
	    getline(file, line);
	    while (line.substr(0,10) != " </cuboid>") {
	      if (line.substr(0,5) == "  <x>") {
		std::istringstream s(line.substr(5));
		s >> xmin;
		s >> xmax;
	      } else  if (line.substr(0,5) == "  <y>") {
		std::istringstream s(line.substr(5));
		s >> ymin;
		s >> ymax;
	      } else if (line.substr(0,5) == "  <z>") {
		std::istringstream s(line.substr(5));
		s >> zmin;
		s >> zmax;
	      } else if (line.substr(0,17) == "  <nb particules>") {
		std::istringstream s(line.substr(17));
		s >> nb_part;
	      } else if (line.substr(0,20) == "  <nb subparticules>") {
		std::istringstream s(line.substr(20));
		s >> nb_sub;
	      } else if (line.substr(0,12) == "  <velocity>") {
		std::istringstream s(line.substr(12));
		for (uint i = 0; i < 3; ++i) {
		  s >> vel(i);
		}
	    
	      } else {
		std::cerr<<"Line not recognized in file FERWE\""<<scene_path<<"\": "<<line<<std::endl;
		exit(-1);
	      }
	      getline(file, line);
	    }
	    FLOAT h = zmax - zmin, w = xmax - xmin, l = ymax - ymin;
	    FLOAT volume = h*l*w;
	    PoissonGenerator::PRNG prng;
	    std::list<VEC3> points;
	    uint i;
	    if (!import_) {
	      points = PoissonGenerator::GeneratePoissonPointsR(nb_part, prng, 30, VEC3(w, l, h));
	      nb_part = points.size();
	      for (auto &v: points) {
		Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, v/*VEC3(0.05, 0.05, 0.052)*/ + VEC3(xmin, ymin, zmin), VEC3(0, 0, 1), vel);
		particules.push_back(p);
	       
		if (random) {
		  randomRotation(rotation);
		}
		++i;
		p->setAnisotropyValues(0.2, 0.2, 1.5); //just used for display
		p->setAnisotropyRotation(rotation);
	      }
	    }
	    if (nb_sub != 0) {
	      points.clear();
	      points = PoissonGenerator::GeneratePoissonPointsR(nb_sub, prng, 30, VEC3(w, l, h));
	      nb_sub = points.size();
	      for (auto &v: points) {
		Subparticule *p = new Subparticule(volume*mpm_conf::density_/(FLOAT)nb_part, v + VEC3(xmin, ymin, zmin), vel);
		subparticules.push_back(p);
		if (random) {
		  randomRotation(rotation);
		}
		++i;
		p->rotate(rotation);
	      }
	    }
	    //end cuboid
	    
	  } else if (line.substr(0,9) == " <sphere>") {
	    VEC3 center(0, 0, 0);
	    FLOAT ray = 1;
	    uint nb_part;
	    VEC3 vel(0, 0, 0);
	    getline(file, line);
	    while (line.substr(0,10) != " </sphere>") {
	      if (line.substr(0,10) == "  <center>") {
		std::istringstream s(line.substr(10));
		for (uint i = 0; i < 3; ++i) {
		  s >> center(i);
		}
	      } else  if (line.substr(0,7) == "  <ray>") {
		std::istringstream s(line.substr(7));
		s >> ray;
	      } else if (line.substr(0,17) == "  <nb particules>") {
		std::istringstream s(line.substr(17));
		s >> nb_part;
	      } else if (line.substr(0,20) == "  <nb subparticules>") {
		std::istringstream s(line.substr(20));
		s >> nb_part;
	      } else if (line.substr(0,12) == "  <velocity>") {
		std::istringstream s(line.substr(12));
		for (uint i = 0; i < 3; ++i) {
		  s >> vel(i);
		}
	      } else {
		std::cerr<<"Line not recognized in file FERWE\""<<scene_path<<"\": "<<line<<std::endl;
		exit(-1);
	      }
	      getline(file, line);
	    }
	    FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
	    center = center - VEC3(ray/2.0, ray/2.0, ray/2.0);
	    PoissonGenerator::PRNG prng;
	    std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
	    nb_part = points.size();
	    for (auto &v: points) {
	      Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*v + center, VEC3(0, 0, 1), vel);
	      particules.push_back(p);
	      if (random) {
		randomRotation(rotation);
	      }
	      p->setAnisotropyValues(1, 1, 0.1); //just used for display
	      p->setAnisotropyRotation(rotation);
	      
	    }
	    if (nb_sub != 0) {
	      points.clear();
	      points = PoissonGenerator::GeneratePoissonPointsC(nb_sub, prng, 30);
	      nb_sub = points.size();
	      for (auto &v: points) {
		Subparticule *p = new Subparticule(volume*mpm_conf::density_/(FLOAT)nb_part,ray*v + center, vel);
		subparticules.push_back(p);
		if (random) {
		  randomRotation(rotation);
		}
		p->rotate(rotation);
	      }
	    }
	    //sphere
	    
	  } else {
	    std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	    exit(-1);
	  }
	}
	getline(file, line);	
      }
      //end particules
	
#ifndef NO_GRAPHICS_ 
    } else if (line.substr(0,8) == "<camera>") {
      getline(file, line);	
      while (line.substr(0,9) != "</camera>") {
	if (line.substr(0,11) == " <position>") {
	  std::istringstream s(line.substr(11));
	  glm::vec3 pos;
	  for (uint i = 0; i < 3; ++i) {
	    s >> pos[i];
	  }
	  Scene::SCENE->setCameraPosition(pos);
	} else if (line.substr(0,14) == " <orientation>") {
	  std::istringstream s(line.substr(14));
	  glm::vec3 ori;
	  for (uint i = 0; i < 3; ++i) {
	    s >> ori[i];
	  }
	  Scene::SCENE->setCameraOrientation(ori);
	} else {
	  std::cerr<<"Line not recognized in file (<camera>)\""<<scene_path<<"\": "<<line<<std::endl;
	  exit(-1);
	}
	getline(file, line);	
      }
#endif
      // end camera
    } else if (line.substr(0,5) == "<fix>") {
      std::istringstream s(line.substr(5));
      VEC3 min, max;
      for (uint i = 0; i < 3; ++i) {
	s >> min(i);
      }
      for (uint i = 0; i < 3; ++i) {
	s >> max(i);
      }
      fix(min, max);
    } else if (line.substr(0,6) == "<move>") {
      std::istringstream s(line.substr(6));
      VEC3 min, max, trans;
      for (uint i = 0; i < 3; ++i) {
	s >> min(i);
      }
      for (uint i = 0; i < 3; ++i) {
	s >> max(i);
      }
      for (uint i = 0; i < 3; ++i) {
	s >> trans(i);
      }
      move(min, max, trans);
    } else {
      std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
      exit(-1);
    }
      
  } //end main loop

    // DO NOT FORGET TO REMOVE: used for testing
    // mpm_conf::dt_ = 0.0001; 
    //  addRotatingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 30);
    //  addRotatingCubeOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, M_PI*10);
    // addExtendingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 10);
    // addSimpleShearingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 10);
    // addSimpleShearingCubeOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 10);
    //addPurShearingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.2, 10);
    //addPurShearingCubeOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 20);
    // addTranslatingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 1);
    //     addSphereOfCenterOrientedParticules(VEC3(0.5, 0.5, 0.3), 0.2);
    // mpm_conf::anisotropy_on = false;
  // for (auto &p: particules) {
  //   MAT3 r;
  //   smallRandomRotation(r);
  //   p->rotate(r);
  // }
  // for (auto &p: subparticules) {
  //   MAT3 r;
  //   smallRandomRotation(r);
  //   p->rotate(r);
  // }
    

  // TEST

  // EigenSolver<MatrixXd> es;
    
  // //  MAT3 I = MAT3::Identity();
  // MAT3 R = utils::rotation(0.9, VEC3(0, 1, 0));
  // //  MAT3 L ;
  // //  L << 0, 0, 7,
  // //    0, 0, 0,
  // //    -7, 0, 0;

  // // // INFO(3, "R*I\n"<< R*I);
  // // // INFO(3, "D + h(L*D + D*L)\n"<< I + 0.001*(L*I));
    
  //    MAT3 D = MAT3::Identity();
  //    D(0, 0) = 3.1;
  //    D(1, 1) = 3;
  //    D(2, 2) = 3;
       

  //    //INFO(3, "R \n"<<R<<"\n");
       
  //    //       D = R*D*R.transpose();
  //    INFO(3, "\nD :\n"<<D<<"\n");

  //    es.compute(D);
  //   INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
  // //  INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
  //   INFO(3, "eigen vec \n"<<es.eigenvectors().real());
  // //  INFO(3, "eigen vec \n"<<es.eigenvectors().imag());

  //    MAT3 L = MAT3::Zero();
  //    L(2, 0) = 0.1;
  //    L(0, 2) = 0.1;
       
  //    D = D + L*D + D*L;
  //    INFO(3, "\nD t+h :\n"<<D<<"\n");


  //    es.compute(D);
  //    INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
  //    //  INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
  //   INFO(3, "eigen vec \n"<<es.eigenvectors().real());
  // //  INFO(3, "eigen vec \n"<<es.eigenvectors().imag());
       
       
  // //   INFO(3, "R*I\n"<< R*D*R.transpose());
  // //   INFO(3, "I + h(L*I + I*L)\n"<< D + 0.001*(L*D + D*L.transpose()));

  // //   MAT3 D1 = R*D*R.transpose();
  // //   MAT3 D1b =  D + 0.001*(L*D + D*L.transpose());
  // //   INFO(3, "R*I\n"<< R*D1*R.transpose());
  // //   INFO(3, "I + h(L*I + I*L)\n"<< D1b + 0.001*(L*D1b + D1b*L.transpose()));

  //    exit(-1);
    
    
  // MAT3 M = R*D;//*R.transpose();
  // M = 0.5*(M*M.transpose() - MAT3::Identity());
  // M = M -  1.0/3.0*M.trace()* MAT3::Identity();
  //  INFO(3, "M \n"<<M);
  //  //INFO(3, "R \n"<<M);
    
  //  EigenSolver<MatrixXd> es(M);
  //  INFO(3, "eigen values \n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
  //  INFO(3, "eigen values \n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
  //  INFO(3, "eigen vec \n"<<es.eigenvectors().real());
  //  INFO(3, "eigen vec \n"<<es.eigenvectors().imag());

  // es.compute(R*R.transpose());
  // INFO(3, "eigen values R\n"<<es.eigenvalues()(0).real()<<" "<<es.eigenvalues()(1).real()<<" "<<es.eigenvalues()(2).real());
  // INFO(3, "eigen values R\n"<<es.eigenvalues()(0).imag()<<" "<<es.eigenvalues()(1).imag()<<" "<<es.eigenvalues()(2).imag());
  // INFO(3, "eigen vec R\n"<<es.eigenvectors().real());
  // INFO(3, "eigen vec R\n"<<es.eigenvectors().imag());
   
} // end loadScene


void Simulation::saveState(std::string save_file) {
  std::ofstream file(save_file);
  ERROR(file.good(), "cannot open file \""<<save_file<<"\"", "");
  INFO(1, "Save simulation state in file \""<<save_file<<"\"");
  exportParticulesAll(file);
  file.close();
}


void Simulation::addRotatingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT angular_speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
  FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  for (auto &v: points) {
    VEC3 dir = ray*(v-VEC3(0.5, 0.5, 0.5));
    VEC3 vel = angular_speed*dir.cross(n);
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    p->setAnisotropyValues(0.1, 0.1, 1);
    // p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
  }
     
}



void Simulation::addRotatingCubeOfParticules(VEC3 center, FLOAT ray, FLOAT angular_speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsR(nb_part, prng, 30, 2*VEC3(ray, ray, ray));
  FLOAT volume = pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(ray, ray, ray));
    VEC3 vel = angular_speed*dir.cross(n);
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, (v-VEC3(ray, ray, ray)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    //        p->setAnisotropyValues(0.05, 0.05, 1);
    // p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
  }
     
}

void Simulation::addExtendingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
  FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
    VEC3 vel = VEC3(0, speed*(v(1) - 0.5), 0);
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    p->setAnisotropyValues(1, 1, 1); 
    //	p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
  }
     
}


void Simulation::addSimpleShearingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
  FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  //	    uint i = 0;
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
    VEC3 vel = VEC3(0, speed*(v(2) - 0.5), 0);
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    p->setAnisotropyValues(0.1, 0.1, 1);
    p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
  }
     
}

void Simulation::addSimpleShearingCubeOfParticules(VEC3 center, FLOAT ray, FLOAT speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsR(nb_part, prng, 30, 2*VEC3(ray, ray, ray));
  FLOAT volume = pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(ray, ray, ray));
    dir.normalize();
    VEC3 vel = VEC3(0, speed*(v(2) - ray), 0);
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, (v-VEC3(ray, ray, ray)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    p->setAnisotropyValues(1, 1, 1);

    //p->setAnisotropyRotation(utils::rotation(/*1.5708*/0.7, VEC3(1, 0, 0)));
  }
     
}


void Simulation::addPurShearingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
  FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
    dir.normalize();
    VEC3 vel = VEC3(0, speed*(v(1) - 0.5), -speed*(v(2) - 0.5));
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    p->setAnisotropyValues(0.1, 0.1, 1.0);

    //	p->setAnisotropyRotation(utils::rotation(0.8, VEC3(1, 0, 0)));
  }
     
}

void Simulation::addPurShearingCubeOfParticules(VEC3 center, FLOAT ray, FLOAT speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsR(nb_part, prng, 30, 2*VEC3(ray, ray, ray));
  FLOAT volume = pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(ray, ray, ray));
    dir.normalize();
    VEC3 vel = VEC3(0, speed*(v(1) - ray), -speed*(v(2) - ray));
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, (v-VEC3(ray, ray, ray)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    p->setAnisotropyValues(1, 1, 1);

    //	p->setAnisotropyRotation(utils::rotation(/*1.5708*/0.7, VEC3(1, 0, 0)));
  }
     
}


void Simulation::addTranslatingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 10000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
  FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
  nb_part = points.size();
  VEC3 n(-1, 0, 0);
  //	    uint i = 0;
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
    VEC3 vel = VEC3(0, speed, 0);
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
    particules.push_back(p);
    p->setAnisotropyValues(1, 1, 0.1);
    p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
  }
     
}


void Simulation::addSphereOfCenterOrientedParticules(VEC3 center, FLOAT ray) { 
  PoissonGenerator::PRNG prng;
  uint nb_part = 50000;
  uint nb_sub = 200000;
  std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
  FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
  nb_part = points.size();
  VEC3 n(0, 0, 1);
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
    dir.normalize();
    Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), VEC3(0, 0, -3));
    particules.push_back(p);
    p->setAnisotropyValues(1, 1, 0.1);

    VEC3 axe = dir.cross(n);
    axe.normalize();
    FLOAT angle = -acos(dir.dot(n));// + M_PI/2.0;
	
    p->setAnisotropyRotation(utils::rotation(angle, axe));
  }
  points.clear();
  points = PoissonGenerator::GeneratePoissonPointsC(nb_sub, prng, 30);
  nb_sub = points.size();
  for (auto &v: points) {
    VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
    dir.normalize();
    Subparticule *p = new Subparticule(volume*mpm_conf::density_/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, -3));
    subparticules.push_back(p);
  
    VEC3 axe = dir.cross(n);
    axe.normalize();
    FLOAT angle = -acos(dir.dot(n));// + M_PI/2.0;
    MAT3 rot = utils::rotation(angle, axe);
    p->rotate(rot);
  }
}


void Simulation::fix(VEC3 min, VEC3 max) {
  grid.fix(min, max);
}


void Simulation::move(VEC3 min, VEC3 max, VEC3 trans) {
  grid.move(min, max, trans);
}

void Simulation::exportMitsuba() const{
  if (nb_file_mitsuba % mpm_conf::export_step_ == 0) {
    std::stringstream ss;
    uint nb = nb_file_mitsuba/mpm_conf::export_step_;
    if (nb < 10) {
      ss <<mitsuba_path<<"000"<<nb<<".xml";
    } else if (nb < 100) {
      ss <<mitsuba_path<<"00"<<nb<<".xml";
    } else if (nb < 1000) {
      ss <<mitsuba_path<<"0"<<nb<<".xml";
    } else {
      ss <<mitsuba_path<<nb<<".xml";
    }
    std::string str(ss.str());
    exportMitsuba(str);
  }
  ++nb_file_mitsuba;
}

void Simulation::exportMitsuba(std::string file_name) const {
  std::ofstream file(file_name.c_str());
  ERROR(file.good(), "cannot open file \""<<file_name<<"\"", "");
  INFO(1, "Export file \""<<file_name<<"\"");

  std::string diff_reflectance = "#ddd224"; // yellowish (truck)
  //std::string diff_reflectance = "#dda824"; // yellowy brownish (wood)
  //  std::string diff_reflectance = "#DBEDFF"; // dull blue 
  // std::string diff_reflectance = "#a90202"; //brownish (rod)
  //std::string diff_reflectance = "#c19c8a"; // red brownish
  //std::string diff_reflectance = "#f94242"; // dull red 
  //std::string diff_reflectance = "#a92222"; // blood red (sparkle)
  std::string truck_color = "#aaaaaa";
  std::string integrator = "direct";

  file<<"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  file<<"<scene version=\"0.5.0\">\n";
  file<<"<integrator type=\""<<integrator<<"\"/>\n";

  /** WOOD ***/
    
  file<<"<shape  type=\"shapegroup\" id=\"particle\">\n";
  file<<"<shape type=\"sphere\">\n";
  file<<"<bsdf type=\"diffuse\">\n";
  file<<"<srgb name=\"reflectance\" value=\""<<diff_reflectance<<"\"/>\n";
  file<<"</bsdf>\n";
  file<<"</shape>\n";
  file<<"</shape>\n";
  /* END WOOD */

  /*** sparkle   !!! in some version of mitsuba, I have a division-by-zero error when using disks **/
  /*
    file<<"<shape  type=\"shapegroup\" id=\"particle\">\n";

    file<<"<shape type=\"cylinder\">\n";
    file<<"<point name=\"p0\" x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<-0.001<<"\"/>\n";
    file<<"<point name=\"p1\" x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<0.001<<"\"/>\n";
    file<<"<float name=\"radius\" value=\""<<0.02<<"\"/>\n";
    file<<"<bsdf type=\"diffuse\">\n";
    file<<"<srgb name=\"reflectance\" value=\""<<diff_reflectance<<"\"/>\n";
    file<<"</bsdf>\n";
    file<<"</shape>\n";

    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<scale  value=\""<<0.02<<"\"/>\n";
    file<<"<translate x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<0.001<<"\"/>\n";
    file<<"</transform>\n";
    file<<"<bsdf type=\"twosided\">\n";
    file<<"<bsdf type=\"plastic\">\n";
    file<<"<srgb name=\"diffuseReflectance\" value=\""<<diff_reflectance<<"\"/>\n";
    file<<"</bsdf>\n";
    file<<"</bsdf>\n";
    file<<"</shape>\n";

    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<scale  value=\""<<0.02<<"\"/>\n";
    file<<"<translate x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<-0.001<<"\"/>\n";
    file<<"</transform>\n";
    file<<"<bsdf type=\"twosided\">\n";
    file<<"<bsdf type=\"plastic\">\n";
    file<<"<srgb name=\"diffuseReflectance\" value=\""<<diff_reflectance<<"\"/>\n";
    file<<"</bsdf>\n";
    file<<"</bsdf>\n";
    file<<"</shape>\n";

    file<<"</shape>\n";

    /** end sparkle **/
    
  /** rod ***/
  /*
    file<<"<shape  type=\"shapegroup\" id=\"particle\">\n";
    file<<"<shape type=\"cylinder\">\n";
    file<<"<point name=\"p0\" x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<-0.04<<"\"/>\n";
    file<<"<point name=\"p1\" x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<0.04<<"\"/>\n";
    file<<"<float name=\"radius\" value=\""<<0.01<<"\"/>\n";
    file<<"<bsdf type=\"diffuse\">\n";
    file<<"<srgb name=\"reflectance\" value=\""<<diff_reflectance<<"\"/>\n";
    file<<"</bsdf>\n";
    file<<"</shape>\n";
    // file<<"<shape  type=\"disk\">\n";
    // file<<"<transform  name=\"toWorld\">\n";
    // file<<"<scale  value=\""<<0.02<<"\"/>\n";
    // file<<"<translate x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<-0.07<<"\"/>\n";
    // file<<"</transform>\n";
    // file<<"<bsdf type=\"twosided\">\n";
    // file<<"<bsdf type=\"diffuse\">\n";
    // file<<"<srgb name=\"reflectance\" value=\""<<diff_reflectance<<"\"/>\n";
    // file<<"</bsdf>\n";
    // file<<"</bsdf>\n";
    // file<<"</shape>\n";
    // file<<"<shape  type=\"disk\">\n";
    // file<<"<transform  name=\"toWorld\">\n";
    // file<<"<scale  value=\""<<0.02<<"\"/>\n";
    // file<<"<translate x=\""<<0<<"\" y=\""<<0<<"\" z=\""<<0.07<<"\"/>\n";
    // file<<"</transform>\n";
    // file<<"<bsdf type=\"twosided\">\n";
    // file<<"<bsdf type=\"diffuse\">\n";
    // file<<"<srgb name=\"reflectance\" value=\""<<diff_reflectance<<"\"/>\n";
    // file<<"</bsdf>\n";
    // file<<"</bsdf>\n";
    // file<<"</shape>\n";
    file<<"</shape>\n";

    /* END rod */

  /** paper **/
  /*
    file<<"<shape  type=\"shapegroup\" id=\"particle\">\n";
    file<<"<shape type=\"rectangle\">\n";
    file<<"<transform name=\"toWorld\">\n";
    file<<"<scale x=\""<<0.05<<"\" y=\""<<0.05<<"\" z=\"1\"/>\n";
    // file<<"<rotate x=\""<<axe(0)<<"\" y=\""<<axe(1)<<"\" z=\""<<axe(2)<<"\" angle=\""<<angle<<"\"/>\n";
    file<<"</transform>";
    file<<"<bsdf  type=\"twosided\">\n";
    file<<"<bsdf type=\"diffuse\">\n";
    file<<"<srgb name=\"reflectance\" value=\"#FFFFFF\"/>\n";
    file<<"</bsdf>\n";
    file<<"</bsdf>\n";
    file<<"</shape>\n";
    file<<"</shape>\n";
    /** END paper **/

  /** mesh **/
  /*
    file<<"<shape  type=\"shapegroup\" id=\"particle\">\n";
    file<<"<shape type=\"obj\">\n";
    file<<"<string  name=\"filename\"  value=\"dragon.obj\"/>\n";
    file<<"<transform name=\"toWorld\">\n";
    file<<"<scale x=\"0.0075\" y=\"0.0075\" z=\"0.0075\"/>\n";
    //      file<<"<rotate x=\""<<1<<"\" y=\""<<0<<"\" z=\""<<0<<"\" angle=\""<<90<<"\"/>\n";
    file<<"<rotate x=\""<<0<<"\" y=\""<<1<<"\" z=\""<<0<<"\" angle=\""<<90<<"\"/>\n";
    file<<"</transform>\n";
    file<<"<bsdf type=\"roughplastic\">\n";
    file<<"<srgb name=\"diffuseReflectance\" value=\""<<diff_reflectance<<"\"/>\n";
    file<<"</bsdf>\n";
    file<<"</shape>\n";
    file<<"</shape>\n";
    /* end mesh*/
    
  if (subparticules.empty()) {
    for (auto &p : particules) {
      p->exportMitsuba(file);
    }
  } else {
    for (auto &p : subparticules) {
      p->exportMitsuba(file);
    }
  }

  for (auto &o : obstacles) {
    o->exportMitsuba(file);
  }

  /*** stretch ***/
  /*
    VEC3 p0(0.5, 0.2-time*0.0005, 0.65);
    VEC3 p1(0.5, 0.25-time*0.0005, 0.65);
    FLOAT ray = 0.12;
    file<<"<shape type=\"cylinder\">\n";
    file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
    file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
    file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
    file<<"</shape>\n";
    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<rotate  x=\"1\"  angle=\"90\"/>\n";
    file<<"<scale  value=\""<<10*ray<<"\"/>\n";
    file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
    file<<"</transform>\n";
    file<<"</shape>\n";
    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<rotate  x=\"1\"  angle=\"90\"/>\n";
    file<<"<scale  value=\""<<10*ray<<"\"/>\n";
    file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
    file<<"</transform>\n";
    file<<"</shape>\n";
    p0= VEC3(0.5, 0.8+time*0.0005, 0.65);
    p1 = VEC3(0.5, 0.75+time*0.0005, 0.65);
    ray = 0.12;
    file<<"<shape type=\"cylinder\">\n";
    file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
    file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
    file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
    file<<"</shape>\n";
    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<rotate  x=\"1\"  angle=\"90\"/>\n";
    file<<"<scale  value=\""<<10*ray<<"\"/>\n";
    file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
    file<<"</transform>\n";
    file<<"</shape>\n";
    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<rotate  x=\"1\"  angle=\"90\"/>\n";
    file<<"<scale  value=\""<<10*ray<<"\"/>\n";
    file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
    file<<"</transform>\n";
    file<<"</shape>\n";
    /* end stetch */

  /*** bend */
  /*
    VEC3 p0(0.5, 0.18, 0.85);
    VEC3 p1(0.5, 0.24, 0.85);
    FLOAT ray = 0.12;
    file<<"<shape type=\"cylinder\">\n";
    file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
    file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
    file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
    file<<"</shape>\n";
    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<rotate  x=\"1\"  angle=\"90\"/>\n";
    file<<"<scale  value=\""<<10*ray<<"\"/>\n";
    file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
    file<<"</transform>\n";
    file<<"</shape>\n";
    file<<"<shape  type=\"disk\">\n";
    file<<"<transform  name=\"toWorld\">\n";
    file<<"<rotate  x=\"1\"  angle=\"90\"/>\n";
    file<<"<scale  value=\""<<10*ray<<"\"/>\n";
    file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
    file<<"</transform>\n";
    file<<"</shape>\n";
    /****end bend **/
    
  /*** truck ***/
    
  file<<"<bsdf type=\"twosided\" id=\"truck\">\n";
  file<<"<bsdf type=\"diffuse\">\n";
  file<<"<srgb name=\"diffuseReflectance\" value=\""<<truck_color<<"\"/>\n";
  file<<"</bsdf>\n";
  file<<"</bsdf>\n";
    
  VEC3 p0(0.26, 0.13, 0.18);
  VEC3 p1(0.3, 0.13, 0.18);
  FLOAT ray = 0.08;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.26, 0.57, 0.18);
  p1 = VEC3(0.3, 0.57, 0.18);
  ray = 0.08;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.74, 0.57, 0.18);
  p1 = VEC3(0.7, 0.57, 0.18);
  ray = 0.08;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.74, 0.13, 0.18);
  p1 = VEC3(0.7, 0.13, 0.18);
  ray = 0.08;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.5, 0.0, 0.18);
  p1 = VEC3(0.5, 0.64, 0.18);
  ray = 0.01;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate x=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate x=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.3, 0.13, 0.18);
  p1 = VEC3(0.7, 0.13, 0.18);
  ray = 0.01;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.3, 0.57, 0.18);
  p1 = VEC3(0.7, 0.57, 0.18);
  ray = 0.01;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
    
  file<<"<shape  type=\"cube\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<scale x=\"1.7\" y=\"0.7\" z=\"0.85\" />\n";
  file<<"<translate x=\""<<5<<"\" y=\""<<-2.45<<"\" z=\""<<2.85<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"cube\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<scale x=\"1.7\" y=\"1\" z=\"1.25\" />\n";
  file<<"<translate x=\""<<5<<"\" y=\""<<-1.2<<"\" z=\""<<3.25<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.29, -0.12, 0.18);
  p1 = VEC3(0.33, -0.12, 0.18);
  ray = 0.08;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.71, -0.12, 0.18);
  p1 = VEC3(0.67, -0.12, 0.18);
  ray = 0.08;
  file<<"<shape type=\"cylinder\">\n";
  file<<"<point name=\"p0\" x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"<point name=\"p1\" x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"<float name=\"radius\" value=\""<<10*ray<<"\"/>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  file<<"<shape  type=\"disk\">\n";
  file<<"<transform  name=\"toWorld\">\n";
  file<<"<rotate  y=\"1\"  angle=\"90\"/>\n";
  file<<"<scale  value=\""<<10*ray<<"\"/>\n";
  file<<"<translate x=\""<<10*p1(0)<<"\" y=\""<<10*p1(1)<<"\" z=\""<<10*p1(2)<<"\"/>\n";
  file<<"</transform>\n";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";

  p0 = VEC3(0.5, 1.01, 0.13);
  file<<"<shape type=\"cube\">\n";
  file<<"<transform name=\"toWorld\">\n";
  file<<"<scale x=\""<<5.4<<"\" y=\""<<0.2<<"\" z=\"0.3\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  p0 = VEC3(1.01, 0.5, 0.13);
  file<<"<shape type=\"cube\">\n";
  file<<"<transform name=\"toWorld\">\n";
  file<<"<scale x=\""<<0.2<<"\" y=\""<<5.1<<"\" z=\"0.3\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
  p0 = VEC3(-0.01, 0.5, 0.13);
  file<<"<shape type=\"cube\">\n";
  file<<"<transform name=\"toWorld\">\n";
  file<<"<scale x=\""<<0.2<<"\" y=\""<<5.1<<"\" z=\"0.3\"/>\n";
  file<<"<translate x=\""<<10*p0(0)<<"\" y=\""<<10*p0(1)<<"\" z=\""<<10*p0(2)<<"\"/>\n";
  file<<"</transform>";
  file<<"<ref name=\"bsdf\" id=\"truck\"/>\n";
  file<<"</shape>\n";
    
  /* end truck */

  /*** avalanche **/
  /* 
     std::string color = "bbbbbb";
     file<<"<shape type=\"rectangle\">\n";
     file<<"<transform name=\"toWorld\">\n";
     file<<"<scale x=\""<<6<<"\" y=\""<<10<<"\" z=\"1\"/>\n";
     file<<"<rotate y =\"1\" angle=\"90\"/>\n";
     file<<"<rotate x=\"-1\" y=\"0\" z=\"0\" angle=\"26.5651\"/>\n";
     file<<"<translate x=\"1.8\"/>\n";
     file<<"</transform>";
     file<<"<bsdf  type=\"twosided\">\n";
     file<<"<bsdf type=\"diffuse\">\n";
     file<<"<srgb name=\"reflectance\" value=\"#"<<color<<"\"/>\n";
     file<<"</bsdf>\n";
     file<<"</bsdf>\n";
     file<<"</shape>\n";
     file<<"<shape type=\"rectangle\">\n";
     file<<"<transform name=\"toWorld\">\n";
     file<<"<scale x=\""<<6<<"\" y=\""<<10<<"\" z=\"1\"/>\n";
     file<<"<rotate y =\"1\" angle=\"90\"/>\n";
     file<<"<rotate x=\"-1\" y=\"0\" z=\"0\" angle=\"26.5651\"/>\n";
     file<<"<translate x=\"8.2\"/>\n";
     file<<"</transform>";
     file<<"<bsdf  type=\"twosided\">\n";
     file<<"<bsdf type=\"diffuse\">\n";
     file<<"<srgb name=\"reflectance\" value=\"#"<<color<<"\"/>\n";
     file<<"</bsdf>\n";
     file<<"</bsdf>\n";
     file<<"</shape>\n";

     /* end avalanche */

   
    
    
  file<<"<sensor type=\"perspective\">\n";
  file<<"<float name=\"focusDistance\" value=\"2.78088\"/>\n";
  file<<"<float name=\"fov\" value=\"64\"/>\n";
  file<<"<string name=\"fovAxis\" value=\"x\"/>\n";
  file<<"<transform name=\"toWorld\">\n";
#ifndef NO_GRAPHICS_
  glm::vec3 pos = Scene::SCENE->getCameraPosition(), target = pos+Scene::SCENE->getCameraOrientation(), up = glm::vec3(0, 0, 1);

  file<<"<lookat target=\""<<10*target[0]<<", "<<10*target[1]<<", "<<10*target[2]<<"\" origin=\""<<10*pos[0]<<", "<<10*pos[1]<<", "<<10*pos[2]<<"\" up=\""<<up[0]<<", "<<up[1]<<", "<<up[2] <<"\"/>\n";
#else
  // file<<"<lookat target=\"6.81088, 6.61976, 0.80722\" origin=\"1.95829, 1.41833, 7.83555\" up=\"0, 0, 1\"/>\n"; //rod
  // file << "<lookat target=\"8.38793, 2.62964, 2.25414\" origin=\"0.550203, 8.15497, 5.08973\" up=\"0, 0, 1\"/>\n";  // paper
  // file <<"<lookat target=\"2.94714, 7.60629, 4.24315\" origin=\"-2.1648, 13.7611, 10.242\" up=\"0, 0, 1\"/>\n"; //truck
  // file << "<lookat target=\"2.90936, 6.57208, 4.24429\" origin=\"0.664482, 15.379, 8.41539\" up=\"0, 0, 1\"/>\n"; // avalanche further
  // file <<"<lookat target=\"5.08034, 4.86198, 0.18152\" origin=\"5.0365, 10.1977, 8.63893\" up=\"0, 0, 1\"/>\n"; //up avalanche 2
  // file<<"<lookat target=\"4.6261, 3.93709, 1.31134\" origin=\"4.45182, 12.8159, 5.90882\" up=\"0, 0, 1\"/>\n"; //up avalanche
  // file<< "<lookat target=\"5.9242, -0.5821, 1.24226\" origin=\"1.93691, 8.05538, 4.32371\" up=\"0, 0, 1\"/>\n"; // avalanche close-up
  // file<< "<lookat target=\"7.41017, -1.41846, 1.16168\" origin=\"3.16852, 7.07321, 4.30807\" up=\"0, 0, 1\"/>\n"; // avalanche close-up 2
  // file<<"<lookat target=\"4.44281, 2.32609, 2.14493\" origin=\"4.30529, 12.2445, 3.41229\" up=\"0, 0, 1\"/>\n"; //avalanche
  file<< "<lookat target=\"6.61536, 5.00178, 4.32453\" origin=\"-2.74088, 5.04394, 7.85426\" up=\"0, 0, 1\"/>\n"; //stretch 2
  // file<<"<lookat target=\"8.09562, 4.88576, 4.0366\" origin=\"-1.46929, 4.7329, 6.95021\" up=\"0, 0, 1\"/>\n"; //stretch
  // file<<"<lookat target=\"4.93231, 5.18918, 0.878829\" origin=\"4.55552, 4.85386, 10.8661\" up=\"0, 0, 1\"/>\n"; //up
  // file<<"<lookat target=\"7.8895, 3.96616, 6.29996\" origin=\"-1.44009, 4.35116, 9.87913\" up=\"0, 0, 1\"/>\n"; //bend
  // file<<"<lookat target=\"9.19033, 6.21065, 3.34206\" origin=\"-0.241074, 3.25843, 4.86951\" up=\"0, 0, 1\"/>\n";
  // file<<"<lookat target=\"6.85785, 5.42408, 3.87225\" origin=\"-2.57355, 2.47186, 5.3997\" up=\"0, 0, 1\"/>\n";
  // file<<"<lookat target=\"0.203847, 3.68133, 4.82679\" origin=\"-9.72093, 4.64086, 5.58709\" up=\"0, 0, 1\"/>\n";                  
#endif
  file<<"</transform>\n";
  file<<"<sampler type=\"ldsampler\">\n";
  file<<"<integer name=\"sampleCount\" value=\"64\"/>\n";
  file<<"</sampler>\n";
  file<<"<film type=\"hdrfilm\">\n";
  file<<"<boolean name=\"banner\" value=\"false\"/>\n";
  file<<"<integer name=\"height\" value=\"720\"/>\n";
  file<<"<string name=\"pixelFormat\" value=\"rgb\"/>\n";
  file<<"<integer name=\"width\" value=\"1280\"/>\n";
  file<<"<rfilter type=\"gaussian\"/>\n";
  file<<"</film>\n";
  file<<"</sensor>\n";


  file<<"<shape type=\"sphere\">\n";
  file<<"<point name=\"center\" x=\"-15\" y=\"-15\" z=\"50\"/>\n";
  file<<"<float name=\"radius\" value=\"4.0\"/>\n";
  file<<"<emitter type=\"area\">\n";
  file<<"<spectrum name=\"radiance\" value=\"320\"/>\n";
  file<<"</emitter>\n";
  file<<"</shape>\n";

  file<<"<emitter type=\"sky\">\n";
  file<<"<transform name=\"toWorld\">\n";
  file<<"<rotate x=\"1\" angle=\"90\"/>\n";
  file<<"</transform>\n";
  file<<"<float name=\"scale\" value=\"4\"/>\n";
  file<<"<vector name=\"sunDirection\" x=\"0\" y=\"40\" z=\"20\"/>\n";
  file<<"</emitter>\n";

  file<<"</scene>\n";

  file.close();
}
