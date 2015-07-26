#include <SFML/Window/Event.hpp>

#include "App/GameState.hpp"
#include "App/Controller.hpp"
#include "App/Params.hpp"

GameState::GameState(Controller& cntrl
                     , sf::RenderWindow& window
                     , std::string username)
: mResetNumEnemy(cntrl.getParams().NumEnemy)
//, mLevelTimeMin(cntrl.getParams().LevelTimeMin)
//, mResetLevelTime(cntrl.getParams().LevelTimeMax)
, mNumEnemy(mResetNumEnemy)
, mTotalEnemiesTrapped(0)
, mController(cntrl)
, mWindow(window)
, mWorldDim(cntrl.getParams().LevelBlockSize * cntrl.getParams().LevelBlockX)
//, mLevelTime(sf::seconds(mResetLevelTime * 60.f))

, mUsername(username)
, mWorld(new World(*this
                   , cntrl
                   , window
                   , "Debug"
                   , mWorldDim
                   , mNumEnemy))
, mPausedScreen(mController
                , *this
                , mWindow)
, mLevelCompleteScreen(mController
                     , *this
                     , mWindow)
, mGameCompleteScreen(mController
                   , *this
                   , mWindow)
, mCurrentScreen(GameState::Screen::Game)
, mNewScreen(mCurrentScreen)
, mPaused(false)
{

}

void GameState::restartWorld()
{
    mWorld = std::unique_ptr<World>(new World(*this
                                              , mController
                                              , mWindow
                                              , mUsername
                                              , mWorldDim
                                              , mResetNumEnemy));

    mNewScreen = GameState::Screen::Game;
}

void GameState::update(sf::Time dt)
{
    if(mNewScreen != mCurrentScreen)
        mCurrentScreen = mNewScreen;

    switch(mCurrentScreen)
    {
        case GameState::Screen::Game:
        {
            if(!mPausedScreen.isPaused())
                mWorld->update(dt);
            else
                mPausedScreen.update(dt);

            break;
        }
        case GameState::Screen::LevelComplete: mLevelCompleteScreen.update(dt); break;
        case GameState::Screen::GameComplete: mGameCompleteScreen.update(dt); break;
        default: break;
    }

//    handleInput();
}

void GameState::handleInput()
{
   if(mCurrentScreen == GameState::Screen::Game
       && mWorld)
   {
      if(!mPausedScreen.isPaused())
      {
         if(mWorld)
               mWorld->handleInput();
      }
      else
         mPausedScreen.handleInput();
    }
    else if(mCurrentScreen == GameState::Screen::LevelComplete)
        mLevelCompleteScreen.handleInput();
    else if(mCurrentScreen == GameState::Screen::GameComplete)
        mGameCompleteScreen.handleInput();
}

void GameState::display()
{
    if(mWorld)
        mWorld->display();

    if(mCurrentScreen == GameState::Screen::Game)
    {
        if(mPausedScreen.isPaused())
            mWindow.draw(mPausedScreen);
    }
    else if(mCurrentScreen == GameState::Screen::LevelComplete)
        mWindow.draw(mLevelCompleteScreen);
    else if(mCurrentScreen == GameState::Screen::GameComplete)
        mWindow.draw(mGameCompleteScreen);
}

void GameState::pause()
{
    mPausedScreen.setup(mWindow.getView());
}

void GameState::levelComplete()
{
  quitGameState();
}

void GameState::gameComplete(int sheepFromLastLevel)
{
  quitGameState();
    //mNewScreen = GameState::Screen::GameComplete;

    //mGameCompleteScreen.setup(mWindow.getView(), mTotalEnemiesTrapped + sheepFromLastLevel);
}

void GameState::nextLevel()
{
  quitGameState();
}

void GameState::resetGame()
{
    //mWorldDim = mResetWorldDim;
    //mLevelTime = sf::seconds(mResetLevelTime * 60.f);

    mTotalEnemiesTrapped = 0;

    restartWorld();
}

void GameState::quitGameState()
{
    mController.changeState();
}
