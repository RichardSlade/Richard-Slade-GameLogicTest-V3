#include <SFML/Window/Event.hpp>

#include "App/GameState.hpp"
#include "App/Controller.hpp"
#include "App/Params.hpp"

GameState::GameState(Controller& cntrl
  , sf::RenderWindow& window
  , std::string username)
  : mResetNumEnemy(cntrl.getParams().NumEnemy)
  , mNumEnemy(mResetNumEnemy)
  , mChangeOverTime(sf::seconds(4.f))
  , mLostGame(false)
  , mLevel(1)
  , mChangeOverCountdown(sf::Time::Zero)
  , mText(" ", cntrl.getFont(Controller::Fonts::Sansation), 32)
  , mScore(0)
  , mController(cntrl)
  , mWindow(window)
  , mWorldDim(cntrl.getParams().LevelBlockSize * cntrl.getParams().LevelBlockX)
  , mUsername(username)
  , mWorld(new World(*this
            , cntrl
            , window
            , "Debug"
            , mWorldDim
            , mResetNumEnemy))
  , mPausedScreen(mController
  , *this
  , mWindow)
  , mCurrentScreen(GameState::Screen::Game)
  , mNewScreen(mCurrentScreen)
  , mPaused(false)
{

}

void GameState::restartWorld()
{
  std::unique_ptr<World> oldWorld(std::move(mWorld));

  mNumEnemy = mResetNumEnemy;
  mLevel++;

  mWorld = std::unique_ptr<World>(new World(*this
    , mController
    , mWindow
    , ""
    , mWorldDim
    , mResetNumEnemy));
}

void GameState::quitGameState()
{
  mController.changeState();
}

void GameState::update(sf::Time dt)
{
  if (mChangeOverCountdown > sf::Time::Zero)
  {
    mChangeOverCountdown -= dt;

    mText.setColor(sf::Color(255, 255, 255, 255));
    mText.setPosition(mWorld->getFocusPoint());

    if (static_cast<int>(mChangeOverCountdown.asSeconds()) % 2 == 0)
    {
      mText.setColor(sf::Color(255, 255, 255, 0));
    }

    // If lost the game
    if (mChangeOverCountdown < sf::Time::Zero)
    {
      if (mLostGame)
        quitGameState();
      else
      {
        restartWorld();
      }
    }
  }
  else
  {
    if (mNewScreen != mCurrentScreen)
      mCurrentScreen = mNewScreen;

    switch (mCurrentScreen)
    {
    case GameState::Screen::Game:
    {
      if (!mPausedScreen.isPaused())
        mWorld->update(dt);
      else
        mPausedScreen.update(dt);

      break;
    }
    default: break;
    }
  }
}

void GameState::handleInput()
{
  if (mCurrentScreen == GameState::Screen::Game)
  {
    if (!mPausedScreen.isPaused())
    {
      if (mWorld)
        mWorld->handleInput();
    }
    else
      mPausedScreen.handleInput();
  }
}

void GameState::display()
{
  if (mWorld)
    mWorld->display();

  if (mChangeOverCountdown > sf::Time::Zero)
  {
    mWindow.draw(mText);
  }
  else if (mCurrentScreen == GameState::Screen::Game)
  {
    if (mPausedScreen.isPaused())
      mWindow.draw(mPausedScreen);
  }
}

void GameState::pause()
{
  mPausedScreen.setup(mWindow.getView());
}

void GameState::levelComplete()
{
  mText.setString("Level Complete\n\tScore:" + std::to_string(mScore));

  sf::FloatRect bounds = mText.getLocalBounds();
  mText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
  mChangeOverCountdown = mChangeOverTime;
}

void GameState::gameOver()
{
  mText.setString("Game Over\n\tScore:" + std::to_string(mScore));

  mLostGame = true;

  sf::FloatRect bounds = mText.getLocalBounds();
  mText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
  mChangeOverCountdown = mChangeOverTime;
}



