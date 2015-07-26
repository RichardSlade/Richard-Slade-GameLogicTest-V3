#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

#include "App/AppState.hpp"
#include "App/PausedScreen.hpp"
#include "World/World.hpp"

class Controller;
class Params;

class GameState : public AppState
{
private:
  enum Screen
  {
    Game,
    Paused
  };

  const int                       mResetNumEnemy;

  int                           mNumEnemy;
  int                           mTotalEnemiesTrapped;

  Controller&                     mController;
  sf::RenderWindow&               mWindow;

  float                           mWorldDim;
  sf::Time                        mLevelTime;


  std::string                     mUsername;

  std::unique_ptr<World>          mWorld;

  PausedScreen                    mPausedScreen;

  Screen                          mCurrentScreen;
  Screen                          mNewScreen;
  bool                            mPaused;

  void                            restartWorld();

public:
  GameState(Controller&
    , sf::RenderWindow&
    , std::string);

  virtual                         ~GameState(){};

  virtual void                    update(sf::Time);
  virtual void                    handleInput();
  virtual void                    display();

  void                            pause();
  void                            levelComplete();
  void                            gameComplete(int);

  void                            nextLevel();
  void                            resetGame();

  void                            quitGameState();

  // Getters
  bool                            getPaused(){ return mPaused; }
  int                    getNumEnemy(){ return mNumEnemy; }
  int                    getTotalEnemiesTrapped() { return mTotalEnemiesTrapped; }

  // Setters
  void                            incEnemiesTrapped(){ mTotalEnemiesTrapped++; }
  void                            decEnemies() { mNumEnemy--; }
};

#endif // GAMESTATE_HPP
