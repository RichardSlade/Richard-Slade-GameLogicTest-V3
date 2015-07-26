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
#include "World/LevelBlock.hpp"
#include "SceneNode/SpriteNode.hpp"
#include "Entity/Adventurer.hpp"
#include "Entity/Enemy.hpp"
#include "Entity/State/EnemyStates.hpp"
//#include "Entity/State/AdventurerStates.hpp"
#include "SceneNode/SpriteNode.hpp"

//const sf::Time World::mComboTime = sf::seconds(1.f);

World::World(GameState& gameState
             , const Controller& controller
             , sf::RenderWindow& window
             , std::string username
             , int worldDim
             , int numEnemy
             , sf::Time levelTime)
: mViewSize(640, 480)
, mWorldBounds(sf::Vector2f(0, 0),
               sf::Vector2f(worldDim, worldDim))
//, mPhysicsWorldDim(worldDim)
//, mPhysicsWorldX(worldDim)
//, mPhysicsWorldY(worldDim)
//, mPixelPerMeter(40.f)
//, mPhysicsWorldScale(1.f / PixelPerMeter)
//, mVelocityIter(6)
//, mPositionIter(2)
//, mTimeStep(1.f / 60.f)
, mLevelBlockX(controller.getParams().LevelBlockX)
, mLevelBlockY(controller.getParams().LevelBlockY)
, mLevelBlockSize(controller.getParams().LevelBlockSize)
, mWaypointRadius(controller.getParams().WaypointRadius)
, mScrollSpeed(controller.getParams().ScrollSpeed)
, mGameState(gameState)
, mWindow(window)
, mWorldView(mWindow.getDefaultView())
//, mWorldRect(sf::Vector2f(mWorldBounds.width, mWorldBounds.height))
, mFocusPoint(0.f, 0.f)
, mQuadTree(new QuadTree(0,
                         mWorldBounds))
//, mLevel(new Level(mLevelBlockSize,
//                  controller.getParams().LevelBlockX,
//                  controller.getParams().LevelBlockY,
//                  mWorldBounds))
, mHUD(this
       , controller.getFont(Controller::Fonts::Sansation)
       , getViewBounds()
       , username)
, mCurrentAdventurer(nullptr)
, mCurrentAdventurerIndex(0)
{
//    mLevel = std::unique_ptr<Level>(new Level(controller.getParams().LevelBlockSize
//                                            , mWorldBounds));

//    mLevel = std::unique_ptr<Level>(new Level(controller.getParams().LevelBlockSize
//                                            , mWorldBounds));

    initialiseStatesAndStats();
    buildScene(controller);
}

void World::initialiseStatesAndStats()
{
    // Initialise enemy states
    mEnemyStates.push_back(std::unique_ptr<EnemyStates::LookOut>(new EnemyStates::LookOut));
    mEnemyStates.push_back(std::unique_ptr<EnemyStates::Relax>(new EnemyStates::Relax));
    mEnemyStates.push_back(std::unique_ptr<EnemyStates::Attack>(new EnemyStates::Attack));
//    mEnemyStates.push_back(std::unique_ptr<Exit>(new Exit));

    //mAdventurerStates.push_back(std::unique_ptr<AdventurerStates::LookOut>(new AdventurerStates::LookOut));
//    mAdventurerStates.push_back(std::unique_ptr<AdventurerStates::Evade>(new AdventurerStates::Evade));
    //mAdventurerStates.push_back(std::unique_ptr<AdventurerStates::Relax>(new AdventurerStates::Relax));
//    mAdventurerStates.push_back(std::unique_ptr<Exit>(new Exit));

    mEntityStats.push_back(EntityStats("data/entityStats/enemyStats.dat"));
    mEntityStats.push_back(EntityStats("data/entityStats/adventurerStats.dat"));
}

void World::buildScene(const Controller& controller)
{
    // Initialize the different scene graph layers
	for(std::size_t i = 0; i < SceneNode::Layers::Num; i++)
	{
		SceneNode::upScNode layer(new SceneNode());
		mSceneLayers.at(i) = layer.get();

		mSceneGraph.addChild(std::move(layer));
	}

//    sf::Vector2f bckgrndSpritePos(mWorldBounds.width / 2.f
//                                  , mWorldBounds.height / 2.f);

   sf::IntRect backgroundSpriteBounds = sf::IntRect(sf::Vector2i(0, 0),
                                                    sf::Vector2i(mWorldBounds.width,
                                                                 mWorldBounds.height));

    std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(controller.getTexture(Controller::Textures::Brick),
                                                                backgroundSpriteBounds));

