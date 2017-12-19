#ifndef NO_GRAPHICS_ 

#include "Scene.hpp"
#include "Cube.hpp"
#include "Sphere.hpp"
#include "CylinderObstacle.hpp"
#include "Cylinder.hpp"
#include "Simulation.hpp"
#include "error.hpp"
#include "Times.hpp"
#include "mpm_conf.hpp"
//#include "Skybox.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>

#define MAX_STRING_LEN 4
#include "common.h"


using namespace glm;

Scene* Scene::SCENE(new Scene("test", 2560, 1440));

void Scene::animateScene() {
  SCENE->animationLoop();
}

void Scene::saveScreenshot() {
  SCENE->saveScreenshotBMP();
}

Scene::Scene() {
}

Scene::Scene(std::string titreFenetre, int largeurFenetre, int hauteurFenetre) :
  m_titreFenetre(titreFenetre), m_largeurFenetre(largeurFenetre),
  m_hauteurFenetre(hauteurFenetre), m_fenetre(0), m_contexteOpenGL(0), m_input() {
}

Scene::~Scene() {
   std::list<Object*>::iterator it;
  for (it = l_objects.begin(); it != l_objects.end(); ++it) {
    delete (*it);
  }
   std::vector<Shader*>::iterator its;
  for (its = l_shaders.begin(); its != l_shaders.end(); ++its) {
    delete (*its);
  }
  std::vector<Texture*>::iterator itt;
  for (itt = l_textures.begin(); itt != l_textures.end(); ++itt) {
    delete (*itt);
  }
  l_objects.clear();
  l_shaders.clear();
  l_textures.clear();
  
    SDL_GL_DeleteContext(m_contexteOpenGL);
    SDL_DestroyWindow(m_fenetre);
    SDL_Quit();
}

bool Scene::initialiserFenetre() {
    // Initialisation de la SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Version d'OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Double Buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    std::cout<<"init buffer"<< std::endl;
    
     // Création de la fenêtre
     m_fenetre = SDL_CreateWindow(m_titreFenetre.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_largeurFenetre, m_hauteurFenetre, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
     m_renderer = SDL_CreateRenderer( m_fenetre, -1, SDL_RENDERER_ACCELERATED);

     if (m_fenetre == 0) {
         std::cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << std::endl;
         SDL_Quit();
         return false;
     }

     // Création du contexte OpenGL
     m_contexteOpenGL = SDL_GL_CreateContext(m_fenetre);
     if (m_contexteOpenGL == 0) {
       std::cout << SDL_GetError() <<"AAAAAAAAAAAAAAA"<< std::endl;
         SDL_DestroyWindow(m_fenetre);
         SDL_Quit();
         return false;
     }
    return true;
}

bool Scene::initGL() {
  srand (time(NULL));
#ifdef WIN32
  GLenum initialisationGLEW( glewInit() );
  if(initialisationGLEW != GLEW_OK) {
    std::cout << "Erreur d'initialisation de GLEW : " << glewGetErrorString(initialisationGLEW) << std::endl;
    SDL_GL_DeleteContext(m_contexteOpenGL);
    SDL_DestroyWindow(m_fenetre);
    SDL_Quit();
    return false;
  }
#endif
  glEnable (GL_BLEND);
  // glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_DEPTH_TEST );
  glDepthFunc(GL_LESS);

   
  /* Init TTF. */
  if (TTF_Init()) {
     fprintf(stderr, "error: cannot init ttf\n");
        exit(EXIT_FAILURE);
  }
    font = TTF_OpenFont("/home/cschreck/Documents/forest/poff/FreeSans.ttf", 60);
    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }

  return true;
}

