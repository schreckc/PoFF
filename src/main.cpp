#include "mpm_conf.hpp"
#include "error.hpp"


void help() {
  std::cout<<"\n     *** PILES OF STUFFS: Help ***\n"<<std::endl;
  std::cout<<"Synopsis: \n     .\\poff <options>\n\nOptions:"<<std::endl;
  std::cout<<"     -l, -load <file>: load configuration file"<<std::endl;
  std::cout<<"     -s, -scene <file>: load scene from file"<<std::endl;
  std::cout<<"     -e, -export <name>: export animation in a set of files <name><frame number>.obj"<<std::endl;
  std::cout<<"     -i, -import <name>: import animation from a set of files <name><frame number>.obj"<<std::endl;
  std::cout<<"     -stop <t>: stop animation and exit at time t"<<std::endl;
  std::cout<<"     -es, -export_step <n>: export every n frames"<<std::endl;
  std::cout<<"     -r, -run: run directly the animation"<<std::endl;
  std::cout<<"     -h, -help: print help\n"<<std::endl;
  exit(0);
}

#ifndef NO_GRAPHICS_

#include "Scene.hpp"

int main(int argc, char **argv) {
	
  //  Scene scene("test", 2400, 1800);
  if(Scene::SCENE->initialiserFenetre() == false)
    return -1;  
  Scene::SCENE->init();

  for (int i = 1;  i < argc; ++i) {
    std::string s(argv[i]);
    //    INFO(3, s);
    if (s == "-l" || s == "-load") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Loading configuration file:"<<" "<<argv[i+1]<<std::endl;
      Scene::SCENE->setLoad(argv[i+1]);
      ++i;
    } else if (s == "-i" || s == "-import") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Importing"<<" "<<argv[i+1]<<std::endl;
      Scene::SCENE->setImport(argv[i+1]);
      ++i;
    } else if (s == "-e") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Exporting"<<" "<<argv[i+1]<<std::endl;
      Scene::SCENE->setExport(argv[i+1]);
      ++i;
    } else if (s == "-s" || s == "-scene") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Loading scene"<<" "<<argv[i+1]<<std::endl;
      Scene::SCENE->setScene(argv[i+1]);
      ++i;
    } else if (s == "-r" || s == "-run") {
      Scene::SCENE->setRun(true);
    } else if (s == "-stop") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Stop at t = "<<argv[i+1]<<std::endl;
      Scene::SCENE->setStop(atoi(argv[i+1]));
      ++i;
    } else if (s == "-export_step" || s == "-es") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Export every "<<argv[i+1]<<" steps"<<std::endl;
      mpm_conf::export_step_ = atoi(argv[i+1]);
      ++i;
    } else if (s == "-h" || s == "-help") {
      help();
    } else {
      std::cerr<<"\nERROR: Unknown option\n"<<std::endl;
      help();
    }
  }
  
  if(Scene::SCENE->initGL() == false)
    return -1;
  
  Scene::SCENE->bouclePrincipale();
  return 0;
}

#else

#include "Simulation.hpp"
#include "Times.hpp"
#include "mpm_conf.hpp"

int main(int argc, char **argv) {
  INFO(3, "NO GRAPHICS");
  Simulation* sim = new Simulation(-1);
  bool end_ = false;
  
  uint t = 0;
  uint stop = 1000000;
  
  for (int i = 1;  i < argc; ++i) {
    std::string s(argv[i]);
    //    INFO(3, s);
    if (s == "-l" || s == "-load") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Loading configuration file:"<<" "<<argv[i+1]<<std::endl;
      sim->setLoad(argv[i+1]);
      ++i;
    } else if (s == "-i" || s == "-import") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Importing"<<" "<<argv[i+1]<<std::endl;
      sim->setImport(argv[i+1]);
      ++i;
    } else if (s == "-e") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Exporting"<<" "<<argv[i+1]<<std::endl;
      sim->setExport(argv[i+1]);
      ++i;
    } else if (s == "-s" || s == "-scene") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Loading scene"<<" "<<argv[i+1]<<std::endl;
      sim->setScene(argv[i+1]);
      ++i;
    } else if (s == "-stop") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Stop at t = "<<argv[i+1]<<std::endl;
      stop = atoi(argv[i+1]);
      ++i;
    } else if (s == "-export_step" || s == "-es") {
      if (argc < i + 2) {
	std::cerr<<"\nERROR: wrong number of arguments\n"<<std::endl;
	help();
      }
      std::cout<<"Export every "<<argv[i+1]<<" steps"<<std::endl;
      mpm_conf::export_step_ = atoi(argv[i+1]);
      ++i;
    } else if (s == "-h" || s == "-help") {
      help();
    } else {
      std::cerr<<"\nERROR: Unknown option\n"<<std::endl;
      help();
    }
  }

  sim->init();

  //animation loop
  while(!end_) {
    Times::TIMES->tick(Times::total_time_);

    ++t;
    sim->animate();
    if (t > stop) {
      end_ = true;
    }
    if (t%100==0) {
      INFO(1, "Simulation step : "<<t);
      INFO(2, "Time per frame "<<Times::TIMES->getTime(Times::total_time_));
    }
    
    Times::TIMES->tock(Times::total_time_);
    Times::TIMES->next_loop();
    // if (t%100==0) {
    //   std::cout<<".";
    // }

  }

  
  return 0;
}

#endif
