#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <memory>
#include <list>
#include <array>

#include <SFML/Graphics/Rect.hpp>

//#include "World/Scenery.hpp"
//#include "Entity/Entity.hpp"

class Scenery;
class Entity;
class SceneNode;
//class PhysicsBody;

class QuadTree
{
public:
  typedef std::unique_ptr<QuadTree> upQuadTree;

  enum ObjectType
  {
    SceneryObjects,
    EntityObjects,
    AllObjects
  };

private:
  const int                             mMaxObjects = 20;
  const int                             mMaxLevels = 1;

  int                                   mLevel;
  std::list<Scenery*>                   mScenery;
  std::list<Entity*>                    mEntities;
//  bool                                  mHasSplit;

  sf::FloatRect                         mBounds;
  std::array<QuadTree*, 4>   mChildren;
  //std::array<QuadTree::upQuadTree, 4>   mChildren;
//  std::array<int, 4>   mChildren;
  //std::vector<QuadTree*>     mChildren;


  void                                  split();
  void                                  checkForSplit();
  void                                  distributeObjects();
  int                                   getIndex(const SceneNode* body) const;

public:
                                        QuadTree();

                                        QuadTree(int level,
                                               sf::FloatRect bounds);

                                        ~QuadTree();

  void                                  insert(Scenery* scenery);
  void                                  insert(Entity* entity);

  std::list<Scenery*>&                  retrieveScenery(std::list<Scenery*>& returnObjects,
														                            const Entity* body,
                                                        unsigned int type) const;

  std::list<Entity*>&                   retrieveEntities(std::list<Entity*>& returnObjects,
														                             const Entity* body,
                                                         unsigned int type) const;

  void                                  clear();
};

#endif // QUADTREE_HPP