void Scene::init() {
  Sphere::create_array();
  Cylinder::create_array();
  
  l_shaders = std::vector<Shader*>(4);
  l_shaders[0] = new Shader("shaders/simple.vert", "shaders/simple.frag");
  l_shaders[1] = new Shader("shaders/texture.vert", "shaders/texture.frag");
  l_shaders[2] = new Shader("shaders/point.vert", "shaders/point.frag");
  l_shaders[3] = new Shader("shaders/toon.vert", "shaders/toon.frag");

  l_textures = std::vector<Texture*>(6);
  l_textures[0] = new Texture("Textures/boulet.jpg");
  l_textures[0]->charger();
  l_textures[1] = new Texture("Textures/leaf.png");
  l_textures[1]->charger();
  l_textures[2] = new Texture("Textures/square.png");
  l_textures[2]->charger();
  l_textures[3] = new Texture("Textures/full_square.png");
  l_textures[3]->charger();
  l_textures[4] = new Texture("Textures/skybox4.png");
  l_textures[4]->charger();
  l_textures[5] = new Texture("Textures/skybox.png");
  l_textures[5]->charger();
    // // Camera matrix
    // glm::mat4 view = glm::lookAt(glm::vec3(-10, -10, 0), // Camera is at (4,3,3), in World Space
    // 				 glm::vec3(0, 0, 0), // and looks at the origin
    // 				 glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
  //);
  m_camera = CameraObject(vec3(-1, 0.5, 0.3), vec3(0.4, 0.4, 0.2), vec3(0, 0, 1), 0.005, 0.005);
    // m_input.afficherPointeur(false);
    // m_input.capturerPointeur(true);
    
    // // Model matrix : an identity matrix (model will be at the origin)
  //  m_model = glm::mat4(1.0f);
  m_projection = glm::perspective(glm::radians(45.0f), (float) m_largeurFenetre/ (float) m_hauteurFenetre, 0.1f, 100.0f);
 
    // // Our ModelViewProjection : multiplication of our 3 matrices
    // glm::mat4 mvp = projection * view * model; 
  m_frameRate = 1000 / 50;
  m_debutBoucle = 0;
  m_finBoucle = 0;
  m_tempsEcoule = 0;

  m_input_rate = 1000 / 10;
  m_input_loop0 = 0;
  m_input_loop1 = 0;
  m_input_loop_time = 0;

  running = false;
  step_by_step = 0;
  end_ = false;
  re_init = false;
  back = 0;

  t = 0;
  stop = 1000000;
  
  Times::TIMES->init();

  sim = new Simulation(0);
  l_objects.push_back(sim);

  skybox = new Skybox(10);
  skybox->setPosition(m_camera.getPosition());
  //l_objects.push_back(skybox);

  toggle_record = false;
  movie_path = "movie/im";
  nb_file_m = 0;
}

void Scene::animate() {
  m_camera.lookAt(m_view);
  m_vp = m_projection * m_view;
  skybox->setPosition(m_camera.getPosition());

  if (running || step_by_step > 0) {
    if (toggle_record) {
      std::thread thr(saveScreenshot);
    t += mpm_conf::replay_speed_;
    std::list<Object*>::iterator it;
    for (it = l_objects.begin(); it != l_objects.end(); ++it) {
      (*it)->animate();
    }
    thr.join();
    } else {
      t += mpm_conf::replay_speed_;
    std::list<Object*>::iterator it;
    for (it = l_objects.begin(); it != l_objects.end(); ++it) {
      (*it)->animate();
    }
    }
  }
  if (step_by_step > 0) {
    --step_by_step;
  }
}

void Scene::draw() {
   SDL_Color color;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Renderer *renderer;

  /* initialize variables. */
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = COMMON_COLOR_MAX;
     char text[MAX_STRING_LEN];
 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   std::list<Object*>::iterator it;

   // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
   //   SDL_RenderClear(renderer);
   //   render_text(renderer, 0, 0,               "hello", font, &rect, &color);
   //   render_text(renderer, 0, rect.y + rect.h, "world", font, &rect, &color);
   //   snprintf(text, MAX_STRING_LEN, "%u", (unsigned int)(time(NULL) % 1000));
   //   render_text(renderer, 0, rect.y + rect.h, text, font, &rect, &color);
   //   SDL_RenderPresent(renderer);

     //     common_fps_update_and_print();
   
  for (it = l_objects.begin(); it != l_objects.end(); ++it) {
      (*it)->draw();
  }
  //skybox->draw();
}


