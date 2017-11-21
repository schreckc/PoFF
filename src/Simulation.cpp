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

  // conf_file = "test.conf";
  // export_path = "object/test";
  // import_path = "object/test";
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

    if (load_conf_) {
      mpm_conf::loadConf(conf_file);
    }
  
  if (!import_) {

    grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, 2);
    loadScene();

    grid.init(particules);

    // for (auto & o : obstacles) {
    //   grid.initCollision(o);
    // }
    grid.initCollision(obstacles);
    
    INFO(1, "Lame Parameters : lambda = "<<mpm_conf::lambda_<<"    mu = "<<mpm_conf::mu_<<"\n");
    INFO(1, "dt = "<<mpm_conf::dt_<<"\n");
    INFO(3, "obstacles = "<<obstacles.size()<<"\n");
    if (export_) {
      exportSim();
    }
  
  } else {
    importSim();
    loadScene();
  }
  t = 0;
}

void Simulation::clearParticules() {
 for (auto& p : particules) {
    // INFO(3,"delete p"<<" "<<p);
    delete p;
  }
  particules.clear();
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
  ++t;
  //  INFO(1, "Simulation step : "<<t);
  if (!import_) {
    oneStep();
    //        mpm_conf::anisotropy_values_(2) += 0.01;
    if (export_) {
      exportSim();
    }
    for (auto &ob : obstacles) {
      ob->animate();
    }
  } else {
    importSim();
    INFO(3, particules.front()->getVolume());
    for (uint s = 0; s < mpm_conf::replay_speed_; ++s) {
      for (auto &ob : obstacles) {
	ob->animate();
      }
    } 
  }
}

#ifndef NO_GRAPHICS_ 
void Simulation::draw(glm::mat4 m, int s) {
  // QString text_ls("");
  //  if (!stop_) {
  //    if (load) {
  //      text_ls = QString("using ");
  //      text_ls += file_l;
  //    } else if (import_) {
  //      text_ls = QString("importing ");
  //      text_ls += file_i;
  //    }
  //    drawText(width() - 200, height() - 7, text_ls);
  //  }
  
  Times::TIMES->tick(Times::display_time_);
  uint cur_shader = m_shader;
  if (m_shader == -1) {
    cur_shader = s;
  }
  glm::mat4 cur_model = m * m_model_view;

   grid.draw(cur_model, cur_shader);

  enableShader();
  for (auto& p : particules) {
    // p->modelView() =  m_model_view * p->modelView();
    p->draw(cur_model, cur_shader);
    //    INFO(3, p->getPosition());
  }
  for (auto& ob : obstacles) {
    //ob->modelView() =  m_model_view * ob->modelView();
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
    INFO(3, "SMOOTH");
  }
  // for (auto & ob : obstacles) {
  //    //INFO(3, "collision "<<obstacles.size() );
  //   grid.collision(ob);
  //  }
  grid.collision(obstacles);
  grid.gridToParticules(particules);

   // for (auto & ob : obstacles) {
   //   grid.initCollision(ob); //set for each node previous distance to the closest obstacle
   // }
  grid.initCollision(obstacles);
   Times::TIMES->tock(Times::simu_time_);
}

void Simulation::importParticulesAll(std::ifstream & file) {
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

void Simulation::importParticules(std::ifstream & file) {
  particules.clear();
  std::string line;
  uint ir = 0, ia = 0;
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
	// INFO(3, p->getPosition());
      } else if (line.substr(0,2) == "r ") {
	std::istringstream s(line.substr(2));
	//INFO(3, line);
	MAT3 r;
	for (uint i = 0; i < 9; ++i) {
	  s >> r(i);
	}
	particules[ir]->setAnisotropyRotation(r);//setAnisotropyAxes(VEC3(r(0), r(3), r(6)), VEC3(r(1), r(4), r(7)), VEC3(r(2), r(5), r(8)));
	++ir;
	//INFO(3, r);
      } else if (line.substr(0,2) == "a ") {
	std::istringstream s(line.substr(2));
	//INFO(3, line);
	FLOAT vx, vy, vz;
	s >> vx >> vy >> vz;
	particules[ia]->setAnisotropyValues(vx, vy, vz);
	++ia;
      } else if (line[0] == '#') {
	//INFO(3, "COMMENT "<<line);
      } else {
	WARNING(false, "Imported file possibly corrupted", line);
      }
    // } else {
    //   ++i;
    // }
  }
}