//    mBackground = backgroundSprite.get();
  mSceneLayers.at(SceneNode::Layers::Background)->addChild(std::move(backgroundSprite));


  // Build level walls
  sf::Vector2f pos(mWorldBounds.width / 2.f, 0.f);
  sf::Vector2f worldSize(mWorldBounds.width, 1.f);

  for(int i =  0; i < 4; i++)
  {
    sf::Vector2f pixelPos(pos);
    sf::Vector2f pixelSize(worldSize);

    sf::IntRect spriteSize(sf::Vector2i(pos - (pixelSize / 2.f)),
                           sf::Vector2i(pixelSize));

    mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(Scenery::upScenery(new Scenery(controller.getTexture(Controller::Textures::Stone),
                                                                                            pos,
                                                                                            spriteSize,
                                                                                            worldSize,
                                                                                            Scenery::Type::Wall)));

    switch(i){
      case 0: pos.y = mWorldBounds.height; break;
      case 1:
      {
        pos = sf::Vector2f(0.f, mWorldBounds.height / 2.f);
        worldSize = sf::Vector2f(1.f, mWorldBounds.height);
        break;
      }
      case 2: pos.x = mWorldBounds.width; break;
      default: break;
    }
  }

  // Place obstacles
  float obstacleSize = 1.f;

  pos = sf::Vector2f(mLevelBlockSize / 2,
                     mLevelBlockSize / 2);

  unsigned int inc = 0;

  for(unsigned int row =  0; row < mLevelBlockY; row+=4)
  {
    mObstacles.push_back(std::vector<Scenery*>());

    for (unsigned int col = 0; col < mLevelBlockX; col+=4)
    {

        Scenery::upScenery trap(new Scenery(controller.getTexture(Controller::Textures::Trap),
          pos,
          obstacleSize,
          Scenery::Type::Trap));

        //mQuadTree->insert(trap.get());

        mObstacles.at(inc).push_back(trap.get());
      mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(std::move(trap));

      pos.x += ((mLevelBlockSize * 4) + (mLevelBlockSize / 2));
    }

    inc++;
    pos.x = mLevelBlockSize / 2;
    pos.y += ((mLevelBlockSize * 4) + (mLevelBlockSize / 2));
  }

  //mLevel->generateLevel(mSceneLayers,
  //                     controller);

  generateAgents(controller);
}

