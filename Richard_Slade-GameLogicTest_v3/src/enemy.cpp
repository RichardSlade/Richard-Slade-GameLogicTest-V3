#include <iostream>
#include <cmath>
#include <vector>

#include "App/Utility.hpp"
#include "World/World.hpp"
#include "Entity/Enemy.hpp"

Enemy::Enemy(QuadTree* quadTree
//Level* level
, const sf::Texture& texture
, const sf::Font& font
, sf::Vector2f pos
, EntityStats stats
, const Params& params
, State<Enemy>* globalState
, State<Enemy>* initState
, StateContainer& states
, unsigned int currentState
, float scale)
: Entity(quadTree
         , texture
         , font
         , pos
         , stats
         , params
         , Entity::Type::Enemy
         , scale)
, mSightRange(params.EnemySightRange)
, mAngleOfVision(params.EnemyAngleOfVision)
, mAggroDistance(stats.aggroDistance)
, mStates(states)
, mStateMachine(this, globalState, initState, currentState)
{
//    mText.setPosition(-10.f, -40.f);

//    mCurrentBlock = mLevel->insertEntityIntoLevel(this);
}

void Enemy::updateCurrent(sf::Time dt)
{
  // Check if entity is dead and needs to be removed
  if(isDead())
     mToRemove = true;

   mStateMachine.update();

   // Velocity
   sf::Vector2f steering = mSteering.calculate(dt);
   mVelocity += steering * dt.asSeconds();

   truncateVec(mVelocity, mMaxSpeed);

   move(mVelocity);

   // Check if enemy has moved onto trap
   //std::list<Scenery*> traps;
   //mQuadTree->retrieveScenery(traps,
   //                           this,
   //                           Scenery::Type::Trap);

   //std::list<Scenery*>::iterator iter;

   //for (iter = traps.begin();
   //  iter != traps.end();
   //  iter++)
   //{
   //  float mag = magVec(getWorldPosition() - (*iter)->getWorldPosition());

   //  if (mag < 10.f)
   //    decreaseLives();
   //}

   //mBody->SetLinearVelocity(convertVec(mVelocity));

   int sign = signVec(mHeading, mVelocity);

   float angle = std::acos(dotVec(mHeading, normVec(mVelocity)));
   float angle2 = dotVec(mHeading, normVec(mVelocity));
   //    angle *= sign;

   //       clampRotation(angle
   //                     , -mMaxTurnRate
   //                     , mMaxTurnRate);

   //       sf::Vector2f toCursor = mVelocity;
   //       float angle2 = std::atan2(mVelocity.x, -mVelocity.y);

   //  std::cout << "Entity angle 2: " << angle2 << std::endl

   //  std::cout << "Angle: " << radianToDegree(angle) << std::endl;
   //  std::cout << "Angle2: " << angle2 << std::endl;

   if (std::abs(radianToDegree(angle)) > 8.f)
     rotate(radianToDegree(angle));

   Entity::updateCurrent(dt);
}

void Enemy::drawCurrent(sf::RenderTarget& target
                        , sf::RenderStates states) const
{
   Entity::drawCurrent(target, states);
}

//void Enemy::changeState(Enemy::States newState)
//{
//    mStateMachine.changeState(mStates.at(newState).get());
//}
