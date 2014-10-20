#include <vector>
#include <SDL.h>
#include "ioManager.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "drawable.h"

class Manager {
public:
  Manager ();
  ~Manager ();
  void play();
  bool spriteConflict( Drawable* multi, Drawable* single );
  bool stopGame();
private:
  const bool env;
  const IOManager& io;
  Clock& clock;

  SDL_Surface * const screen;
  World world;
  Viewport& viewport;

  std::vector<Drawable*> sprites;
  int currentSprite;

  bool makeVideo;
  bool eatStar;//let the program know that the eagle now eating stars
  int singlePostion;//when the singlePost will start
  int conflictScale;
  
  int frameCount;
  const std::string username;
  const std::string title;
  const int frameMax;

  void draw() const;
  void update();

  Manager(const Manager&);
  Manager& operator=(const Manager&);
  void makeFrame();
};
