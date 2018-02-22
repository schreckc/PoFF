#ifndef NO_GRAPHICS_ 

#ifndef SCENE_HPP
#define SCENE_HPP

#ifdef WIN32
#include <GL/glew.h>

#else
#define GL3_PROTOTYPES 1
#include <GLES3/gl3.h>

#endif

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "Input.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "CameraObject.hpp"
#include "Object.hpp"
#include "Simulation.hpp"
#include "Skybox.hpp"

/** to implement **/
//#include <SDL2/SDL_ttf.h>

class Scene {
public:
  static Scene* SCENE;
  static void animateScene();
  static void saveScreenshot();
  
  Scene();
  Scene(std::string titreFenetre, int largeurFenetre, int hauteurFenetre);
  ~Scene();

  bool initialiserFenetre();
  bool initGL();
  void animationLoop();
  void bouclePrincipale();

  void init();
  void animate();
  void draw();

  void getProjView(glm::mat4 &vp);
  Shader* getShader(uint i);
  Texture* getTexture(uint i);
  glm::vec3 getCameraPosition() const;
  glm::vec3 getCameraOrientation() const;
  void setCameraPosition(glm::vec3 pos);
  void setCameraOrientation(glm::vec3 pos);

  void setLoad(std::string);
  void setScene(std::string);

  /** import ot export postition and orientation of the particles (or subparticles) **/
  void setExport(std::string);
  void setImport(std::string);
  
  /** export or import the velocities of the grid **/
  void setExportGrid(std::string);
  void setImportGrid(std::string);

  /** export xml file to be render with mitsuba */
  void setExportMitsuba(std::string);

  
  void setRun(bool run);
  void setStop(uint t_end);

  FLOAT getTime();
  void printCameraPos();
  bool running;

  bool saveScreenshotBMP(std::string filepath);
  bool saveScreenshotBMP();
private:
  uint step_by_step;
  uint time_;
  bool end_;
  bool re_init;
  int back;
  uint stop;

  /** to record viewer into bmp files **/
  bool toggle_record;
  uint nb_file_m;
  std::string movie_path;

  std::string m_titreFenetre; // window title
  int m_largeurFenetre; // window width
  int m_hauteurFenetre; // window height

  SDL_Window* m_fenetre;
  SDL_GLContext m_contexteOpenGL;
  SDL_Renderer* m_renderer;

  /*** to implement */
  //  TTF_Font *font;

  Input m_input;
  
  CameraObject m_camera;
  glm::mat4 m_projection;
  glm::mat4 m_view;
  glm::mat4 m_vp;

  std::vector<Shader*> l_shaders;
  std::vector<Texture*> l_textures;

  std::list<Object*> l_objects;
  Simulation *sim;

  unsigned int m_frameRate;
  Uint32 m_debutBoucle, m_finBoucle, m_tempsEcoule;
  unsigned int m_input_rate;
  Uint32 m_input_loop0, m_input_loop1, m_input_loop_time;

  Skybox *skybox;
};


/*** not implemented ***/
// void render_text(SDL_Renderer *renderer,
//     int x,
//     int y,
//     const char *text,
//     TTF_Font *font,
//     SDL_Rect *rect,
//     SDL_Color *color);


#endif

#endif