void Scene::animationLoop() {
  while(!end_) {
    //INFO(3,"input");
    m_input_loop0 = SDL_GetTicks();
    m_input.updateEvenements();

    if(m_input.getTouche(SDL_SCANCODE_ESCAPE) || m_input.terminer()) {
	end_ = true;
	break;
    }
    if(m_input.getTouche(SDL_SCANCODE_RETURN)) {
      if (running) {
	INFO(1, "STOP ANIMATION");
      } else {
	INFO(1, "RUN ANIMATION");
      }
      running = !running;
    }
    if(m_input.getTouche(SDL_SCANCODE_SPACE)) {
      // --back;
      ++step_by_step;
    }
    if(m_input.getTouche(SDL_SCANCODE_BACKSPACE)) {
      re_init = true;
    }
    if(m_input.getTouche(SDL_SCANCODE_B) ) {
      ++step_by_step;
      ++back;
    }
    if(m_input.getTouche(SDL_SCANCODE_C) ) {
      printCameraPos();
    }
    if(m_input.getTouche(SDL_SCANCODE_P) ) {
      saveScreenshotBMP("test_screenshot.bmp");
    }
    if(m_input.getTouche(SDL_SCANCODE_R) ) {
      toggle_record = !toggle_record;
      if (toggle_record) {
	INFO(1, "Recording "<<movie_path);
      } else {
	INFO(1, "Stop Recording ");
      }
    }
    if(m_input.getTouche(SDL_SCANCODE_E) ) {
      sim->exportMitsuba("test_mistuba.xml");
    }
    if(m_input.getTouche(SDL_SCANCODE_S) ) {
          mpm_conf::display_sphere_ = !mpm_conf::display_sphere_;
	   if (mpm_conf::display_sphere_) {
	     INFO(1, "Display sphere");
	  //   QString text = QString("Display sphere");
	  //   displayMessage(text);
	  } else {
	      INFO(1, "Do not display sphere");
	  //   QString text = QString("Do not display sphere");
	  //   displayMessage(text);
	   }
    }
     if(m_input.getTouche(SDL_SCANCODE_KP_PLUS) ) {
       if (mpm_conf::replay_speed_ >= 1) {
	 mpm_conf::replay_speed_ *= 2;
       } else if (mpm_conf::replay_speed_ < -1) {
	 mpm_conf::replay_speed_ /= 2;
       } else {
	 assert( mpm_conf::replay_speed_ == -1);
	 mpm_conf::replay_speed_ = 1;
	 INFO(1, "Speed x"<<mpm_conf::replay_speed_);
	 // QString text = QString("Speed x");
	 // text += QString::number(mpm_conf::replay_speed_);
	 // displayMessage(text); 
       }
    }
     if(m_input.getTouche(SDL_SCANCODE_KP_MINUS) ) {
         if (mpm_conf::replay_speed_ <= -1) {
	 mpm_conf::replay_speed_ *= 2;
       } else if (mpm_conf::replay_speed_ > 1) {
	 mpm_conf::replay_speed_ /= 2;
       } else {
	 assert( mpm_conf::replay_speed_ == 1);
	 mpm_conf::replay_speed_ = -1;
       }
	 INFO(1, "Speed x"<<mpm_conf::replay_speed_);
	 // QString text = QString("Speed x");
	 // text += QString::number(mpm_conf::replay_speed_);
	 // displayMessage(text); 
    }
    m_camera.deplacer(m_input);


    m_input_loop1 = SDL_GetTicks();
    m_input_loop_time = m_input_loop1 - m_input_loop0;
    if(m_input_loop_time < m_input_rate) {
      SDL_Delay(m_input_rate - m_input_loop_time);
    }
  }
}

