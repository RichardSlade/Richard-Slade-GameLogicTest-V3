#ifndef WORLD_HPP
#define WORLD_HPP

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "SceneNode/SceneNode.hpp"
#include "Entity/Enemy.hpp"
#include "Entity/Adventurer.hpp"
#include "Entity/State/State.hpp"
#include "Entity/EntityStats.hpp"
#include "App/HUD.hpp"

class Controller;
class Waypoint;
class MovingEntity;
class GameState;
class SpriteNode;
class Scenery;

class World : private sf::NonCopyable
{
public:
  enum Stats
  {
    EnemyStats,
    AdventurerStats,
    StatsTypeNum
  };

private:
  const sf::Vector2f                           mViewSize;
  const sf::FloatRect                            mWorldBounds;

  const  int                             mLevelBlockX;
  const  int                            mLevelBlockY;
  const  int                           mLevelBlockSize;
  const unsigned int                           mSpacing;

  const float                                  mWaypointRadius;
  const float                                  mScrollSpeed;

  std::vector<unsigned int>                    mPlayerLives;

  GameState&                                   mGameState;
  sf::RenderWindow&                            mWindow;
  sf::View                                     mWorldView;
  sf::Vector2f                                 mFocusPoint;

  std::vector<std::vector<Scenery*>>           mObstacles;

  SceneNode                                    mSceneGraph;
  std::array<SceneNode*
    , SceneNode::Layers::Num>          mSceneLayers;

  HUD                                          mHUD;

  std::vector<EntityStats>                     mEntityStats;
  Enemy::StateContainer                        mEnemyStates;

  SpriteNode*                                  mBackground;
  sf::Vector2f                                 mExitPos;
  std::vector<Adventurer*>                     mAdventurers;

  Adventurer*                                  mCurrentAdventurer;
  unsigned int                                 mCurrentAdventurerIndex;

  void                                         initialiseStatesAndStats();
  void                                         buildScene(const Controller&);
  void                                         generateAgents(const Controller&);

  void                                        handleRealTimeInput();

public:
  World(GameState&
    , const Controller&
    , sf::RenderWindow&
    , std::string
    , int worldDim
    , int numEnemy);

  void                                        update(sf::Time);
  void                                        handleInput();
  void                                        display();

  void                                        regenWorld(const Controller& controller);

  // Getters
  const sf::FloatRect                          getViewBounds() const;

  std::vector<Scenery*>                        getObstacles(sf::Vector2f pos);

  const sf::FloatRect                          getWorldBounds() const { return mWorldBounds; }

  // Setters
  void                                         incEnemiesTrapped();
  void                                         decEnemies();
};

#endif // WORLD_HPP
