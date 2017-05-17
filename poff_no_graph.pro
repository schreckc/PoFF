#TEMPLATE = app
TARGET   = poff_no_graph
#CONFIG  += warn_on release openmp
#QT *= xml opengl
QMAKE_CXXFLAGS += -g -O2 -std=c++11  -D__MODE_DEBUG=3 -D_REENTRANT -fopenmp -DNO_GRAPHICS_

LIBS += -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lgomp -fopenmp

#Dossier où les exécutables seront placés
DESTDIR = .
# Dossier où les fichiers objets seront placés
unix:OBJECTS_DIR = ./obj

HEADERS  = src/*hpp
HEADERS  += src/*h
SOURCES  = src/*.cpp


