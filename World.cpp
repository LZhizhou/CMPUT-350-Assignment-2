#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "World.h"
#include "Unit.h"

using namespace std;

/* helper that you need to implement
   
 - move unit by (current_speed * heading)

 - units only collide with the map border
 
 - when a unit hits a wall it stops at the collision point (current_speed=0)
   and collision_hook needs to be called with the previous speed and an array
   of bools encoding all the walls that were hit (the array indexes need to be
   consistent with enum CollDir). This array is used to implement bouncing by
   only changing the unit's heading, i.e. even when bouncing the unit stays at
   the collision location for the remainder of the simulation frame and only
   starts moving again in the following frame

 - it is essential that units are located inside the map at all times. There
   can be no overlap. Ensure this property by clipping coordinates after
   moving

*/

void World::move_unit(Unit &u)

{
  // get res of current_speed * heading
  double new_x = u.pos.x+u.heading.x * u.current_speed;
  double new_y = u.pos.y+u.heading.y * u.current_speed;

  // hit_wall shows if hit a wall, wall_array shows which wall the unit hits
  bool hit_wall(false), wall_array[4] = {0};
  // left wall and moving left
  if (new_x-u.radius < 0&&u.heading.x <0)
  {
    
    u.pos.x = u.radius;
    hit_wall = true;
    wall_array[LEFT] = true;
  }
  // right wall and moving right
  else if (new_x+u.radius > width&&u.heading.x >0)
  {
    
    u.pos.x = width-u.radius;
    hit_wall = true;
    wall_array[RIGHT] = true;
  }
  else
  {
    // not hit
    u.pos.x = new_x;
  }
  // top wall and moving top
  if (new_y-u.radius < 0&&u.heading.y<0)
  {
    
    u.pos.y = u.radius;
    hit_wall = true;
    wall_array[TOP] = true;
  }
  // bottom wall and moving down
  else if (new_y+u.radius > height&&u.heading.y>0)
  {
    u.pos.y = height-u.radius;
    hit_wall = true;
    wall_array[BOTTOM] = true;
  }
  else
  {
    // not hit
    u.pos.y = new_y;
  }
  
  if (hit_wall)
  {
    int pre_speed(u.current_speed);
    u.current_speed = 0;
    u.collision_hook(pre_speed, wall_array);
    
  }

  
  // ... implement
}

// returns policy name
const char *apol2str(AttackPolicy pol)
{
  switch (pol)
  {
  case ATTACK_WEAKEST:
    return "attack-weakest";
  case ATTACK_CLOSEST:
    return "attack-closest";
  case ATTACK_MOST_DANGEROUS:
    return "attack-most-dangerous";
  }
  return "?";
}

World::~World()
{
  // clean up
  for(Unit* it: units){
    delete it;
  }
  units.clear();
  // ... implement
}

// deducts hit points and calls attack_hook

void World::attack(Unit &attacker, Unit &attacked)
{
  // ... implement
  attacked.hp-=attacker.damage;
  attack_hook(attacker, attacked);
}

// return a random position at which a circle of that radius fits

Vec2 World::rnd_pos(double radius) const
{
  double slack = radius * 2;

  Vec2 p((width - 2 * slack) * rnd01() + slack,
         (height - 2 * slack) * rnd01() + slack);

  // assert circle in rectangle
  assert(p.x > radius && p.x < width - radius);
  assert(p.y > radius && p.y < height - radius);
  return p;
}

// return uniform random heading
// length of vector = 1

Vec2 World::rnd_heading() const
{
  // ... implement
  
  double a(rnd01());
  
  // geomtry 101
  // Radian of angle (0,2pi)
  // cout<<"a:"<<2*M_PI*a<<","<<cos(2*M_PI/a)<<","<<sin(2*M_PI/a)<<endl;
  return Vec2 (cos(2*M_PI*a), sin(2*M_PI*a));
}

// mirror position with respect to map mid-point

Vec2 World::mirror(const Vec2 &pos) const
{
  // ... implement
  
  return Vec2 (width - pos.x,height - pos.y);
}

// return squared distance between two unit centers

double World::distance2(const Unit &u, const Unit &v)
{
  // ... implement
  return square(u.pos.x - v.pos.x) + square(u.pos.y - v.pos.y);
}

// return true iff u can attack v, i.e. distance of u's and v's centers is
// less than u's attack distance plus v's radius
bool World::can_attack(const Unit &u, const Unit &v)
{
  // ... implement
  return u.team!=v.team && distance2(u, v) < square(u.attack_radius) + square(v.radius);
}

// populate a vector with enemy units that can be attacked by u;
// clears vector first
void World::enemies_within_attack_range(const Unit &u,
                                        vector<Unit *> &targets) const
{
  targets.clear();

  // ... implement
  for (Unit* it : units)
  {
    
    if (can_attack(u, *it))
    {
      targets.push_back(it);
    }
  }
  // use push_back to add elements to targets
}

// return a random unit that can be attacked by u with minimal hp_old value,
// or 0 if none exists

Unit *World::random_weakest_target(Unit &u) const
{
  vector<Unit *> targets;
  enemies_within_attack_range(u,targets);

  Unit *temp = 0;
  for (Unit* it : targets)
  {
    if (!temp||temp->hp_old > it->hp_old)
    {
      temp = it;
    }
  }
  return temp;
  // ... implement
}

// return a random unit that can be attacked by u with minimal distance to
// u, or 0 if none exists

Unit *World::random_closest_target(Unit &u) const
{
  vector<Unit *> targets;
  enemies_within_attack_range(u,targets);
  Unit *temp = 0;
  double dis = -1;
  for (Unit* it : targets)
  {
    double curr_dis = distance2(u,*it);
    if (!temp||dis > curr_dis)
    {
      dis = curr_dis;
      temp = it;
    }
  }
  return temp;
  // ... implement
}

// return a random unit that can be attacked by u with maximal damage/hp_old
// ratio, or 0 if none exists

Unit *World::random_most_dangerous_target(Unit &u) const
{
  vector<Unit *> targets;
  enemies_within_attack_range(u,targets);
  Unit *max_damage = 0, *max_hp = 0;
  int hp = 0;
  double damage = 0;
  for (Unit* it : targets)
  {
    int curr_hp = it->hp_old;
    double curr_damage = it->damage;
    if (!max_damage||curr_damage > damage)
    {
      max_damage = it;
      damage = curr_damage;
    }
    if (!max_hp||curr_hp > hp)
    {
      max_hp = it;
      hp = curr_hp;
    }
  }
  if(max_damage&&max_hp){
    if(rnd_int(2)){
      return max_hp;
    }else{
      return max_damage;
    }
  }else if (max_damage){
    return max_damage;
  }else{
    return max_hp;
  }
  
  // ... implement
}

// return score for red: 2 for win, 0 for loss, 1 for draw, -1 for game not
// over yet

int World::red_score() const
{
  if (units.empty())
  {
    return 1;
  }
  bool red_exist(false), blue_exist(false);
  for(Unit *it : units){
    if(it->team == RED){
      red_exist = true;
    }else if(it->team == BLUE){
      blue_exist = true;
    }
  }
  // cout<<"red:"<<red_exist<<"blue:"<<blue_exist<<endl;
  if(red_exist&&blue_exist){
    return -1;
  }else if(!red_exist){
    return 0;
  }else{
    return 2;
  }
  // ... implement
}
