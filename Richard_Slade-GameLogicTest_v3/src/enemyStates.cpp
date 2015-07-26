#include <cassert>
#include <iostream>
#include <cstdlib>

#include "App/Utility.hpp"
#include "World/World.hpp"
#include "Entity/State/EnemyStates.hpp"
#include "Entity/SteeringBehaviour.hpp"
#include "Entity/Entity.hpp"

using namespace EnemyStates;

void LookOut::enter(Enemy* host)
{
  assert(host);
}

void LookOut::execute(Enemy* host)
{

  if (host->getCurrentStateType() == Enemy::States::Relax)
  {
    Entity* player = host->getCurrentTarget();
    sf::Vector2f hostPos = host->getWorldPosition();

    if (player)
    {
      sf::Vector2f vecToEnemy = hostPos - player->getWorldPosition();
      float mag = magVec(vecToEnemy);

      if (mag < host->mAggroDistance)
      {
        host->changeState(Enemy::States::Attack);
      }
    }
  }
}

void LookOut::exit(Enemy* host)
{

}

void Relax::enter(Enemy* host)
{
  assert(host);

  if (!host->checkSteeringBehaviour(SteeringBehaviour::Behaviour::Rest))
  {
    std::vector<SteeringBehaviour::Behaviour> behaviours;
    behaviours.push_back(SteeringBehaviour::Behaviour::Rest);

    host->setSteeringTypes(behaviours);
  }

  host->setText("Grr");
  host->setMaxSpeed(host->getMaxWalkSpeed());
}

void Relax::execute(Enemy* host)
{

}

void Relax::exit(Enemy* host)
{

}

void Attack::enter(Enemy* host)
{
  assert(host);

  if (!host->checkSteeringBehaviour(SteeringBehaviour::Behaviour::Arrive))
  {
    std::vector<SteeringBehaviour::Behaviour> behaviours;
    behaviours.push_back(SteeringBehaviour::Behaviour::Arrive);
    host->setSteeringTypes(behaviours);
  }

  host->setText("Arrggh!");
  host->setMaxSpeed(host->getMaxRunSpeed());
}

void Attack::execute(Enemy* host)
{
  Entity* player = host->getCurrentTarget();
  //   Adventurer* curTarg = nullptr;

  //  std::cout << "Attack execute()" << std::endl;

  if (!player
    || player->isDead())
    host->changeState(Enemy::States::Relax);

  int mag = magVec(host->getWorldPosition() - player->getWorldPosition());

  if (mag < 40.f
    && !player->hasLostLife())
  {
    player->decreaseLives();
    host->decreaseLives();
    host->decEnemies();
  }
  else if (mag < host->mAggroDistance) // If there is current target
  {
    if (!host->checkSteeringBehaviour(SteeringBehaviour::Behaviour::Seek))
    {
      std::vector<SteeringBehaviour::Behaviour> behaviours;
      behaviours.push_back(SteeringBehaviour::Behaviour::Seek);
      host->setSteeringTypes(behaviours);
    }

    host->setText("arRRGHHH");
  }
  else // If no target
  {
    host->changeState(Enemy::States::Relax);
  }
}

void Attack::exit(Enemy* host)
{

}

