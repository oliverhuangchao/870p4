#ifndef DRAWABLE__H
#define DRAWABLE__H
#include <SDL.h>
#include <iostream>
#include <string>
#include "vector2f.h"
#include "frame.h"
 
// Drawable is an Abstract Base Class (ABC) that
// specifies the methods that derived classes may
// and must have.
class Drawable {
public:
  Drawable(const std::string& n, const Vector2f& pos, const Vector2f& vel, const int order,const int width, const int height): 
    name(n), position(pos), velocity(vel),frameNumber(order),frameFollowNumber(-1), catched(false), totalFollowers(0), frameWidth(width), frameHeight(height) {}

  Drawable(const Drawable& s) : 
    name(s.name), position(s.position), velocity(s.velocity),frameNumber(s.frameNumber),frameFollowNumber(s.frameFollowNumber),catched(s.catched), totalFollowers(s.totalFollowers), frameWidth(s.frameWidth), frameHeight(s.frameHeight)
    { }

  virtual ~Drawable() {}

  const std::string& getName() const { return name; }
  void setName(const std::string& n) { name = n;    }
  virtual const Frame* getFrame() const = 0;

  virtual void draw() const = 0;
  virtual void update(Uint32 ticks) = 0;
  virtual void update(Uint32 ticks, Drawable*) = 0;

  float X() const { return position[0]; }
  void X(float x) { position[0] = x;    }

  float Y() const { return position[1]; }
  void Y(float y) { position[1] = y;    }

  float velocityX() const  { return velocity[0]; }
  void velocityX(float vx) { velocity[0] = vx;   }
  float velocityY() const  { return velocity[1]; }
  void velocityY(float vy) { velocity[1] = vy;   }

  const Vector2f& getVelocity() const   { return velocity; }
  void setVelocity(const Vector2f& vel) { velocity = vel;  }
  const Vector2f& getPosition() const   { return position; }
  void setPosition(const Vector2f& pos) { position = pos;  }

  virtual bool collidedWith(const Drawable*) const { 
    throw std::string("No collidedWith");  
  }

  void setFrameNumber(const int x){frameNumber = x;}
  void setFrameFollower(const int x){frameFollowNumber = x;}
  void setCatched(const bool x){catched = x;}
  void setTotalFollowers(const int x){totalFollowers = x;}

  int getFrameNumber() const {return frameNumber;}
  int getFrameFollower() const {return frameFollowNumber;}
  bool getCatched(){return catched;}
  int getFrameWidth(){return frameWidth;}
  int getFrameHeight(){return frameHeight;}
  int getTotalFollowers(){return totalFollowers;}



private:
  std::string name;
  Vector2f position;
  Vector2f velocity;
  int frameNumber;//the number for each sprite
  int frameFollowNumber;//the number of sprite he try to follow

  bool catched;
  int totalFollowers;

  int frameWidth;
  int frameHeight;
};
#endif