void World::generateAgents(const Controller& controller)
{
  sf::Vector2f pos;

    // Initialise characters and add to scene graph
   for(int i = 0; i < 1; i++)
   {
//      float inc = i * 40.f;

//      sf::Vector2f pos((mWorldBounds.width / 2.f) + inc, (mWorldBounds.height / 2.f) + inc);
//      sf::Vector2f pos(mWorldBounds.width / 2.f, mWorldBounds.height / 2.f);
//      sf::Vector2f pos(10, 10);
      pos = sf::Vector2f(mWorldBounds.width / 2.f, mWorldBounds.height / 4.f);
//      float size = controller.getTexture(Controller::Textures::Adventurer).getSize().x;


      std::unique_ptr<Adventurer> adventurerNode(new Adventurer(mWindow
                                                               //, mQuadTree.get()
                                                               //, mLevel.get()
                                                               , this
                                                               , controller.getTexture(Controller::Textures::Adventurer)
                                                               , controller.getFont(Controller::Fonts::Sansation)
                                                               , pos
                                                               , mEntityStats.at(World::Stats::AdventurerStats)
                                                               , controller.getParams()));


   // Save pointer to character for enemy initialisation
//      Adventurer* adventurerPtr = adventurerNode.get();
      mAdventurers.push_back(adventurerNode.get());

      mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(std::move(adventurerNode));
   }

    mCurrentAdventurer = mAdventurers.at(0);
    //mCurrentAdventurer->setIsSelected(true);

    sf::Vector2f startPos(mLevelBlockSize * ((mLevelBlockX / 2) - 2), mLevelBlockSize * ((mLevelBlockY / 2) - 6));
    pos = startPos;

  for (unsigned int row = 0; row < 4; row++)
  {
    for (unsigned int col = 0; col < 2; col++)
    {
      std::unique_ptr<Enemy> enemyNode(new Enemy(//mQuadTree.get()
        //mLevel.get()
        this
        , controller.getTexture(Controller::Textures::Enemy)
        , controller.getFont(Controller::Fonts::Sansation)
        , pos
        , mEntityStats.at(World::Stats::EnemyStats)
        , controller.getParams()
        , mEnemyStates.at(Enemy::States::LookOut).get()
        , mEnemyStates.at(Enemy::States::Relax).get()
        , mEnemyStates
        , Enemy::States::Relax));

      //        enemyNode->setMovingTarget(mAdventurers.at(0));
      mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(std::move(enemyNode));

      pos.x += mLevelBlockSize * 4;
    }

    pos.x = startPos.x;
    pos.y += mLevelBlockSize * 4;
  }

//    // Initialise enemy and add to scene graph
//    for(int i = 0 ; i < mGameState.getNumEnemy(); i++)
//    {
//      // Find square for enemy to start in
////      LevelBlock* levelBlock;
//      sf::Vector2f pos(mWorldBounds.width / 2.f, mWorldBounds.height / 2.f);
//
//      pos.x = rangedClamped(2.f, mWorldBounds.width - 2.f);
//      pos.y = rangedClamped(2.f, mWorldBounds.width - 2.f);
//
////      int maxX = mLevel->getLevelX();
//
////      index = sf::Vector2i(rangedClamped(1, maxX - 2)
////                           , rangedClamped(1, maxX - 2));
//
////      levelBlock = mLevel->getBlock(index);
//
////      pos = levelBlock->getCenter();
//
////      float size = controller.getTexture(Controller::Textures::Enemy).getSize().x;
//
//      std::unique_ptr<Enemy> enemyNode(new Enemy(//mQuadTree.get()
//                                                //mLevel.get()
//                                                this
//                                                , controller.getTexture(Controller::Textures::Enemy)
//                                                , controller.getFont(Controller::Fonts::Sansation)
//                                                , pos
//                                                , mEntityStats.at(World::Stats::EnemyStats)
//                                                , controller.getParams()
//                                                , mEnemyStates.at(Enemy::States::LookOut).get()
//                                                , mEnemyStates.at(Enemy::States::Relax).get()
//                                                , mEnemyStates
//                                                , Enemy::States::Relax));
//
////        enemyNode->setMovingTarget(mAdventurers.at(0));
//      mSceneLayers.at(SceneNode::Layers::Foreground)->addChild(std::move(enemyNode));
//   }
}

void World::handleRealTimeInput()
{
//   sf::Vector2f adventurerMovement = mCurrentAdventurer->getVelocity() / 2.f;
   //sf::Vector2f movement;// = mCurrentAdventurer->getVelocity() / 2.f;
   //float speed = 4.f;

   //if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)
   //|| sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
   //{
   //   movement.x -= speed;
   //}

   //if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)
   //|| sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
   //{
   //   movement.x += speed;
   //}

   //if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)
   //|| sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
   //{
   //   movement.y -= speed;
   //}

   //if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)
   //|| sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
   //{
   //   movement.y += speed;
   //}

//   if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
//   {
//      mFocusPoint += movement;
//   }
//   else
//   {
//      if(mCurrentAdventurer)
//      {
//         mCurrentAdventurer->setVelocity(movement);
////         mCurrentAdventurer->setVelocity(sf::Vector2f(1.f, 2.f));
//         adaptPlayerVelocity();
////         mFocusPoint =
//   //      mFocusPoint = meterToPixel(mCurrentAdventurer->getWorldPosition());
//      }
//   }
}

void World::adjustView()
{
   sf::View view = mWindow.getView();
   view.setCenter(mFocusPoint);

   sf::FloatRect bckgrndBounds = mBackground->getGlobalBounds();

   sf::Vector2f center = view.getCenter();
   sf::Vector2f dim = view.getSize();
   dim /= 2.f;

   if(center.x - dim.x < bckgrndBounds.left)
      mFocusPoint.x += mScrollSpeed;
   else if(center.x + dim.x > bckgrndBounds.left + bckgrndBounds.width)
      mFocusPoint.x -= mScrollSpeed;

   if(center.y - dim.y < bckgrndBounds.top)
      mFocusPoint.y += mScrollSpeed;
   else if(center.y + dim.y > bckgrndBounds.top + bckgrndBounds.height)
      mFocusPoint.y -= mScrollSpeed;
}

