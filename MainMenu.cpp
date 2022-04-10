#include "Screen.h"
#include "MainMenu.h"
#include "Game.h"
#include <functional>
#include <iostream>

MainMenu::MainMenu(LWindow &window) : LScreen(window)
{
    window.loadTexture(buttonBg, "resources/ButtonBG.png");
    window.loadTexture(buttonBgHighlighted, "resources/ButtonBGHighlighted.png");
    window.loadTexture(buttonBgClicked, "resources/ButtonBGClicked.png");
    int w = buttonBg.getWidth();
    int h = buttonBg.getHeight();
    int x = (window.getWidth() - w) / 2;
    int y = (window.getHeight() - h) / 2;
    SDL_Color txtColor = {2, 10, 255, 255};
    TTF_Font *font = TTF_OpenFont("resources/FrostbiteBossFight-dL0Z.ttf", 28);

    Button *playButton = new Button(x, y, &buttonBg, window, "Play", font, txtColor);
    playButton->setCenterX(true);
    playButton->setHighlightTexture(buttonBgHighlighted);
    playButton->setClickTexture(buttonBgClicked);
    playButton->setOnClickListener([&](LWindow &window)
                                   {LGame* myGame = new LGame(window);
                                   window.setCurrScreen(myGame); });
    buttons.push_back(playButton);
}

void MainMenu::handleEvent(SDL_Event &e)
{
    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i]->handle(e);
    }
}

void MainMenu::update()
{
}

void MainMenu::render(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 3, 211, 252, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i]->render(renderer);
    }
}

void MainMenu::cleanUp()
{
    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i]->cleanUp();
        delete buttons[i];
    }
}