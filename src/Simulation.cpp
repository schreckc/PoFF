#include "Simulation.hpp"
#include "Scene.hpp"
#include "error.hpp"
#include "PlaneObstacle.hpp"
#include "SphereObstacle.hpp"
#include "BoxObstacle.hpp"
#include "Times.hpp"
#include "mpm_conf.hpp"

Simulation::Simulation(Shader * shader) : Object(shader) {
  import_ = false;
  export_ = false;
  load_conf_ = false;

  // conf_file = "test.conf";
  // export_path = "object/test";
  // import_path = "object/test";

}

Simulation::~Simulation() {
  clear();
}

void Simulation::init() {
  nb_file_i = 0;
  nb_file_e = 0;
  grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, Scene::SCENE->getShader(2));
  if (!import_) { 
    if (load_conf_) {
      mpm_conf::loadConf(conf_file);
    }
    // grid = Grid(mpm_conf::size_grid_(0), mpm_conf::size_grid_(1), mpm_conf::size_grid_(2), mpm_conf::grid_spacing_, Scene::SCENE->getShader(2));
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
    for(uint i = 0; i < mpm_conf::nb_particules_; ++i) {
      FLOAT h = 0.4, w = 0.2, l = 0.2;
      FLOAT volume = h*l*w; 
      VEC3 r(0.4 + w*rand()/RAND_MAX, 0.4 + l*rand()/RAND_MAX, 0.21 + h*rand()/RAND_MAX);
      //  VEC3 r(1.2, 1.3, 1.4);
      Particule *p = new Particule(volume*mpm_conf::density_/(FLOAT)mpm_conf::nb_particules_, volume/(FLOAT)mpm_conf::nb_particules_, r);
      //p->setVelocity(VEC3(0.5, 0.5, 0.5));
      //  p->setVelocity(VEC3(1.0*rand()/RAND_MAX -0.5, 1.0*rand()/RAND_MAX-0.5, 1.0*rand()/RAND_MAX-0.5));
      particules.push_back(p);
    }
    grid.nextStep();
    grid.particulesToGrid(particules);
    grid.initVolumes(particules);

    //   PlaneObstacle *ob = new PlaneObstacle(0.1, VEC3(0, 0, 1));
      
     // PlaneObstacle *po = new PlaneObstacle(0.1, VEC3(0, 0, 1));
     // obstacles.push_back(po);

     //  BoxObstacle *box = new BoxObstacle(VEC3(0.02, 0.02, 0.02), VEC3(0.98, 0.98, 0.98));
     //  obstacles.push_back(box);

      SphereObstacle *ob = new SphereObstacle(VEC3(0.55, 0.55, 0.4), 0.3, 0.0, VEC3(0, 0, -1), Scene::SCENE->getShader(2)); 
      obstacles.push_back(ob);
      //          ob->setFriction(0);

    
    for (auto & o : obstacles) {
      grid.initCollision(o);
    }
    
    INFO(1, "Lame Parameters : lambda = "<<mpm_conf::lambda_<<"    mu = "<<mpm_conf::mu_<<"\n");
    INFO(1, "dt = "<<mpm_conf::dt_<<"\n");
    if (export_) {
      exportSim();
    }
  
  } else {
    importSim();
  }
  t = 0;
}

void Simulation::clear() {
  INFO(1, "Clear Simulation");
  for (auto& p : particules) {
    // INFO(3,"delete p"<<" "<<p);
    delete p;
  }
  particules.clear();
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
  uint i = 0;
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
      particules.push_back(p);
      // INFO(3, p->getPosition());
      } else if (line[0] == '#') {
	
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
}

void Simulation::importSim() {
  std::stringstream ss;
  ss <<import_path<<nb_file_i<<".obj";
  std::string str(ss.str());
  std::ifstream file(str.c_str());
  INFO(1, "Import file \""<<str<<"\"  >>"<<mpm_conf::replay_speed_);
  if (file.good()) {
    clear();
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
   std::stringstream ss;
  ss <<export_path<<nb_file_e<<".obj";
  std::string str(ss.str());
  std::ofstream file(str.c_str());
  ERROR(file.good(), "cannot open file \""<<str<<"\"", "");
  INFO(1, "Export file \""<<str<<"\"");
  exportParticules(file);
  ++nb_file_e;
  file.close();
  
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

void Simulation::backward(uint n) {
  if (nb_file_i >= 1) {
    nb_file_i -= 1;
  } else {
    nb_file_i = 0;
  }
}