void World::rotateViewToAdventurer()
{
//   if(std::fabs(magVec(mVelocity)) > MINFLOAT)
//   {
//     int sign = signVec(mHeading, mVelocity);
//
//     float angle = std::acos(dotVec(mHeading, normVec(mVelocity)));
//     angle *= sign;
//
//     clampRotation(angle
//                   , -mMaxTurnRate
//                   , mMaxTurnRate);
//
//     if(angle > MINFLOAT || angle < -MINFLOAT)
//         rotate(angle * (180.f / SteeringBehaviour::mPI));
//   }
//
//   float currentRotation = getRotation() * (SteeringBehaviour::mPI / 180.f);
//   mHeading = sf::Vector2f(std::sin(currentRotation), -std::cos(currentRotation));
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mCurrentAdventurer->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mCurrentAdventurer->setVelocity(velocity / std::sqrt(2.f));
}

void World::update(sf::Time dt)
{
  if (mCurrentAdventurer->isDead())
    mGameState.levelComplete();

  //mQuadTree->clear();

  mSceneGraph.removeDeletedNodes();
  //mSceneGraph.addToQuadTree(mQuadTree.get());
  mSceneGraph.update(dt);

  //mTimeLeft -= dt;
  //mTimeTaken += dt;

  mHUD.update(mCurrentAdventurer->getLives());

//    if(mEnemyHerded == mNumEnemy)
//        mGameState.levelComplete();
//    else if(mTimeLeft < sf::Time::Zero)
//        mGameState.gameComplete(mEnemyHerded);
}

void World::handleInput()
{
    sf::Event event;

    while(mWindow.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            mWindow.close();
        else if(event.type == sf::Event::KeyReleased)
        {
            if(event.key.code == sf::Keyboard::Escape)
                mWindow.close();
        }
        else if(event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i mousePos;
            sf::FloatRect vBounds = getViewBounds();

            mousePos.x = sf::Mouse::getPosition(mWindow).x + vBounds.left;
            mousePos.y = sf::Mouse::getPosition(mWindow).y + vBounds.top;

            sf::Vector2f mousePosF(mousePos.x, mousePos.y);

            if(event.mouseButton.button == sf::Mouse::Left)
            {
                mousePosF.x = std::min(mousePosF.x, mWorldBounds.width);
                mousePosF.x = std::max(mousePosF.x, 0.f);

                mousePosF.y = std::min(mousePosF.y, mWorldBounds.height);
                mousePosF.y = std::max(mousePosF.y, 0.f);

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    if(mCurrentAdventurer)
                        mCurrentAdventurer->addToPath(mousePosF);
                }
                else
                {
                    if(mCurrentAdventurer)
                        mCurrentAdventurer->startNewPath(mousePosF);
                }
            }
        }
    }

    handleRealTimeInput();
}

void World::display()
{
//    adjustView();

   if(mCurrentAdventurer)
      mFocusPoint = mCurrentAdventurer->getWorldPosition();
//      mFocusPoint = mCurrentAdventurer->Transformable::getPosition();

   mWorldView.setCenter(mFocusPoint);
   mWindow.setView(mWorldView);

//   mWindow.draw(mWorldRect);
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

  //sf::Vector2i index((pos.x / 40) / 4, (pos.y / 40) / 4);

  //unsigned int row = index.y > 0 ? index.y - 1 : index.y;
  //unsigned int col = index.x > 0 ? index.x - 1 : index.x;

  //unsigned int maxRow = index.y != mLevelBlockY ? index.y + 1 : index.y;
  //unsigned int maxCol = index.x != mLevelBlockX ? index.x + 1 : index.x;

  //for (row; row < maxRow; row++)
  //{
  //  for (col; col < maxCol; col++)
  //  {
  //    obstacles.push_back(mObstacles.at(row).at(col));
  //  }
  //}

  return obstacles;
}
