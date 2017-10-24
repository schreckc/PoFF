#TEMPLATE = app
TARGET   = poff
#CONFIG  += qt4 opengl glut warn_on release thread openmp
#QT *= xml opengl
QMAKE_CXXFLAGS += -g -O3 -std=c++11  -D__MODE_DEBUG=3 -D_REENTRANT -I/usr/local/include/SDL2 -DHAVE_OPENGLES3 -DHAVE_OPENGL -g -I../external/glfw-3.1.2/include/GLFW -fopenmp $(CFLAGS)

LIBS += -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2 -lGL -lSDL2_image -lgomp -fopenmp -lSDL2_ttf

#Dossier où les exécutables seront placés
DESTDIR = .
# Dossier où les fichiers objets seront placés
unix:OBJECTS_DIR = ./obj

HEADERS  = src/*hpp
HEADERS  += src/*h
SOURCES  = src/*.cpp


