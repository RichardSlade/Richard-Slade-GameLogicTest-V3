#include <iostream>

#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include "App/Utility.hpp"
#include "App/Controller.hpp"
#include "App/GameState.hpp"
#include "App/HUD.hpp"
#include "World/World.hpp"
#include "World/Scenery.hpp"
#include "SceneNode/SpriteNode.hpp"
#include "Entity/Adventurer.hpp"
#include "Entity/Enemy.hpp"
#include "Entity/State/EnemyStates.hpp"
#include "SceneNode/SpriteNode.hpp"

World::World(GameState& gameState
  , const Controller& controller
  , sf::RenderWindow& window
  , std::string username
  , int worldDim
  , int numEnemy)
  : mViewSize(640, 480)
  , mWorldBounds(sf::Vector2f(0, 0),
                 sf::Vector2f(worldDim, worldDim))
  , mLevelBlockX(controller.getParams().LevelBlockX)
  , mLevelBlockY(controller.getParams().LevelBlockY)
  , mLevelBlockSize(controller.getParams().LevelBlockSize)
  , mSpacing(4)
  , mWaypointRadius(controller.getParams().WaypointRadius)
  , mScrollSpeed(controller.getParams().ScrollSpeed)
  , mGameState(gameState)
  , mWindow(window)
  , mWorldView(mWindow.getDefaultView())
  , mFocusPoint(0.f, 0.f)
  , mHUD(this
  , controller.getFont(Controller::Fonts::Sansation)
  , getViewBounds()
  , username)
  , mCurrentAdventurer(nullptr)
  , mCurrentAdventurerIndex(0)
{
  initialiseStatesAndStats();
  buildScene(controller);
}

void World::initialiseStatesAndStats()
{
  // Initialise enemy states
  mEnemyStates.push_back(std::unique_ptr<EnemyStates::LookOut>(new EnemyStates::LookOut));
  mEnemyStates.push_back(std::unique_ptr<EnemyStates::Relax>(new EnemyStates::Relax));
  mEnemyStates.push_back(std::unique_ptr<EnemyStates::Attack>(new EnemyStates::Attack));

  mEntityStats.push_back(EntityStats("data/entityStats/enemyStats.dat"));
  mEntityStats.push_back(EntityStats("data/entityStats/adventurerStats.dat"));
}

void World::buildScene(const Controller& controller)
{
  // Initialize the different scene graph layers
  for (std::size_t i = 0; i < SceneNode::Layers::Num; i++)
  {
    SceneNode::upScNode layer(new SceneNode());
    mSceneLayers.at(i) = layer.get();

    mSceneGraph.addChild(std::move(layer));
  }

  sf::IntRect backgroundSpriteBounds = sf::IntRect(sf::Vector2i(0, 0),
    sf::Vector2i((mLevelBlockSize)* mLevelBlockX,
    (mLevelBlockSize)* mLevelBlockX));

  std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(controller.getTexture(Controller::Textures::Brick),
    backgroundSpriteBounds));

  mSceneLayers.at(SceneNode::Layers::Background)->addChild(std::move(backgroundSprite));

  // Place obstacles
  float obstacleSize = 40.f;

  sf::Vector2f pos(mLevelBlockSize / 2, mLevelBlockSize / 2);

  for (unsigned int row = 0; row < mLevelBlockY; row++)
  {
    mObstacles.push_back(std::vector<Scenery*>());

    if (row % 4 == 0)
    {
      for (unsigned int col = 0; col < mLevelBlockX; col++)
      {
        if (col % 4 == 0)
        {
          Scenery::upScenery trap(new Scenery(controller.getTexture(Controller::Textures::Trap),
            pos,
            obstacleSize,
            Scenery::Type::Trap));

          mObstacles.at(row).push_back(trap.get());
          mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(std::move(trap));


        }
        else
        {
          mObstacles.at(row).push_back(nullptr);
        }

        pos.x += mLevelBlockSize;
      }
    }
    else
    {
      for (unsigned int col = 0; col < mLevelBlockX; col++)
      {
        mObstacles.at(row).push_back(nullptr);
      }
    }

    pos.x = mLevelBlockSize / 2;

    pos.y += mLevelBlockSize;
  }

  generateAgents(controller);
}

