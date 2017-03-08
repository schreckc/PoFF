#include "Scene.hpp"

void help() {
  std::cout<<"help: "<<std::endl;
  exit(0);
}

int main(int argc, char **argv) {
	
  //  Scene scene("test", 2400, 1800);
	
  if(Scene::SCENE->initialiserFenetre() == false)
	return -1;
	
  if(Scene::SCENE->initGL() == false)
	return -1;
  
  Scene::SCENE->init();

  for (int i = 1;  i < argc; ++i) {
    std::string s(argv[i]);
    if (s == "-l") {
      if (argc < i + 2) {
	help();
      }
      std::cout<<"load"<<" "<<argv[i+1]<<std::endl;
      Scene::SCENE->setLoad(argv[i+1]);
      ++i;
    } else if (s == "-i") {
      if (argc < i + 2) {
	help();
      }
      std::cout<<"import"<<" "<<argv[i+1]<<std::endl;
       Scene::SCENE->setImport(argv[i+1]);
      ++i;
    } else if (s == "-e") {
      if (argc < i + 2) {
	help();
      }
      std::cout<<"export"<<" "<<argv[i+1]<<std::endl;
      Scene::SCENE->setExport(argv[i+1]);
      ++i;
    } else if (s == "-r") {
      Scene::SCENE->setRun(true);
    }
  }
  Scene::SCENE->bouclePrincipale();
  return 0;
}