void Simulation::exportParticules(std::ofstream & file) const {
  file << "# particules \n";
  for (auto &p : particules) {
    VEC3 v = p->getPosition();
    file<<"v "<<v(0)<<" "<<v(1)<<" "<<v(2)<<"\n";
  }
  for (auto &p : particules) {
    MAT3 r = p->getRotation();
    file<<"r ";
    //    std::cout<<"r ";
    for (uint i = 0; i < 9; ++i) {
      file<<r(i)<<" ";
      //std::cout<<r(i)<<" ";
    }
    file<<"\n";
    //std::cout<<"\n";
  }
   for (auto &p : particules) {
    VEC3 a = p->getAnisotropy();
    file<<"a "<<a(0)<<" "<<a(1)<<" "<<a(2)<<"\n";
  }
  
}

void Simulation::exportParticulesAll(std::ofstream & file) const {
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
    //    if ((int)nb_file_i >= -mpm_conf::replay_speed_) {
      nb_file_i += mpm_conf::replay_speed_;
      //      INFO(1, "Import file \""<<str<<"\"  >>"<<mpm_conf::replay_speed_<<" "<<nb_file_i);
      //} // else {
    //   mpm_conf::replay_speed_ = 1;
    // }
  } else {
    if ((int)nb_file_i >= 1 && mpm_conf::replay_speed_ < 0 && (int)nb_file_i >= -mpm_conf::replay_speed_) {
     nb_file_i += mpm_conf::replay_speed_;
    }//  else{
    //   mpm_conf::replay_speed_ = 1;
    // }
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
    exportParticules(file);
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

void Simulation::setScene(std::string s) {
  scene_path = s;
}

void Simulation::backward(uint n) {
  if (nb_file_i >= 1) {
    nb_file_i -= 1;
  } else {
    nb_file_i = 0;
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

void Simulation::loadScene() {
    std::ifstream file(scene_path.c_str());
      std::string line;
      INFO(3, "load SCENE");

    while (getline(file, line)) {
      //     INFO(3, "line "<<line);
      if (line.substr(0,11) == "<obstacles>") {
	getline(file, line);
	std::list<Motion> motions;
	while (line.substr(0,12) != "</obstacles>") {
	  // INFO(3, "Obsctacle "<<line);
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
	    //INFO(3, "obstacles = "<<center<<"\n"<<ray);
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
	    //INFO(3, "obstacles = "<<min<<"\n"<<max);
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
	    //INFO(3, "obstacles = "<<min<<"\n"<<max);
	  // end box

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
	    //INFO(3, "obstacles = "<<pos<<"\n"<<n);
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
	    //INFO(3, "obstacles = "<<pos<<"\n"<<n);
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
	    //INFO(3, "obstacles = "<<pos<<"\n"<<n);
	    // end cylinder
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
	//	INFO(3, "line "<<line);
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
		// rotation << 1, 0, 0,
	      // 	0, sqrt(2.0)*0.5, sqrt(2.0)*0.5,
	      // 	0, -sqrt(2.0)*0.5, sqrt(2.0)*0.5;
	      //INFO(3, "rot \n"<<rotation);
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
	    // for(uint i = 0; i < nb_part; ++i) {
	    //   VEC3 r(xmin + w*rand()/RAND_MAX, ymin + l*rand()/RAND_MAX, zmin + h*rand()/RAND_MAX);
	    //   //      VEC3 r(0.515, 0.515, 0.515);
	    //   Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, r, VEC3(0, 0, 1), vel);
	    //   particules.push_back(p);

	    //   // VEC3 x((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	    //   // VEC3 y((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	    //   // VEC3 z((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	    //   VEC3 x(1, 0, 0);
	    //   VEC3 y(0, 1, 0);
	    //   VEC3 z(0, 0, 1);
	      
	    //   p->setAnisotropyAxes(x, y, z);
	    //   p->setAnisotropyValues(0.5, 0.5, 2);
	      
	    // }
	    PoissonGenerator::PRNG prng;
	    std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsR(nb_part, prng, 30, VEC3(w, l, h));
	    uint i;
	    nb_part = points.size();
	    for (auto &v: points) {
	      Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, v/*VEC3(0.05, 0.05, 0.052)*/ + VEC3(xmin, ymin, zmin), VEC3(0, 0, 1), vel);
	       particules.push_back(p);

	       if (random) {
		 randomRotation(rotation);
	       }
	         // if (i % 2 == 0) {
	       // 	 p->setAnisotropyValues(1, 1, 0.1);
	       // } else {
	       // 	 p->setAnisotropyValues(1, 1, 0.2);
	       // }
	       //	       INFO(3, "rot2 \n"<<rotation);
	       ++i;
	       p->setAnisotropyValues(0.2, 0.2, 1.5);
	       p->setAnisotropyRotation(rotation);
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
	    // for(uint i = 0; i < nb_part; ++i) {
	    //   VEC3 r(xmin + w*rand()/RAND_MAX, ymin + l*rand()/RAND_MAX, zmin + h*rand()/RAND_MAX);
	    //   //      VEC3 r(0.515, 0.515, 0.515);
	    //   Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, r, VEC3(0, 0, 1), vel);
	    //   particules.push_back(p);

	    //   // VEC3 x((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	    //   // VEC3 y((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	    //   // VEC3 z((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	    //   VEC3 x(1, 0, 0);
	    //   VEC3 y(0, 1, 0);
	    //   VEC3 z(0, 0, 1);
	      
	    //   p->setAnisotropyAxes(x, y, z);
	    //   p->setAnisotropyValues(0.5, 0.5, 2);
	      
	    // }
	    PoissonGenerator::PRNG prng;
	    std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
	    nb_part = points.size();
	    //	    uint i = 0;
	    for (auto &v: points) {
	      Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*v + center, VEC3(0, 0, 1), vel);
	       particules.push_back(p);

	       // // VEC3 x((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       // // VEC3 y((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       // // VEC3 z((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       // VEC3 x(1, 0, 0);
	       // VEC3 y(0, 1, 0);
	       // VEC3 z(0, 0, 1);
	      
	       // p->setAnisotropyAxes(x, y, z);

	       if (random) {
		 randomRotation(rotation);
	       }
	       // if (i % 2 == 0) {
		 p->setAnisotropyValues(1, 1, 0.1);
	       // } else {
	       // 	 p->setAnisotropyValues(1, 1, 0.2);
	       // }
	       // ++i;
	       p->setAnisotropyRotation(rotation);
	      
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
      } else {
	std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	exit(-1);
      }
      
    } //end main loop

    // DO NOT FORGET TO REMOVE
    //mpm_conf::dt_ = 0.0001; 
    //    addRotatingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 50);
    // addRotatingCubeOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 100);
    //  addExtendingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 10);
    //   addSimpleShearingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 3);
    // addPurShearingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 10);
    //addTranslatingSphereOfParticules(VEC3(0.5, 0.5, 0.45), 0.1, 1);
    //    mpm_conf::anisotropy_on = false;


    

    // // // TEST

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
     //	    uint i = 0;
     for (auto &v: points) {
       VEC3 dir = ray*(v-VEC3(0.5, 0.5, 0.5));
       //dir.normalize();
       VEC3 vel = angular_speed*dir.cross(n);
       Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
       particules.push_back(p);
        p->setAnisotropyValues(1, 1, 1);
	// p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
     }
     
}



void Simulation::addRotatingCubeOfParticules(VEC3 center, FLOAT ray, FLOAT angular_speed) { 
     PoissonGenerator::PRNG prng;
     uint nb_part = 6000;
     std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsR(nb_part, prng, 30, 2*VEC3(ray, ray, ray));
     FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
     nb_part = points.size();
     VEC3 n(-1, 0, 0);
     //	    uint i = 0;
     for (auto &v: points) {
       VEC3 dir = (v-VEC3(ray, ray, ray));
       //dir.normalize();
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
     //	    uint i = 0;
     for (auto &v: points) {
       VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
       //dir.normalize();
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
       //dir.normalize();
       VEC3 vel = VEC3(0, speed*(v(2) - 0.5), 0);
       Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
       particules.push_back(p);
        p->setAnisotropyValues(1, 1, 1);
	p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
     }
     
}


void Simulation::addPurShearingSphereOfParticules(VEC3 center, FLOAT ray, FLOAT speed) { 
     PoissonGenerator::PRNG prng;
     uint nb_part = 10000;
     std::list<VEC3> points = PoissonGenerator::GeneratePoissonPointsC(nb_part, prng, 30);
     FLOAT volume = 4.0/3.0*M_PI*pow(ray, 3);
     nb_part = points.size();
     VEC3 n(-1, 0, 0);
     //	    uint i = 0;
     for (auto &v: points) {
       VEC3 dir = (v-VEC3(0.5, 0.5, 0.5));
       dir.normalize();
       VEC3 vel = VEC3(0, speed*(v(1) - 0.5), -speed*(v(2) - 0.5));
       Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
       particules.push_back(p);
        p->setAnisotropyValues(1, 1, 1);

	p->setAnisotropyRotation(utils::rotation(/*1.5708*/0.7, VEC3(1, 0, 0)));
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
       //dir.normalize();
       VEC3 vel = VEC3(0, speed, 0);
       Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*(v-VEC3(0.5, 0.5, 0.5)) + center, VEC3(0, 0, 1), vel);
       particules.push_back(p);
        p->setAnisotropyValues(1, 1, 0.1);
	p->setAnisotropyRotation(utils::rotation(1.5708, VEC3(1, 0, 0)));
     }
     
}


void Simulation::fix(VEC3 min, VEC3 max) {
  grid.fix(min, max);
}
