#CC specifies which compiler we're using
CXX = g++

#OBJS specifies which files to compile as part of the project
OBJS = Button.o collision.o Game.o main.o MainMenu.o MessageStructs.o MyTexture.o MyWindow.o Player.o Screen.o Text.o TextInput.o Tile.o TileAtlas.o Timer.o SoundEffect.o Entity.o Animation.o NPC.o # SearchOpponent.o

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = game.out

#This is the target that compiles our executable
all : Client Server

Client: $(OBJS)
	$(CXX) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

Button.o: MyWindow.h

collision.o: 

Game.o: Tile.h Player.h MyTexture.h TileAtlas.h TileAtlas.h Renderable.h Entity.h Text.h Timer.h Screen.h constants.h MyWindow.h NPC.o

main.o: MyWindow.h Game.h MainMenu.h

MainMenu.o: Screen.h Game.h TextInput.h MyWindow.h Button.h

MessageStructs.o: MessageStructs.h

MyTexture.o: 

MyWindow.o: MyTexture.h Text.h Renderable.h constants.h Screen.h

Player.o:	Tile.h MyTexture.h Renderable.h MyWindow.h SoundEffect.h Timer.h utilities.h Animation.h constants.h Game.h

Screen.o: MyWindow.h 

Text.o: MyTexture.h MyWindow.h

TextInput.o: MyWindow.h 

Tile.o: MyTexture.h Renderable.h collision.h TileAtlas.h Game.h

TileAtlas.o: 

Timer.o: 

SoundEffect.o: 

Entity.o: Player.h 

Animation.o: Renderable.h MyTexture.h

NPC.o: Renderable.h Game.h MyTexture.h Timer.h

Server: server/server.cpp MessageStructs.cpp
	$(CXX) server/server.cpp MessageStructs.cpp $(COMPILER_FLAGS) -o server/server.out

clean:
	rm *.o server/server.out $(OBJ_NAME) 