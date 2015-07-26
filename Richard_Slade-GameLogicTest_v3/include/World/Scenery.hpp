#ifndef SCENERY_HPP
#define SCENERY_HPP

#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "App/Utility.hpp"
#include "SceneNode/SceneNode.hpp"
//#include "SceneNode/SpriteNode.hpp"

//class LevelBlock;

class Scenery : public SceneNode
{
public:
    typedef std::unique_ptr<Scenery> upScenery;

    enum Type
    {
      Wall,
      Trap,
      All
    };

protected:
  Scenery::Type                 mSceneryType;
//    const LevelBlock*           mHostBlock;
    sf::Sprite                  mSprite;
    float                       mRadius;


    virtual void                updateCurrent(sf::Time dt);

    virtual void                drawCurrent(sf::RenderTarget& target,
                                              sf::RenderStates states) const;

public:
                                Scenery(const sf::Texture& texture,
                                        sf::Vector2f pos,
                                        sf::IntRect spriteRect,
                                        sf::Vector2f worldSize,
                                        Scenery::Type type);

                                Scenery(const sf::Texture& texture,
                                        sf::Vector2f pos,
                                        float radius,
                                        Scenery::Type type);

    virtual                     ~Scenery(){};

    //virtual void                addToQuadTree(QuadTree* quadTree);

    Scenery::Type               getSceneryType() {return mSceneryType;}

    float                       getRadius() { return mRadius; }
    sf::FloatRect               getBounds() { return mSprite.getGlobalBounds(); }

    // Getters
//    virtual void                getSceneryData() const {};

//    const sf::Sprite&           getSprite()
//                                { return mSprite; }

    // Setters
//    void                        rotateSprite(float angle) {mSprite.rotate(angle);}
};

#endif // SCENERY_HPP
