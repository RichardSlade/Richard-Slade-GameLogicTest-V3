#ifndef MOVINGENTITY_HPP
#define MOVINGENTITY_HPP

#include <list>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "App/Params.hpp"
#include "App/Utility.hpp"
#include "World/Scenery.hpp"
#include "SceneNode/SceneNode.hpp"
#include "Entity/SteeringBehaviour.hpp"
#include "Entity/EntityStats.hpp"
#include "Entity/Attribute/Killable.hpp"

class World;

class Entity : public SceneNode, public Killable
{
public:
  enum Type
  {
    Adventurer,
    Enemy,
    AllTypes
  };

protected:
  World*                      mWorld;

  const float                 mMass;
  const float                 mWalkMaxSpeed;
  const float                 mRunMaxSpeed;
  const float                 mMaxForce;
  const float                 mMaxTurnRate;

  float                       mMaxSpeed;
  Type                        mEntityType;

  sf::Sprite                  mSprite;
  sf::Vector2f                mVelocity;
  sf::Vector2f                mHeading;

  float                       mRadius;
  float                       mSpeed;

  SteeringBehaviour           mSteering;
  Entity*                     mCurrentTarget;
  sf::Text                    mText;
  sf::CircleShape               mWanderTarget;

  virtual void                updateCurrent(sf::Time);
  virtual void                drawCurrent(sf::RenderTarget& target
    , sf::RenderStates states) const;

  void                        adjustPosition();
  void                        ensureZeroOverlap();

public:
  Entity(World* world

    , const sf::Texture&
    , const sf::Font&
    , sf::Vector2f
    , EntityStats
    , const Params&
    , Type type
    , float = 1.f);

  virtual                     ~Entity(){};

  std::vector<Scenery*>           getObstacles() const;

  sf::Vector2f                  getVelocity() const { return mVelocity; }

  sf::Vector2f                  getHeading() const { return mHeading; }

  float                         getSpeed() const { return magVec(mVelocity); }

  float                         getMass() const { return mMass; }

  float                         getMaxSpeed() const { return mMaxSpeed; }

  float                         getMaxWalkSpeed() { return mWalkMaxSpeed; }

  float                         getMaxRunSpeed() { return mRunMaxSpeed; }

  float                         getMaxForce() const { return mMaxForce; }

  float                         getMaxTurnRate() const { return mMaxTurnRate; }

  bool                          checkSteeringBehaviour(SteeringBehaviour::Behaviour type) const { return mSteering.checkBehaviour(type); }

  float                         getRadius() const { return mRadius; }

  sf::FloatRect               getBounds() const { return mSprite.getGlobalBounds(); }

  float                         getSpriteWidth() const { return mSprite.getLocalBounds().width; }

  const Entity*                 getCurrentTarget() const { return mCurrentTarget; }
  Entity*                        getCurrentTarget() { return mCurrentTarget; }

  Entity::Type                  getEntityType(){ return mEntityType; }

  // Setters
  void                          setVelocity(sf::Vector2f newVel){ mVelocity = newVel; }

  void                          resetVelocity()
  {
    mVelocity = sf::Vector2f();
  }

  void                       changeVelocity(sf::Vector2f newVec)
  {
    mVelocity += newVec;
  }

  void                       setCurrentTarget(Entity* target)
  {
    mCurrentTarget = target;
  }

  void                       setMaxSpeed(float speed)
  {
    mMaxSpeed = speed;
  }

  void                       setSteeringTypes(std::vector<SteeringBehaviour::Behaviour> newBehaviours)
  {
    mSteering.setNewBehaviours(newBehaviours);
  }

  void                       setSteeringTypes(SteeringBehaviour::Behaviour newBehaviour)
  {
    mSteering.setNewBehaviours(newBehaviour);
  }

  void                       changeColour(sf::Color colour)
  {
    mSprite.setColor(colour);
  }

  void                       setText(std::string msg)
  {
    mText.setString(msg);
    mText.setColor(sf::Color(255, 255, 255, 255));
  }

  void                          setWanderTargetPosition(sf::Vector2f pos) { mWanderTarget.setPosition(pos); }

  void                          decEnemies();
};

#endif // MOVINGENTITY_HPP
