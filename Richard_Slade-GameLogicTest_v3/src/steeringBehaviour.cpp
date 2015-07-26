#include <cmath>
#include <algorithm>
#include <iostream>
#include <cassert>

#include "App/Utility.hpp"
#include "App/Params.hpp"
#include "World/Scenery.hpp"
#include "Entity/Entity.hpp"
#include "Entity/SteeringBehaviour.hpp"

SteeringBehaviour::SteeringBehaviour(Entity* host
                                     , const Params& params)
: mWanderRadius(params.WanderRadius)
, mWanderDistance(params.WanderDistance)
, mWanderJitter(params.WanderJitter)
, mMinViewBoxLength(params.MinViewBoxLength)
, mInteractionRadius(params.InteractionRadius)
, mFeelerLength(params.FeelerLength)
, mMinArriveDist(params.MinArriveDist)
, mWallAvoidanceMultiplier(params.WallAvoidanceMultiplier)
, mObstacleAvoidanceMultiplier(params.ObstacleAvoidanceMultiplier)
, mArriveMultiplier(params.ArriveMultiplier)
, mEvadeMultiplier(params.EvadeMultiplier)
, mWanderMultiplier(params.WanderMultiplier)
, mSeperationMultiplier(params.SeperationMultiplier)
, mAlignmentMultiplier(params.AlignmentMultiplier)
, mCohesionMultiplier(params.CohesionMultiplier)
, mFlockingMultiplier(params.FlockingMultiplier)
, mSeperationRadius(params.SeperationRadius)
, mAlignRadius(params.AlignRadius)
, mCohesionRadius(params.CohesionRadius)
, mHost(host)
, mWanderTarget(std::sin(mTheta) * mWanderRadius, -std::cos(mTheta) * mWanderRadius)
{
   for(bool& b : mBehaviourFlags)
     b = false;

   //if (mHost->getEntityType() == Entity::Type::Adventurer)
    //mBehaviourFlags.at(SteeringBehaviour::Seperation) = true;
};

bool SteeringBehaviour::accumulateForce(sf::Vector2f& runningTotal, sf::Vector2f forceToAdd){
    float magSoFar = magVec(runningTotal);

    float magRemaining = mHost->getMaxForce() - magSoFar;

    if(magRemaining <= 0.f) return false;

    double magToAdd = magVec(forceToAdd);

    if(magToAdd < magRemaining)
    {
        runningTotal += forceToAdd;
    }
    else
    {
        runningTotal += normVec(forceToAdd) * magRemaining;
    }

    return true;
}

void SteeringBehaviour::createFeelers(){
    mFeelers.clear();

    mFeelers.push_back(sf::Vector2f(std::sin(degreeToRadian(-90.f)) * mFeelerLength
                                      , std::cos(degreeToRadian(-90.f)) * -mFeelerLength));

    mFeelers.push_back(sf::Vector2f(std::sin(degreeToRadian(-45.f)) * mFeelerLength
                                      , std::cos(degreeToRadian(-45.f)) * -mFeelerLength));

    mFeelers.push_back(sf::Vector2f(0.f
                                      , -mFeelerLength));

    mFeelers.push_back(sf::Vector2f(std::sin(degreeToRadian(45.f)) * mFeelerLength
                                      , std::cos(degreeToRadian(45.f)) * -mFeelerLength));

    mFeelers.push_back(sf::Vector2f(std::sin(degreeToRadian(90.f)) * mFeelerLength
                                      , std::cos(degreeToRadian(90.f)) * -mFeelerLength));
}

sf::Vector2f SteeringBehaviour::rest(){
   sf::Vector2f steeringForce, hostVel = mHost->getVelocity();

   steeringForce = -hostVel;

   return steeringForce;
}

sf::Vector2f SteeringBehaviour::face(){
   sf::Vector2f steeringForce;

   steeringForce = mHost->getCurrentTarget()->getWorldPosition() - mHost->getWorldPosition();

   return steeringForce;
}

sf::Vector2f SteeringBehaviour::arrive(sf::Vector2f targ, Deceleration deceleration){
   sf::Vector2f toTarget = targ - mHost->getWorldPosition();

   float dist = magVec(toTarget);

   sf::Vector2f steeringForce;

   if(dist > mMinArriveDist)
   {
      const float DecelerationTweaker = 0.3;

      float speed = dist / (static_cast<float>(deceleration) * DecelerationTweaker);

      speed = std::min(speed, mHost->getMaxSpeed());

      sf::Vector2f desiredVelocity = toTarget;

      steeringForce = toTarget;
   }

   return steeringForce;
}

sf::Vector2f SteeringBehaviour::seek(sf::Vector2f target){
    sf::Vector2f desiredVelocity = normVec(target - mHost->getWorldPosition()) * mHost->getMaxSpeed();

    return (desiredVelocity - mHost->getVelocity());
}

