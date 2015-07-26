/*
*   Adventurer.cpp
*
*   @Author Richard Slade
*   @Date 12/2014
*/

#include <cstdlib>

#include "Entity/Adventurer.hpp"
#include "World/World.hpp"
#include "App\Utility.hpp"

Adventurer::Adventurer(const sf::RenderWindow& window
  , World* world
  , const sf::Texture& texture
  , const sf::Font& font
  , sf::Vector2f startPos
  , EntityStats stats
  , const Params& params
  , float scale)
  : Entity(world
  , texture
  , font
  , startPos
  , stats
  , params
  , Entity::Type::Adventurer
  , scale)
  , mWindow(window)
  , mFlashTime(sf::seconds(3.f))
  , mFlashCounter(sf::Time::Zero)
{
  setSteeringTypes(SteeringBehaviour::Behaviour::FollowPath);
  mText.setString("....");
}

/*
*   Main update for sheep dog
*/
void Adventurer::updateCurrent(sf::Time dt)
{
  // Check if entity has lost life or is dead and needs to be removed
  if (isDead())
  {
    mToRemove = true;
  }

  if (mFlashCounter > sf::Time::Zero) // Flash if lost life
  {
    mFlashCounter -= dt;

    mSprite.setColor(sf::Color(255, 255, 255, 255));

    if (static_cast<int>(mFlashCounter.asMilliseconds()) % 2 == 0)
    {
      mSprite.setColor(sf::Color(255, 255, 255, 0));
    }

    if (mFlashCounter < sf::Time::Zero)
    {
      mSprite.setColor(sf::Color(255, 255, 255, 255));
      lostLife = false;
    }

  }
  else if (hasLostLife())
  {
    mFlashCounter = mFlashTime;
  }

  sf::Vector2f steering = mSteering.calculate(dt);
  mVelocity += steering;

  if (std::fabs(magVec(mVelocity)) > MINFLOAT)
  {
    int sign = signVec(mHeading, mVelocity);

    float angle = std::acos(dotVec(mHeading, normVec(mVelocity)));
    angle *= sign;

    clampRotation(angle
      , -mMaxTurnRate
      , mMaxTurnRate);

    if (angle > MINFLOAT || angle < -MINFLOAT)
      rotate(angle * (180.f / PI));
  }

  float currentRotation = getRotation() * (PI / 180.f);
  mHeading = sf::Vector2f(std::sin(currentRotation), -std::cos(currentRotation));

  move(mVelocity);

  std::vector<Scenery*> obstacles = getObstacles();

  for (int i = 0; i < obstacles.size(); i++)
  {
    float mag = magVec(getWorldPosition() - obstacles.at(i)->getWorldPosition());

    if (mag < 40.f)
    {
      move((getWorldPosition() - obstacles.at(i)->getWorldPosition()) * 0.1f);
      break;
    }
  }

  Entity::updateCurrent(dt);
}

/*
*   Draw function used by SFML to render to sf::RenderTarget
*/
void Adventurer::drawCurrent(sf::RenderTarget& target
  , sf::RenderStates states) const
{
  Entity::drawCurrent(target, states);

  std::vector<sf::CircleShape> wypnts = mSteering.getPathToDraw();

  for (sf::CircleShape circle : wypnts)
    target.draw(circle);
}

