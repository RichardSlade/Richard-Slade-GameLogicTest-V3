#ifndef ENTITYSTATS_HPP
#define ENTITYSTATS_HPP

/*
*   EntityStats.hpp
*
*   Class holding parameters of different entity types in game
*   one for each type
*
*   @Author Richard Slade
*   @Date 12/2014
*/

#include <fstream>
#include <string>
#include <stdexcept>

struct EntityStats
{
public:
  float       mass;
  float       walkMaxSpeed;
  float       runMaxSpeed;
  float       maxForce;
  float       maxTurnRate;
  float       lives;
  float       aggroDistance;

  EntityStats(std::string fileName)
  {
      std::ifstream in(fileName);

      if(!in.is_open())
          throw std::runtime_error("ERROR: Could not open file" + fileName);

      in >> *this;
      in.close();
  }

  // Extractor
  friend void operator>>(std::ifstream &in, EntityStats &stats)
  {
      in.ignore(255, ' ');
      in >> stats.mass;

      in.ignore(255, ' ');
      in >> stats.walkMaxSpeed;

      in.ignore(255, ' ');
      in >> stats.runMaxSpeed;

      in.ignore(255, ' ');
      in >> stats.maxForce;

      in.ignore(255, ' ');
      in >> stats.maxTurnRate;

      in.ignore(255, ' ');
      in >> stats.lives;

      in.ignore(255, ' ');
      in >> stats.aggroDistance;
  }

};

#endif // ENTITYSTATS_HPP
