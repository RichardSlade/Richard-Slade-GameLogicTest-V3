#include <cassert>
#include <iostream>
#include <cstdlib>

#include "App/Utility.hpp"
#include "World/World.hpp"
//#include "Entity/Adventurer.hpp"
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

  if(host->getCurrentStateType() == Enemy::States::Relax)
  {
    std::list<Entity*> chars;

    //chars = host->getNeighbours();

    //host->getNeighbours(chars
    //                    , Entity::Type::Adventurer);

    //std::cout << "charSize: " << chars.size() << std::endl;

    sf::Vector2f hostPos = host->getWorldPosition();

    Entity* closestChar = nullptr;
    float closestDist = 9999.f;

    // Loop thorugh enemies to find closest
    for(Entity* e : chars)
    {
       sf::Vector2f vecToEnemy = hostPos - e->getWorldPosition();
       float mag = magVec(vecToEnemy);

       if(mag < closestDist)
       {
          closestChar = e;
          closestDist = mag;
       }
    }

    // If host target exists set it as target
    // and change state accordingly to health
    if(closestChar)
    {
      if (closestDist < host->mAggroDistance)
      {
        host->changeState(Enemy::States::Attack);
        host->setCurrentTarget(closestChar);
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

    //if(!host->checkSteeringBehaviour(SteeringBehaviour::Behaviour::Wander))
    //{
    //    std::vector<SteeringBehaviour::Behaviour> behaviours;
    //    behaviours.push_back(SteeringBehaviour::Behaviour::Wander);

    //    host->setSteeringTypes(behaviours);
    //}

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

    if(!host->checkSteeringBehaviour(SteeringBehaviour::Behaviour::Arrive))
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
   Entity* curTarg = host->getCurrentTarget();
//   Adventurer* curTarg = nullptr;

//  std::cout << "Attack execute()" << std::endl;

   if(!curTarg
     || curTarg->isDead())
     host->changeState(Enemy::States::Relax);

   int mag = magVec(host->getWorldPosition() - curTarg->getWorldPosition());

   if (mag < 10.f)
   {
     curTarg->decreaseLives();
     host->decreaseLives();
   }
   else if(mag < host->mAggroDistance) // If there is current target
   {
      if(!host->checkSteeringBehaviour(SteeringBehaviour::Behaviour::Seek))
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

