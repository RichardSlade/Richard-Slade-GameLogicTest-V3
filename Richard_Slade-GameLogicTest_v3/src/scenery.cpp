#include <cmath>

#include "World/Scenery.hpp"

Scenery::Scenery(const sf::Texture& texture,
  sf::Vector2f pos,
  sf::IntRect spriteRect,
  sf::Vector2f worldSize,
  Scenery::Type type)
  : mSceneryType(type)
  , mSprite(texture,
  spriteRect)
{
  sf::FloatRect bounds = mSprite.getLocalBounds();
  mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
  sf::Transformable::setPosition(pos);
  mRadius = std::sqrt((mSprite.getLocalBounds().width * mSprite.getLocalBounds().width)
    + (mSprite.getLocalBounds().height * mSprite.getLocalBounds().height));
};

Scenery::Scenery(const sf::Texture& texture,
  sf::Vector2f pos,
  float radius,
  Scenery::Type type)
  : mSceneryType(type)
  , mSprite(texture)
{
  sf::FloatRect bounds = mSprite.getLocalBounds();
  mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
  sf::Transformable::setPosition(pos);
  mRadius = radius;
};

//void Scenery::addToQuadTree(QuadTree* quadTree)
//{
//  // Add children
//  SceneNode::addToQuadTree(quadTree);
//
//  quadTree->insert(this);
//}

void Scenery::updateCurrent(sf::Time dt)
{
  sf::Transformable::setPosition(getWorldPosition());
}

void Scenery::drawCurrent(sf::RenderTarget& target,
  sf::RenderStates states) const
{
  target.draw(mSprite, states);
}
