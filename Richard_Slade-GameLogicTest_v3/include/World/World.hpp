#ifndef WORLD_HPP
#define WORLD_HPP

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/CircleShape.hpp>
//#include <SFML/Graphics/Sprite.hpp>
//#include <SFML/Graphics/Font.hpp>
//#include <SFML/System/NonCopyable.hpp>

//#include "World/Level.hpp"
#include "World/QuadTree.hpp"
#include "SceneNode/SceneNode.hpp"
#include "Entity/Enemy.hpp"
#include "Entity/Adventurer.hpp"
#include "Entity/State/State.hpp"
#include "Entity/EntityStats.hpp"
#include "App/HUD.hpp"

class Controller;
//class LevelBlock;
class Wall;
class Waypoint;
class MovingEntity;
class GameState;
class SpriteNode;
class Scenery;
//class Adventurer;

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


//   const float                                  mPhysicsWorldDim;
//   const float                                  mPhysicsWorldY;
//   const float                                  mPixelPerMeter;
//   const float                                  mPhysicsWorldScale;

   //const int32                                  mVelocityIter;
   //const int32                                  mPositionIter;
   //const float32                                mTimeStep;
   const unsigned int                             mLevelBlockX;
   const unsigned int                            mLevelBlockY;
   const unsigned int                           mLevelBlockSize;

   const float                                  mWaypointRadius;
   const float                                  mScrollSpeed;

   std::vector<unsigned int>                    mPlayerLives;

   GameState&                                   mGameState;
   sf::RenderWindow&                            mWindow;
   sf::View                                     mWorldView;
//   sf::RectangleShape                           mWorldRect;
   sf::Vector2f                                 mFocusPoint;
   QuadTree::upQuadTree                         mQuadTree;

   std::vector<std::vector<Scenery*>>           mObstacles;

   SceneNode                                    mSceneGraph;
   std::array<SceneNode*
             , SceneNode::Layers::Num>          mSceneLayers;

   //sf::Time                                     mTimeLeft;
   //sf::Time                                     mTimeTaken;

   //std::unique_ptr<Level>                       mLevel;

   HUD                                          mHUD;

   std::vector<EntityStats>                     mEntityStats;
   //Adventurer::StateContainer                   mAdventurerStates;
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
    void                                        adjustView();
    void                                        rotateViewToAdventurer();

    //void                                        cycleAdventurer();

   void	                                       adaptPlayerVelocity();

public:
                                                World(GameState&
                                                      , const Controller&
                                                      , sf::RenderWindow&
                                                      , std::string
                                                      , int worldDim
                                                      , int numEnemy
                                                      , sf::Time levelTime);

    void                                        update(sf::Time);
    //void                                        updatePhysicsEngine();
    void                                        handleInput();
    void                                        display();

    // Getters
   const sf::FloatRect                          getViewBounds() const;

   std::vector<Scenery*>                        getObstacles(sf::Vector2f pos);

    const sf::FloatRect                          getWorldBounds() const { return mWorldBounds; }
};

#endif // WORLD_HPP
