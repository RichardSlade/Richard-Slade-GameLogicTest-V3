#include <iostream>
#include <cmath>
#include <vector>

#include "App/Utility.hpp"
#include "World/World.hpp"
#include "Entity/Enemy.hpp"

Enemy::Enemy(World* world
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
  : Entity(world
  , texture
  , font
  , pos
  , stats
  , params
  , Entity::Type::Enemy
  , scale)
  , mAggroDistance(stats.aggroDistance)
  , mStates(states)
  , mStateMachine(this, globalState, initState, currentState)
{

}

void Enemy::updateCurrent(sf::Time dt)
{
  // Check if entity is dead and needs to be removed
  if (isDead())
  {
    mToRemove = true;
    return;
  }

  mStateMachine.update();

  // Velocity
  sf::Vector2f steering = mSteering.calculate(dt);
  mVelocity += steering * dt.asSeconds();

  truncateVec(mVelocity, mMaxSpeed);

  move(mVelocity);

  //Check if enemy has moved onto trap
  std::vector<Scenery*> traps = getObstacles();

  std::vector<Scenery*>::iterator iter;

  for (int i = 0; i < traps.size(); i++)
  {
    int mag = magVec(getWorldPosition() - traps.at(i)->getWorldPosition());

    if (mag < 40.f)
    {
      decreaseLives();
      mWorld->incScore();
      mWorld->decEnemies();

      break;
    }
  }

  int sign = signVec(mHeading, mVelocity);

  float angle = std::acos(dotVec(mHeading, normVec(mVelocity)));
  angle *= sign;

  clampRotation(angle
    , -mMaxTurnRate
    , mMaxTurnRate);

  sf::Vector2f toCursor = mVelocity;
  float angle2 = std::atan2(mVelocity.x, -mVelocity.y);


  if (std::abs(radianToDegree(angle)) > 8.f)
    rotate(radianToDegree(angle));

  Entity::updateCurrent(dt);
}

void Enemy::drawCurrent(sf::RenderTarget& target
  , sf::RenderStates states) const
{
  Entity::drawCurrent(target, states);
}

