#ifndef KILLABLE_HPP
#define KILLABLE_HPP

#include "Entity/EntityStats.hpp"

struct Killable
{
protected:
   int                        lives;
   bool                       lostLife;
   bool                       dead;

public:
                              Killable(int lp)
                              : lives(lp)
                              , lostLife(false)
                              , dead(false)
                              {};

   int                        getLives() const { return lives; }
   bool                       hasLostLife() const { return lostLife; }
   bool                       isDead() const { return dead; }

   void                       decreaseLives()
                              {
                                lives--;
                                lostLife = true;

                                if (lives < 0)
                                {
                                  dead = true;
                                }
                              }

};

#endif // KILLABLE_HPP