void Scene::bouclePrincipale() {
  std::thread t1(animateScene);
  sim->init();
  FLOAT spacing =  mpm_conf::grid_spacing_;
  
  while(!end_) {
    
    m_debutBoucle = SDL_GetTicks();
    if (re_init) {
      sim->clear();
      sim->init();
      Times::TIMES->init();
      re_init = false;
    }
    if (back) {
      //      INFO(3, "back "<<back);
      sim->backward(back+mpm_conf::replay_speed_);
      back = 0;
    }
    Times::TIMES->tick(Times::total_time_);
    animate();
    draw();
    
    if (t > stop) {
      end_ = true;
    }

    Times::TIMES->tock(Times::total_time_);
     if (running) {
       INFO(2, "Times : simu "<<Times::TIMES->getTime(Times::simu_time_)
     	   <<"   display "<<Times::TIMES->getTime(Times::display_time_)
     	   <<"   total "<<Times::TIMES->getTime(Times::total_time_));
       Times::TIMES->next_loop();
       // if (toggle_record) {
       // 	 saveScreenshotBMP();
       // }
     }
     SDL_GL_SwapWindow(m_fenetre);
     m_finBoucle = SDL_GetTicks();
     m_tempsEcoule = m_finBoucle - m_debutBoucle;
     if(m_tempsEcoule < m_frameRate) {
      SDL_Delay(m_frameRate - m_tempsEcoule);
     }

  }
 
  t1.join();
}

void Scene::getProjView(glm::mat4 &vp) {
  vp = m_vp;
}

Shader* Scene::getShader(uint i) {
  return l_shaders[i];
}

Texture* Scene::getTexture(uint i) {
  return l_textures[i];
}

glm::vec3 Scene::getCameraPosition() const {
  return m_camera.getPosition();
}

glm::vec3 Scene::getCameraOrientation() const {
  return m_camera.getOrientation();
}

void Scene::setCameraPosition(glm::vec3 pos) {
  return m_camera.setPosition(pos);
}

void Scene::setCameraOrientation(glm::vec3 pos) {
  return m_camera.setOrientation(pos);
}

void Scene::setLoad(std::string s) {
  sim->setLoad(s);
}

void Scene::setExport(std::string s) {
  sim->setExport(s);
}

void Scene::setImport(std::string s) {
  sim->setImport(s);
}

void Scene::setScene(std::string s) {
  sim->setScene(s);
}

void Scene::setExportGrid(std::string s) {
  sim->setExportGrid(s);
}

void Scene::setImportGrid(std::string s) {
  sim->setImportGrid(s);
}

void Scene::setExportMitsuba(std::string s) {
  sim->setExportMitsuba(s);
}

void Scene::setRun(bool run) {
  if (running) {
    INFO(1, "STOP ANIMATION");
  } else {
    INFO(1, "RUN ANIMATION");
  }
  running = run;
}

void Scene::setStop(uint t_end) {
  stop = t_end;
}

FLOAT Scene::getTime() {
  return t*mpm_conf::dt_;
}


void Scene::printCameraPos() {
  glm::vec3 pos = m_camera.getPosition(), target = pos+m_camera.getOrientation(), up = glm::vec3(0, 0, 1);

  INFO(1, "<camera>\n <position> "<<m_camera.getPosition()[0]<<" "<<m_camera.getPosition()[1]<<" "<<m_camera.getPosition()[2]<<
       "\n <orientation> "<<m_camera.getOrientation()[0]<<" "<<m_camera.getOrientation()[1]<<" "<<m_camera.getOrientation()[2]<<
       "\n</camera>");
  INFO(1, "Mistuba:\n"<<"<lookat target=\""<<target[0]<<", "<<target[1]<<", "<<target[2]<<"\" origin=\""<<pos[0]<<", "<<pos[1]<<", "<<pos[2]<<"\" up=\""<<up[0]<<", "<<up[1]<<", "<<up[2] <<"\"/>");
}


