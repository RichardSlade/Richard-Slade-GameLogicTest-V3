/*
*   Controller.cpp
*
*   @Author Richard Slade
*   @Date 12/2014
*/

#include <iostream>
#include <string>
#include <cassert>

#include <SFML/Graphics/RenderTexture.hpp>

#include "App/Controller.hpp"
#include "App/MenuState.hpp"
#include "App/GameState.hpp"

// Const FPS for frame cap
const sf::Time Controller::mFPS = sf::seconds(1.f / 60.f);

Controller::Controller()
  : mParams()
  , mWindowX(mParams.WindowX)
  , mWindowY(mParams.WindowY)
  , mWindow(sf::VideoMode(mWindowX, mWindowY), "AI Steering behaviours")
  , mResetViewCenter(mWindow.getView().getCenter())
  , mStatisticsText()
  , mStatisticsUpdateTime()
  , mStatisticsNumFrames(0)
  , mAppStateType(AppState::StateType::Menu)
  , mChangeState(false)
{
  mWindow.setPosition(sf::Vector2i(0, 0));
  loadMedia();

  mStatisticsText.setFont(getFont(Controller::Fonts::Sansation));
  mStatisticsText.setCharacterSize(16);
  mStatisticsText.setString("0.000");

  // Initialise menu state
  mCurrentAppState = std::unique_ptr<MenuState>(new MenuState(*this,
    mWindow));
};

/*
*   Load Textures and fonts from files
*   store in appropriate containers in Controller
*/
void Controller::loadMedia()
{
  for (unsigned int i = 0; i < Textures::NumTextures; i++)
    mTextures.push_back(Controller::upTexture());

  std::vector<std::string> fileNames;
  fileNames.push_back("media/textures/adventurer.png");
  fileNames.push_back("media/textures/enemy.png");
  fileNames.push_back("media/textures/brickLarge.png");
  fileNames.push_back("media/textures/barrel.png");
  fileNames.push_back("media/textures/background.png");

  assert(mTextures.size() >= fileNames.size());

  std::vector<Controller::upTexture>::iterator texIter = mTextures.begin();
  std::vector<std::string>::iterator fileNameIter;

  unsigned int index = 0;

  for (fileNameIter = fileNames.begin();
    fileNameIter != fileNames.end();
    fileNameIter++)
  {
    sf::Texture t;

    if (!t.loadFromFile(*fileNameIter))
      throw std::runtime_error("Failed to load file: " + *fileNameIter);

    if (index == Controller::Textures::Brick)
      t.setRepeated(true);

    (*texIter).reset(new sf::Texture(t));
    texIter++;

    index++;

  }

  // Fonts
  for (unsigned int i = 0; i < Fonts::NumFonts; i++)
    mFonts.push_back(Controller::upFont());

  std::vector<Controller::upFont>::iterator fontIter = mFonts.begin();

  fileNames.clear();

  fileNames.push_back("media/fonts/Sansation.ttf");

  for (fileNameIter = fileNames.begin();
    fileNameIter != fileNames.end();
    fileNameIter++)
  {
    sf::Font f;

    if (!f.loadFromFile(*fileNameIter))
      throw std::runtime_error("Failed to load file: " + (*fileNameIter));

    (*fontIter).reset(new sf::Font(f));
    fontIter++;
  }
}

/*
*   Change App state from menu -> game or vice versa
*/
void Controller::changeAppState()
{
  resetView();

  switch (mAppStateType)
  {
  case AppState::StateType::Menu:
  {
    mCurrentAppState = std::unique_ptr<GameState>(new GameState(*this
      , mWindow
      , mUserName));
    mAppStateType = AppState::StateType::Game;
    break;

  }
  case AppState::StateType::Game:
  {
    mCurrentAppState = std::unique_ptr<MenuState>(new MenuState(*this
      , mWindow));
    mAppStateType = AppState::StateType::Menu;
    break;
  }
  default: break;
  }

  mChangeState = false;
}

void Controller::updateStatistics(sf::Time dt)
{
  mStatisticsUpdateTime += dt;
  mStatisticsNumFrames += 1;

  if (mStatisticsUpdateTime >= sf::seconds(1.0f))
  {
    mStatisticsText.setString(
      "Frames / Second = " + std::to_string(mStatisticsNumFrames) + "\n" +
      "Time / Update = " + std::to_string(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

    mStatisticsUpdateTime -= sf::seconds(1.0f);
    mStatisticsNumFrames = 0;
  }
}

/*
*   Application main loop
*/
void Controller::run()
{
  sf::Time timeSinceLastUpdate = sf::Time::Zero;

  while (mWindow.isOpen())
  {
    sf::Time dt = mClock.restart();
    timeSinceLastUpdate += dt;

    while (timeSinceLastUpdate > mFPS)
    {
      timeSinceLastUpdate -= mFPS;

      mCurrentAppState->handleInput();
      mCurrentAppState->update(mFPS);
    }

    updateStatistics(dt);

    mWindow.clear();
    mCurrentAppState->display();

    sf::View view = mWindow.getView();
    mStatisticsText.setPosition(view.getCenter().x - (view.getSize().x / 2),
      view.getCenter().y - (view.getSize().y / 2));
    mWindow.draw(mStatisticsText);

    mWindow.display();

    if (mChangeState)
      changeAppState();
  }
}

/*
*   Ensure view is always at centre when changing states
*/
void Controller::resetView()
{
  sf::View view = mWindow.getView();
  view.setCenter(mResetViewCenter);
  mWindow.setView(view);
}

