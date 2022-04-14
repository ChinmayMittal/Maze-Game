#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "Screen.h"
#include "MyWindow.h"
#include "Button.h"
#include "TextInput.h"

class MainMenu : public LScreen
{
private:
    std::vector<Button *> buttons;
    std::vector<TextInput *> textInputs;
    LTexture buttonBg;
    LTexture buttonBgHighlighted;
    LTexture buttonBgClicked;

public:
    MainMenu(LWindow &window);
    void handleEvent(SDL_Event &e);
    void update();
    void render(SDL_Renderer *renderer);
    void cleanUp();
    std::string getName();
};

#endif