sf::Vector2f SteeringBehaviour::followPath(){
    if(mPath.isActive())
    {
        sf::Vector2f toWaypoint = mHost->getPosition() - mPath.currentWaypoint();

        float mag = magVec(toWaypoint);
        float sqMag = mag * mag;
        float sqDistToWypnt = 20.f * 20.f;

        if(sqMag < sqDistToWypnt)
        {
            mHost->setVelocity(sf::Vector2f());

            if(!mPath.nextWaypoint())
                return rest();
        }

        if(!mPath.isEnd())
        {
            return seek(mPath.currentWaypoint());
        }
        else
        {
            return arrive(mPath.currentWaypoint(), Deceleration::Fast);
        }
    }
    else
    {
        return rest();
    }
}

sf::Vector2f SteeringBehaviour::obstacleAvoidance(){
  float boxLength = mMinViewBoxLength
                + (mHost->getSpeed() / mHost->getMaxSpeed())
                * mMinViewBoxLength;

  std::vector<Scenery*> obstacles;

  obstacles = mHost->getObstacles();

  Scenery* closestObstacle = nullptr;

  float distToClosest = MAXFLOAT;

  sf::Transform hostTrans = mHost->getWorldTransform().getInverse();

  std::vector<Scenery*>::iterator iter;

  for(iter = obstacles.begin();
    iter != obstacles.end();
    iter++)
  {
    sf::Vector2f obsPos = hostTrans * (*iter)->getWorldPosition();

    if(obsPos.y <= 0.f)
    {
      float expandedRadius = (*iter)->getRadius() + mHost->getRadius();

      if(std::fabs(obsPos.x) < expandedRadius)
      {
        float sqrtPart = std::sqrt((expandedRadius * expandedRadius) - (obsPos.x * obsPos.x));
        float intersectionPoint = obsPos.y - sqrtPart;

        if(intersectionPoint <= 0.f)
          intersectionPoint = obsPos.y + sqrtPart;


        if(intersectionPoint < distToClosest)
        {
          distToClosest = intersectionPoint;
          closestObstacle = *iter;
        }
      }
    }
  }

  sf::Vector2f steeringForce;

  if(closestObstacle)
  {
    sf::Vector2f closestPos = hostTrans * closestObstacle->getWorldPosition();

    float multiplier = 1.f + (boxLength - closestPos.y) / boxLength;

    steeringForce.x = (closestObstacle->getRadius() - closestPos.x);;

    const float brakingWeight = 0.02;

    steeringForce.y = (closestObstacle->getRadius() - closestPos.y);

  }

  sf::Vector2f worldForce = mHost->getWorldTransform().transformPoint(steeringForce);

  return worldForce - mHost->getWorldPosition();
}

sf::Vector2f SteeringBehaviour::calculate(sf::Time dt){
  sf::Vector2f steeringForce;

  if(mBehaviourFlags.at(SteeringBehaviour::Behaviour::Rest))
  {
    sf::Vector2f force = rest();

    if(!accumulateForce(steeringForce, force))
        return steeringForce;
  }

  if(mBehaviourFlags.at(SteeringBehaviour::Behaviour::ObstacleAvoidance))
  {
    sf::Vector2f force = obstacleAvoidance() * 0.1f;

    if(!accumulateForce(steeringForce, force))
      return steeringForce;
  }

  if(mBehaviourFlags.at(SteeringBehaviour::Behaviour::Arrive))
  {
    sf::Vector2f force = arrive(mHost->getCurrentTarget()->getWorldPosition(), Deceleration::Fast) * mArriveMultiplier;

    if(!accumulateForce(steeringForce, force))
        return steeringForce;
  }

 if(mBehaviourFlags.at(SteeringBehaviour::Behaviour::Seek))
  {
    sf::Vector2f force = seek(mHost->getCurrentTarget()->getWorldPosition()) * mArriveMultiplier;

    if(!accumulateForce(steeringForce, force))
        return steeringForce;
  }

  if(mBehaviourFlags.at(SteeringBehaviour::Behaviour::FollowPath))
  {
    sf::Vector2f force = followPath() * 1.f;

    if(!accumulateForce(steeringForce, force))
        return steeringForce;
  }

  if(mBehaviourFlags.at(SteeringBehaviour::Behaviour::Face))
  {
    sf::Vector2f force = face();

    if(!accumulateForce(steeringForce, force))
        return steeringForce;
  }

  return steeringForce;
}

void SteeringBehaviour::setNewBehaviours(std::vector<SteeringBehaviour::Behaviour> newTypes){
    for(bool& b : mBehaviourFlags)
        b = false;

    for(SteeringBehaviour::Behaviour& bh : newTypes)
        mBehaviourFlags.at(bh) = true;

    //if (mHost->getEntityType() == Entity::Type::Adventurer)
    //mBehaviourFlags.at(SteeringBehaviour::Seperation) = true;
}

void SteeringBehaviour::setNewBehaviours(SteeringBehaviour::Behaviour newType){
    for(bool& b : mBehaviourFlags)
        b = false;

    mBehaviourFlags.at(newType) = true;

    //if (mHost->getEntityType() == Entity::Type::Adventurer)
      //mBehaviourFlags.at(SteeringBehaviour::Seperation) = true;
}