bool Scene::saveScreenshotBMP(std::string filepath) {
    SDL_Surface* saveSurface = NULL;
     // SDL_Surface* infoSurface = NULL;
     // infoSurface = SDL_GetWindowSurface(SDLWindow);
    // if (infoSurface == NULL) {
    //     std::cerr << "Failed to create info surface from window in saveScreenshotBMP(string), SDL_GetError() - " << SDL_GetError() << "\n";
    // } else {
    unsigned char * pixels = new (std::nothrow) unsigned char[m_largeurFenetre*m_hauteurFenetre*4];
    //[infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel];
        if (pixels == 0) {
            std::cerr << "Unable to allocate memory for screenshot pixel data buffer!\n";
            return false;
        } else {
	  if (SDL_RenderReadPixels(m_renderer, NULL, 0, pixels, 4* m_largeurFenetre)  != 0) {
	    //	     &infoSurface->clip_rect, infoSurface->format->format, pixels, infoSurface->w * infoSurface->format->BytesPerPixel) != 0) {
                std::cerr << "Failed to read pixel data from SDL_Renderer object. SDL_GetError() - " << SDL_GetError() << "\n";
                 pixels = NULL;
                 return false;
	  } else {
	  Uint32 rmask, gmask, bmask, amask;
	  rmask = 0x00ff0000;
	  gmask = 0x0000ff00;
	  bmask = 0x000000ff;
	  amask = 0xff000000;

	  saveSurface = SDL_CreateRGBSurfaceFrom(pixels, m_largeurFenetre, m_hauteurFenetre, 32, 4* m_largeurFenetre, rmask, gmask, bmask, amask);
	  //						       infoSurface->w, infoSurface->h, infoSurface->format->BitsPerPixel, infoSurface->w * infoSurface->format->BytesPerPixel, infoSurface->format->Rmask, infoSurface->format->Gmask, infoSurface->format->Bmask, infoSurface->format->Amask);
                if (saveSurface == NULL) {
                    std::cerr << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";
                    return false;
                }
                SDL_SaveBMP(saveSurface, filepath.c_str());
                SDL_FreeSurface(saveSurface);
                saveSurface = NULL;
	  }
            delete[] pixels;
        }
	//  SDL_FreeSurface(infoSurface);
        //infoSurface = NULL;
	// }
	INFO(1, "Screenshot "<<filepath);
    return true;
}


bool Scene::saveScreenshotBMP() {
   std::stringstream ss;
   if (nb_file_m < 10) {
     ss <<movie_path<<"000"<<nb_file_m<<".bmp";
   } else if (nb_file_m < 100) {
       ss <<movie_path<<"00"<<nb_file_m<<".bmp";
   } else if (nb_file_m < 1000) {
     ss <<movie_path<<"0"<<nb_file_m<<".bmp";
   } else {
     ss <<movie_path<<nb_file_m<<".bmp";
   }
     std::string str(ss.str());
     saveScreenshotBMP(str);
     nb_file_m++;
}


/*
- x, y: upper left corner of string
- rect output Width and height contain rendered dimensions.
*/
void render_text(
    SDL_Renderer *renderer,
    int x,
    int y,
    const char *text,
    TTF_Font *font,
    SDL_Rect *rect,
    SDL_Color *color
) {
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = TTF_RenderText_Solid(font, text, *color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect->x = x;
    rect->y = y;
    rect->w = surface->w;
    rect->h = surface->h;
    /* This is wasteful for textures that stay the same.
     * But makes things less stateful and easier to use.
     * Not going to code an atlas solution here... are we? */
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
}




#endif