void World::generateAgents(const Controller& controller)
{
  sf::Vector2f pos;

  // Initialise characters and add to scene graph
  for (int i = 0; i < 1; i++)
  {
    pos = sf::Vector2f(mLevelBlockSize * 2, mLevelBlockSize * 2);

    std::unique_ptr<Adventurer> adventurerNode(new Adventurer(mWindow, this
      , controller.getTexture(Controller::Textures::Adventurer)
      , controller.getFont(Controller::Fonts::Sansation)
      , pos
      , mEntityStats.at(World::Stats::AdventurerStats)
      , controller.getParams()));


    // Save pointer to character for enemy initialisation
    mAdventurers.push_back(adventurerNode.get());

    mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(std::move(adventurerNode));
  }

  mCurrentAdventurer = mAdventurers.at(0);

  sf::Vector2f startPos(mLevelBlockSize * ((mLevelBlockX / 2) - 2), mLevelBlockSize * ((mLevelBlockY / 2) - 4));
  pos = startPos;

  // Initialise Enemies
  for (unsigned int row = 0; row < 4; row++)
  {
    for (unsigned int col = 0; col < 2; col++)
    {
      std::unique_ptr<Enemy> enemyNode(new Enemy(this
        , controller.getTexture(Controller::Textures::Enemy)
        , controller.getFont(Controller::Fonts::Sansation)
        , pos
        , mEntityStats.at(World::Stats::EnemyStats)
        , controller.getParams()
        , mEnemyStates.at(Enemy::States::LookOut).get()
        , mEnemyStates.at(Enemy::States::Relax).get()
        , mEnemyStates
        , Enemy::States::Relax));

      enemyNode->setCurrentTarget(mAdventurers.at(0));
      mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(std::move(enemyNode));

      pos.x += (mLevelBlockSize * mSpacing);
    }

    pos.x = startPos.x;
    pos.y += (mLevelBlockSize * mSpacing);
  }
}

void World::handleRealTimeInput()
{

}

void World::update(sf::Time dt)
{
  if (mCurrentAdventurer->isDead())
    mGameState.levelComplete();

  mSceneGraph.removeDeletedNodes();
  mSceneGraph.update(dt);

  mHUD.update(mGameState.getLevel(),
              mGameState.getNumEnemy(),
              mCurrentAdventurer->getLives(),
              mGameState.getScore());

  // game ending scenarios
  if ((mGameState.getNumEnemy() <= 0 && mCurrentAdventurer->getLives() <= 0)
      || mCurrentAdventurer->getLives() <= 0)
  {
    mGameState.gameOver();
  }
  else if(mGameState.getNumEnemy() <= 0)
  {
    mGameState.levelComplete();
  }

}

void World::handleInput()
{
  sf::Event event;

  while (mWindow.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
      mWindow.close();
    else if (event.type == sf::Event::KeyReleased)
    {
      if (event.key.code == sf::Keyboard::Escape)
        mGameState.pause();
    }
    else if (event.type == sf::Event::MouseButtonPressed)
    {
      sf::Vector2i mousePos;
      sf::FloatRect vBounds = getViewBounds();

      mousePos.x = sf::Mouse::getPosition(mWindow).x + vBounds.left;
      mousePos.y = sf::Mouse::getPosition(mWindow).y + vBounds.top;

      sf::Vector2f mousePosF(mousePos.x, mousePos.y);

      if (event.mouseButton.button == sf::Mouse::Left)
      {
        mousePosF.x = std::min(mousePosF.x, mWorldBounds.width);
        mousePosF.x = std::max(mousePosF.x, 0.f);

        mousePosF.y = std::min(mousePosF.y, mWorldBounds.height);
        mousePosF.y = std::max(mousePosF.y, 0.f);

        bool collisionWithTrap = false;

        for (int i = 0; i < mObstacles.size(); i++)
        {
          for (int j = 0; j < mObstacles.at(0).size(); j++)
          {
            float mag = magVec(mousePosF - mObstacles.at(i).at(j)->getWorldPosition());

            if (mag < 20.f)
            {
              collisionWithTrap = true;
              break;
            }
          }
        }

        if (mCurrentAdventurer
          && !collisionWithTrap)
          mCurrentAdventurer->startNewPath(mousePosF);

      }
    }
  }

  handleRealTimeInput();
}

void World::display()
{
  if (mCurrentAdventurer)
    mFocusPoint = mCurrentAdventurer->getWorldPosition();

  mWorldView.setCenter(mFocusPoint);
  mWindow.setView(mWorldView);

  mWindow.draw(mSceneGraph);

  mHUD.setHUDPosition(getViewBounds());
  mWindow.draw(mHUD);
}

const sf::FloatRect World::getViewBounds() const
{
  return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

std::vector<Scenery*> World::getObstacles(sf::Vector2f pos)
{
  std::vector<Scenery*> obstacles;

  sf::Vector2i index(pos.x / mLevelBlockSize, pos.y / mLevelBlockSize);

  for (int i = index.y - 3; i < index.y + 3; i++)
  {
    for (int j = index.x - 3; j < index.x + 3; j++)
    {
      if (i < 0) i = 0;
      if (i > mLevelBlockY - 1) i = mLevelBlockY - 1;
      if (j < 0) j = 0;
      if (j > mLevelBlockX - 1) j = mLevelBlockX - 1;

      if (mObstacles.at(i).at(j))
        obstacles.push_back(mObstacles.at(i).at(j));
    }
  }

  // REturn sorounding traps
  return obstacles;
}

void World::incScore()
{
  mGameState.incScore();
}

void World::decEnemies()
{
  mGameState.decEnemies();
}