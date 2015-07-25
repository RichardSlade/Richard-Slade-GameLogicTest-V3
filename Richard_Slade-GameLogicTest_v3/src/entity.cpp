#include <string>

#include "Entity/Entity.hpp"
#include "World/World.hpp"
//#include "World/LevelBlock.hpp"
#include "World/QuadTree.hpp"
//#include "World/Scenery.hpp"

Entity::Entity(//Level* level
                QuadTree* quadTree
               , const sf::Texture& texture
               , const sf::Font& font
               , sf::Vector2f startPos
               , EntityStats stats
               , const Params& params
               , Type type
               , float scale)
: Killable(stats.lives)
//, mLevel(level)
, mQuadTree(quadTree)
//, mPhysicsBody(body)
, mMass(stats.mass)
, mWalkMaxSpeed(stats.walkMaxSpeed)
, mRunMaxSpeed(stats.runMaxSpeed)
, mMaxForce(stats.maxForce)
, mMaxTurnRate(stats.maxTurnRate)
, mMaxSpeed(mWalkMaxSpeed)
, mEntityType(type)
//, mCurrentBlock(nullptr)
, mSprite(texture)
, mVelocity(0.f, 0.f)
, mHeading(0.f, 0.f)
, mSteering(this, params)
, mCurrentTarget(nullptr)
, mText(".....", font, 12)
, mWanderTarget(5.f)
//, mOrigin(5.f)
{
//  mWanderTarget.setPosition(sf::Vector2f(500.f, 500.f));

   mSprite.scale(scale, scale);
   sf::FloatRect bounds = mSprite.getLocalBounds();
   mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
//   mRadius = std::max(bounds.width, bounds.height);

   sf::Transformable::setPosition(startPos);

  mWanderTarget.setFillColor(sf::Color::Magenta);
  bounds = mWanderTarget.getLocalBounds();
  mWanderTarget.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

  //mOrigin.setFillColor(sf::Color::Green);
  //bounds = mOrigin.getLocalBounds();
  //mOrigin.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
  //mOrigin.setPosition(0.f, 0.f);
//   mBodyBounds = mSprite.getLocalBounds();

//   float theta = randomClamped() * (2.f * PI);
//   rotate(theta * (180 / PI));
//   mHeading = sf::Vector2f(std::sin(theta), -std::cos(theta));

   //bounds = mText.getLocalBounds();
   //mText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
   //mText.setPosition(0, -20.f);

   //mCurrentBlock = mLevel->insertEntityIntoLevel(this);
  //mQuadTree->insert(this);
}

void Entity::updateCurrent(sf::Time dt)
{
    //Check if entity is dead and needs to be removed


   ensureZeroOverlap();

   //sf::Transformable::setPosition(getWorldPosition());

   float currentRotation = getRotation();
   mHeading = sf::Vector2f(std::sin(degreeToRadian(currentRotation)), -std::cos(degreeToRadian(currentRotation)));

   //sf::Transformable::setRotation(currentRotation);

   //sf::Color currentTextColor = mText.getColor();
   //currentTextColor.a -= 1;
   //mText.setColor(currentTextColor);

   //setHPText(std::string(std::to_string(static_cast<int>(getHealthPercentage())) +  + "%"));

   //mCurrentBlock->deleteEntity(this);
   //mCurrentBlock = mLevel->insertEntityIntoLevel(this);

  //mQuadTree->insert(this);
}

void Entity::drawCurrent(sf::RenderTarget& target
                        , sf::RenderStates states) const
{
  target.draw(mSprite, states);
  //target.draw(mText, states);
  //target.draw(mHPText, states);
//  target.draw(mWanderTarget, states);
//  target.draw(mOrigin, states);
  //target.draw(mWanderTarget);
}


//void Entity::adjustPosition()
//{
//    sf::IntRect worldBounds = mLevel->getWorldBounds();
//
//    sf::Vector2f pos = getWorldPosition();
//
//    pos.x = std::min(pos.x, static_cast<float>(worldBounds.width));
//    pos.x = std::max(pos.x, 0.f);
//    pos.y = std::min(pos.y, static_cast<float>(worldBounds.height));
//    pos.y = std::max(pos.y, 0.f);
//
//    setPosition(pos);
//}

void Entity::ensureZeroOverlap()
{
    std::list<Entity*> neighbours;
    getNeighbours(neighbours,
                  Entity::Type::AllTypes);

  //std::list<Entity*> neighbours = mLevel->getEntitiesInRange(const_cast<Entity*>(this),
  //                                                           mRadius);

  sf::Vector2f pos = getWorldPosition();
  float radius = getRadius();

  for(Entity* e : neighbours)
  {
    if(e != this)
    {
      sf::Vector2f awayFromEntity = pos - e->getWorldPosition();
      float expandedRadius = radius + e->getRadius();
      float mag = magVec(awayFromEntity);

      if(mag < expandedRadius)
      {
        move(normVec(awayFromEntity));
      }
    }
  }
}

std::list<Entity*>& Entity::getNeighbours(std::list<Entity*>& returnList,
                                          Entity::Type type) const
{
    //return mLevel->getEntitiesInRange(const_cast<Entity*>(this)
    //                                , mRadius);

  return mQuadTree->retrieveEntities(returnList,
                                     this,
                                     type);
}

std::list<Scenery*>& Entity::getObstacles(std::list<Scenery*>& returnList,
                                         Scenery::Type type) const
{
  return mQuadTree->retrieveScenery(returnList,
                                     this,
                                     type);
}

void Entity::addToQuadTree(QuadTree* quadTree)
{
  // Add children
  SceneNode::addToQuadTree(quadTree);

//  std::cout << "adding entity to quad tree" << std::endl;

  quadTree->insert(this);
}

//std::vector<LevelBlock*> Entity::getBlockTypeInRange(LevelBlock::Type blockType, float radius) const
//{
////    return mLevel->getBlockTypeInRange(const_cast<Entity*>(this), radius, blockType);
//}

//LevelBlock* Entity::getLevelBlock(sf::Vector2i index)
//{
////    return mLevel->getBlock(index);
//}

//std::vector<LevelBlock*> Entity::getLevelExit()
//{
//    return mLevel->getLevelExit();
//}

//sf::Transform Entity::getWorldTransform() const
//{
//   b2Transform b2dTrans = mPhysicsBody->GetTransform();
//   b2Vec2 pos = b2dTrans.p;
//
//   sf::Transform sfTrans;
//   sfTrans.rotate(b2dTrans.q.GetAngle()).translate(pos.x, pos.y);
//
//   return sfTrans;
//}
//
//sf::Vector2f Entity::getWorldPosition() const
//{
//    return convertVec(mPhysicsBody->GetPosition());
//}
