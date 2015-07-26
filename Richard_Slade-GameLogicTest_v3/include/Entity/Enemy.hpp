#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <memory>

#include "Entity/Entity.hpp"
#include "Entity/State/StateMachine.hpp"

class Adventurer;

class Enemy : public Entity
{
public:
  typedef std::vector<std::unique_ptr<State<Enemy>>> StateContainer;

  enum States
  {
    LookOut,
    Relax,
    Attack,
    NumStates
  };

  const unsigned int          mAggroDistance;

private:
  StateContainer&             mStates;
  StateMachine<Enemy>         mStateMachine;

  virtual void                updateCurrent(sf::Time);
  virtual void                drawCurrent(sf::RenderTarget&
    , sf::RenderStates) const;

public:
  Enemy(World* world
    , const sf::Texture&
    , const sf::Font&
    , sf::Vector2f
    , EntityStats
    , const Params&
    , State<Enemy>*
    , State<Enemy>*
    , StateContainer&
    , unsigned int currentState
    , float = 1.f);

  virtual                    ~Enemy(){};

  void                        changeState(Enemy::States newState)
  {
    mStateMachine.changeState(mStates.at(newState).get(), newState);
  }

  // Setters
  void                        setVelocity(sf::Vector2f vel)
  {
    mVelocity = vel;
  }

  void                        setText(std::string msg)
  {
    mText.setString(msg);
    mText.setColor(sf::Color(255, 255, 255, 255));
  }

  unsigned int                getCurrentStateType() { return mStateMachine.getCurrentStateType(); }

};

#endif // ENEMY_HPP
