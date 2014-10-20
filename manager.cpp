#include <iostream>
#include <string>
#include <iomanip>
#include "multisprite.h"
#include "sprite.h"
#include "gamedata.h"
#include "manager.h"
#include "drawable.h"
#include <cmath>



bool getDistance(int a1,int a2, int b1, int b2, int value);


Manager::~Manager() { 
  // These deletions eliminate "definitely lost" and
  // "still reachable"s in Valgrind.
  for (unsigned i = 0; i < sprites.size(); ++i) {
    delete sprites[i];
  }
}

Manager::Manager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  io( IOManager::getInstance() ),
  clock( Clock::getInstance() ),
  screen( io.getScreen() ),
  world("back", Gamedata::getInstance().getXmlInt("back/factor") ),
  viewport( Viewport::getInstance() ),
  sprites(),
  currentSprite(0),

  makeVideo( false ),
  eatStar( false ),
  singlePostion( 0 ),
  conflictScale( Gamedata::getInstance().getXmlInt("conflict") ),

  frameCount( 0 ),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  title( Gamedata::getInstance().getXmlStr("screenTitle") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") )
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  SDL_WM_SetCaption(title.c_str(), NULL);
  atexit(SDL_Quit);
  int tmpCount = 0;
  /*----- multi frame object -----*/
  for (int i = 0; i < Gamedata::getInstance().getXmlInt("dragon/count"); i++){
    sprites.push_back( new MultiSprite("dragon", singlePostion) );
    ++singlePostion;
    ++tmpCount;
  }
  // ------ below is the single frame ------  
  for (int i = 0; i < Gamedata::getInstance().getXmlInt("star/count"); i++){
    sprites.push_back( new Sprite("star", tmpCount) );
    ++tmpCount;
  }
  
  
  viewport.setObjectToTrack(sprites[currentSprite]);
}

void Manager::draw() const {
  world.draw();
  for (unsigned i = 0; i < sprites.size(); ++i) {
    sprites[i]->draw();
    //std::cout<<sprites[i]->getFrameFollower()<<"  ";
  }
  //std::cout<<std::endl;

  io.printMessageValueAt("Seconds: ", clock.getSeconds(), 10, 20);
  io.printMessageValueAt("fps: ", clock.getAvgFps(), 10, 40);
  io.printMessageAt("Press T to switch sprites", 10, 70);
  io.printMessageAt(title, 10, 750);
  viewport.draw();

  SDL_Flip(screen);
}

void Manager::makeFrame() {
  std::stringstream strm;
  strm << "frames/" << username<< '.' 
       << std::setfill('0') << std::setw(4) 
       << frameCount++ << ".bmp";
  std::string filename( strm.str() );
  std::cout << "Making frame: " << filename << std::endl;
  SDL_SaveBMP(screen, filename.c_str());
}

void Manager::update() {
  ++clock;
  Uint32 ticks = clock.getElapsedTicks();
  for (unsigned int i = 0; i < sprites.size(); ++i) {
    if (sprites[i]->getFrameFollower() == -1)
      sprites[i]->update(ticks);
    else
      sprites[i]->update(ticks,sprites[sprites[i]->getFrameFollower()]);
  }
  /*----- Birds begin to eat stars ------*/
  if (eatStar){
    for ( int i = 0; i < singlePostion; ++i){
      for ( int j = singlePostion; j< (int)sprites.size()-singlePostion; ++j){
        if(spriteConflict(sprites[i],sprites[j])){}
      }
    }
  }

  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }
  world.update();
  viewport.update(); 
}

bool Manager::spriteConflict( Drawable* multi, Drawable* single ){
  if ( getDistance(multi->X()+multi->getFrameWidth(),multi->Y()+multi->getFrameHeight(),single->X(),single->Y(),conflictScale) && !single->getCatched() )
  {
    single->setCatched(true);
    single->setFrameFollower(multi->getFrameNumber());
    multi->setTotalFollowers(multi->getTotalFollowers()+1);
    return true;
  }
  else
    return false;
}

bool getDistance(int a1,int a2, int b1, int b2, int value){
  int result = sqrt( pow( (a1 - b1), 2 ) + pow( ( a2 - b2 ), 2 ) );
  if (result <= value)
    return true;
  else
    return false;
}

void Manager::play() {
  SDL_Event event;

  bool done = false;
  bool keyCatch = false;
  while ( not done ) {

    SDL_PollEvent(&event);
    Uint8 *keystate = SDL_GetKeyState(NULL);
    if (event.type ==  SDL_QUIT) { done = true; break; }
    if(event.type == SDL_KEYUP) { 
      keyCatch = false; 
    }
    if(event.type == SDL_KEYDOWN) {

      if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
        done = true;
        break;
      }

      if (keystate[SDLK_t] && !keyCatch) {
        keyCatch = true;
        currentSprite = (currentSprite+1) % sprites.size();
        viewport.setObjectToTrack(sprites[currentSprite]);
      }

      if (keystate[SDLK_s] && !keyCatch) {
        keyCatch = true;
        clock.toggleSloMo();
      }

      if (keystate[SDLK_p] && !keyCatch) {
        keyCatch = true;
        if ( clock.isPaused() ) clock.unpause();
        else clock.pause();
      }

      if (keystate[SDLK_F4] && !makeVideo) {
        std::cout << "Making video frames" << std::endl;
        makeVideo = true;
      }

      if (keystate[SDLK_e] && !makeVideo) {
        if(!eatStar){
          std::cout<<"begin eating"<<std::endl;
          io.printMessageAt("Birds begin eating", 100, 70);
          eatStar = true;   
          SDL_Flip(screen);
   
        }
        /*else{
          io.printMessageAt("Birds stop eating", 100, 70);
          eatStar = false;
        }   */
      }
    }
  
    draw();
    update();
  }
}

bool Manager::stopGame(){
  for (int i = singlePostion ; i< (int)sprites.size(); i++){
    if (sprites[i]->getFrameFollower() == -1)
      return false;
  }
  return true;
}
