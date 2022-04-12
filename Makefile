#CC specifies which compiler we're using
CXX = g++

#OBJS specifies which files to compile as part of the project
OBJS = Button.cpp collision.cpp Game.cpp main.cpp MainMenu.cpp MyTexture.cpp MyWindow.cpp Player.cpp Screen.cpp Text.cpp Tile.cpp TileAtlas.cpp Timer.cpp SoundEffect.cpp

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = experiment.out

#This is the target that compiles our executable
all : $(OBJS)
	$(CXX) -I/usr/include/SDL2 $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)