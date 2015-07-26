#include <string>

#include "Entity/Entity.hpp"
#include "World/World.hpp"
#include <World\World.hpp>

Entity::Entity(World* world
                , const sf::Texture& texture
               , const sf::Font& font
               , sf::Vector2f startPos
               , EntityStats stats
               , const Params& params
               , Type type
               , float scale)
: Killable(stats.lives)
, mWorld(world)
, mMass(stats.mass)
, mWalkMaxSpeed(stats.walkMaxSpeed)
, mRunMaxSpeed(stats.runMaxSpeed)
, mMaxForce(stats.maxForce)
, mMaxTurnRate(stats.maxTurnRate)
, mMaxSpeed(mWalkMaxSpeed)
, mEntityType(type)
, mSprite(texture)
, mVelocity(0.f, 0.f)
, mHeading(0.f, 0.f)
, mSteering(this, params)
, mCurrentTarget(nullptr)
, mText(".....", font, 12)
, mWanderTarget(5.f)
, mRadius(20.f)
{
   mSprite.scale(scale, scale);
   sf::FloatRect bounds = mSprite.getLocalBounds();
   mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

   sf::Transformable::setPosition(startPos);

  mWanderTarget.setFillColor(sf::Color::Magenta);
  bounds = mWanderTarget.getLocalBounds();
  mWanderTarget.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

  adjustPosition();
}

void Entity::updateCurrent(sf::Time dt)
{
    //Check if entity is dead and needs to be removed
   ensureZeroOverlap();

   float currentRotation = getRotation();
   mHeading = sf::Vector2f(std::sin(degreeToRadian(currentRotation)), -std::cos(degreeToRadian(currentRotation)));

   sf::Color currentTextColor = mText.getColor();
   currentTextColor.a -= 1;
   mText.setColor(currentTextColor);
}

void Entity::drawCurrent(sf::RenderTarget& target
                        , sf::RenderStates states) const
{
  target.draw(mSprite, states);
  target.draw(mText, states);
}


void Entity::adjustPosition()
{
    //sf::IntRect worldBounds = mLevel->getWorldBounds();

  sf::FloatRect worldBounds = mWorld->getWorldBounds();
    sf::Vector2f pos = getWorldPosition();

    pos.x = std::min(pos.x, static_cast<float>(worldBounds.width));
    pos.x = std::max(pos.x, 0.f);
    pos.y = std::min(pos.y, static_cast<float>(worldBounds.height));
    pos.y = std::max(pos.y, 0.f);

    setPosition(pos);
}

void Entity::ensureZeroOverlap()
{
    std::list<Entity*> neighbours;
    //getNeighbours(neighbours,
    //              Entity::Type::AllTypes);

  //std::list<Entity*> neighbours = mLevel->getEntitiesInRange(const_cast<Entity*>(this),
  //                                                           mRadius);

  //sf::Vector2f pos = getWorldPosition();
  //float radius = getRadius();

  //for(Entity* e : neighbours)
  //{
  //  if(e != this)
  //  {
  //    sf::Vector2f awayFromEntity = pos - e->getWorldPosition();
  //    float expandedRadius = radius + e->getRadius();
  //    float mag = magVec(awayFromEntity);

  //    if(mag < expandedRadius)
  //    {
  //      move(normVec(awayFromEntity));
  //    }
  //  }
  //}
}

std::vector<Scenery*> Entity::getObstacles() const
{
  return mWorld->getObstacles(getWorldPosition());
}

void Entity::decEnemies()
{ 
  mWorld->decEnemies(); 
}

