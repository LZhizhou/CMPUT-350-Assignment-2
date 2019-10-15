#include "Marine.h"

// ... implement

  Marine::Marine(Team team, const Vec2 &pos, AttackPolicy pol, bool bounce):
  Unit(team,pos,10,40,10,1,45),policy(pol),bounce(bounce)
  {  
    
  };
  
  // execute one attack depending on policy
  // when attacking you need to call w.attack
  void Marine::act(World &w){
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
  void Marine::collision_hook(double prev_speed, bool collisions[4]){
    if(bounce){
      current_speed = prev_speed;
      if(collisions[RIGHT]||collisions[LEFT]){
        heading.x = -heading.x;
        
      }
      if(collisions[TOP]||collisions[BOTTOM]){
        heading.y = -heading.y;

        
      }
    }
  };
