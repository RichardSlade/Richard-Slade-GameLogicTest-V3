#ifndef ADVENTURER_HPP
#define ADVENTURER_HPP

/*
*   Class for Adventurer controlled by player
*
*   @Author Richard Slade
*   @Date 12/2014
*/

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window.hpp>

#include "Entity/Entity.hpp"
#include "Entity/Path.hpp"

class Adventurer : public Entity
{
private:
  const sf::RenderWindow&        mWindow;

  sf::Time                       mFlashTime;
  sf::Time                       mFlashCounter;

  virtual void                    updateCurrent(sf::Time);
  virtual void                    drawCurrent(sf::RenderTarget&
    , sf::RenderStates) const;

public:
  Adventurer(const sf::RenderWindow& window
    , World* world
    , const sf::Texture&
    , const sf::Font&
    , sf::Vector2f
    , EntityStats
    , const Params&
    , float scale = 1.f);

  virtual                          ~Adventurer(){};

  void                             addToPath(sf::Vector2f pos)
  {
    mSteering.addToPath(pos);
  }

  void                             startNewPath(sf::Vector2f pos)
  {
    mSteering.startNewPath(pos);
  }
};

#endif // ADVENTURER_HPP
