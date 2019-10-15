#include "Tank.h"

// ... implement

  Tank::Tank(Team team, const Vec2 &pos, AttackPolicy pol, bool bounce):
  Unit(team,pos,20,80,15,4,100),policy(pol),bounce(bounce)
  {  };
  
  // execute one attack depending on policy
  // when attacking you need to call w.attack
  void Tank::act(World &w){
    Unit *attacked;
    switch (policy){
      case ATTACK_WEAKEST:
        attacked = w.random_weakest_target(*this);
        if(attacked!=0){
          w.attack(*this,*attacked);
        }
        
        break;
      case ATTACK_CLOSEST:
        attacked = w.random_closest_target(*this);
        if(attacked!=0){
          w.attack(*this,*attacked);
        }
        break;
      case ATTACK_MOST_DANGEROUS:
        attacked = w.random_most_dangerous_target(*this);
        if(attacked!=0){
          w.attack(*this,*attacked);
        }
        break;
      default:
        break;
    }
  };

  // deal with wall collisions depending on bounce
  void Tank::collision_hook(double prev_speed, bool collisions[4]){
    if(bounce){
      if(collisions[RIGHT]||collisions[LEFT]){
        current_speed = prev_speed;
        heading.x = -heading.x;
      }
      if(collisions[TOP]||collisions[BOTTOM]){
        current_speed = prev_speed;
        heading.y = -heading.y;
      }
    }
  };