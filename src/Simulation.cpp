#include "Simulation.hpp"
#include "Scene.hpp"
#include "error.hpp"
#include "PlaneObstacle.hpp"
#include "SphereObstacle.hpp"
#include "CylinderObstacle.hpp"
#include "BoxObstacle.hpp"
#include "BallObstacle.hpp"
#include "Times.hpp"
#include "mpm_conf.hpp"

#define POISSON_PROGRESS_INDICATOR 1
#include "PoissonGenerator.hpp"

Simulation::Simulation(Shader * shader) : Object(shader) {
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
  //  grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, Scene::SCENE->getShader(2));
  if (!import_) {
    if (load_conf_) {
      mpm_conf::loadConf(conf_file);
    }
       //grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, Scene::SCENE->getShader(2));
    // for(uint i = 0; i < mpm_conf::nb_particules_; ++i) {
    //   float h = 0.1, w = 0.1, l = 0.1;
    //   FLOAT volume = h*l*w; 
    //   VEC3 r(0.35 + w*rand()/RAND_MAX, 0.35 + l*rand()/RAND_MAX, 0.65 + h*rand()/RAND_MAX);
    //   //  VEC3 r(1.2, 1.3, 1.4);
    //   Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)mpm_conf::nb_particules_, volume/(FLOAT)mpm_conf::nb_particules_, r);
    //   //p->setVelocity(VEC3(0.5, 0.5, 0.5));
    //   //  p->setVelocity(VEC3(1.0*rand()/RAND_MAX -0.5, 1.0*rand()/RAND_MAX-0.5, 1.0*rand()/RAND_MAX-0.5));
    //   particules.push_back(p);
    // }
    // grid.nextStep();
    // grid.particulesToGrid(particules);
    // grid.initVolumes(particules);

    // //   PlaneObstacle *ob = new PlaneObstacle(0.1, VEC3(0, 0, 1));
      
    // PlaneObstacle *po = new PlaneObstacle(0.1, VEC3(0, 0, 1));
    // obstacles.push_back(po);

     // SphereObstacle *ob = new SphereObstacle(VEC3(0.5, 0.5, 0.8), 0.25, 0.2, VEC3(0, 0, -1), Scene::SCENE->getShader(2)); 
     // obstacles.push_back(ob);
    // ob->setFriction(0);

         
    grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, Scene::SCENE->getShader(2));
    loadScene();
    // grid.nextStep();
    // grid.particulesToGrid(particules);
    grid.init(particules);

    for (auto & o : obstacles) {
      grid.initCollision(o);
    }
    
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
  INFO(1, "Simulation step : "<<t);
  if (!import_) {
    oneStep();
    if (export_) {
      exportSim();
    }
  } else {
    importSim();
    INFO(3, particules.front()->getVolume());
  }
  for (auto &ob : obstacles) {
    ob->animate();
  }
}

void Simulation::draw(glm::mat4 m, Shader *s) {
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
  Shader *cur_shader = m_shader;
  if (m_shader == NULL) {
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

void Simulation::oneStep() {
  Times::TIMES->tick(Times::simu_time_);
  grid.nextStep();
  grid.particulesToGrid(particules);
  if (mpm_conf::smooth_vel_) {
    for (uint i = 0; i < 1; ++i) {
      grid.smoothVelocity();
    }
    INFO(3, "SMOOTH");
  }
  for (auto & ob : obstacles) {
     //INFO(3, "collision "<<obstacles.size() );
    grid.collision(ob);
   }
   

  grid.gridToParticules(particules);

   for (auto & ob : obstacles) {
     grid.initCollision(ob); //set for each node previous distance to the closest obstacle
   }
   Times::TIMES->tock(Times::simu_time_);
}

void Simulation::importParticules(std::ifstream & file) {
  particules.clear();
  std::string line;
  uint ir = 0, ia = 0;
  while (getline(file, line)) {
     // if (i == 10) {
    //   i = 0;
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
	particules[ir]->setAnisotropyAxes(VEC3(r(0), r(1), r(2)), VEC3(r(3), r(4), r(5)), VEC3(r(6), r(7), r(8)));
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
    } // else {
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

void Simulation::loadScene() {
    std::ifstream file(scene_path.c_str());
      std::string line;
      INFO(3, "load SCENE");
    while (getline(file, line)) {
      
      if (line.substr(0,11) == "<obstacles>") {
	getline(file, line);	
	while (line.substr(0,12) != "</obstacles>") {
	  // INFO(3, "Obsctacle "<<line);
	  if (line.substr(0,9) == " <sphere>") {
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
	    //INFO(3, "obstacles = "<<min<<"\n"<<max);
	  // end box

	  }  else if (line.substr(0,8) == " <plane>") {
	    FLOAT pos = 0;
	    VEC3 n(0, 0, 0);
	    FLOAT fric = mpm_conf::friction_coef_;
	    getline(file, line);
	    while (line.substr(0,9) != " </plane>") {
	      if (line.substr(0,7) == "  <pos>") {
		std::istringstream s(line.substr(7));
		s >> pos;
	      } else if (line.substr(0,10) == "  <normal>") {
		std::istringstream s(line.substr(10));
		for (uint i = 0; i < 3; ++i) {
		  s >> n(i);
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
	    PlaneObstacle *o = new PlaneObstacle(pos, n);
	    obstacles.push_back(o);
	    o->setFriction(fric);
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
	    CylinderObstacle *o = new CylinderObstacle(pos, dir, r, Scene::SCENE->getShader(0));
	    obstacles.push_back(o);
	    o->setFriction(fric);
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
	    BallObstacle *o = new BallObstacle(pos, r, Scene::SCENE->getShader(0));
	    obstacles.push_back(o);
	    o->setFriction(fric);
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
	getline(file, line);	
	while (line.substr(0,13) != "</particules>") {
	  if (!import_) {
	    if (line.substr(0,9) == " <cuboid>") {
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
	    for (auto &v: points) {
	      Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, v + VEC3(xmin, ymin, zmin), VEC3(0, 0, 1), vel);
	       particules.push_back(p);

	       // VEC3 x((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       // VEC3 y((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       // VEC3 z((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       VEC3 x(1, 0, 0);
	       VEC3 y(0, 1, 0);
	       VEC3 z(0, 0, 1);
	      
	       p->setAnisotropyAxes(x, y, z);
	       p->setAnisotropyValues(0.5, 0.5, 2);
	      
	     }
	    
	    //cuboid
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
	    for (auto &v: points) {
	      Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)nb_part, volume/(FLOAT)nb_part, ray*v + center, VEC3(0, 0, 1), vel);
	       particules.push_back(p);

	       // VEC3 x((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       // VEC3 y((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       // VEC3 z((FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX, (FLOAT)rand()/(FLOAT)RAND_MAX);
	       VEC3 x(1, 0, 0);
	       VEC3 y(0, 1, 0);
	       VEC3 z(0, 0, 1);
	      
	       p->setAnisotropyAxes(x, y, z);
	       p->setAnisotropyValues(0.5, 0.5, 2);
	      
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
      } else {
	std::cerr<<"Line not recognized in file \""<<scene_path<<"\": "<<line<<std::endl;
	exit(-1);
      }
      
    } //end main loop
    
} // end loadScene


// void Simulation::saveState(std::string save_file) {
//     std::ofstream file(save_file);
//     ERROR(file.good(), "cannot open file \""<<save_file<<"\"", "");
//     INFO(1, "Save simulation state in file \""<<str<<"\"");
//     exportParticules(file);
//     file.close();
//  }
