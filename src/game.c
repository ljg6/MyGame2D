#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL_ttf.h"
#include "SDL.h"
#include "SDL_image.h"
#include "graphics.h"
#include "simple_logger.h"
#include "animation.h"
#include "sprite.h"
#include "entity.h"
#include "player.h"
#include "save.h"

#define DIR_UP 0
#define DIR_LEFT 1
#define DIR_RIGHT 2
#define DIR_DOWN 3

#define MENU_UP 0
#define MENU_DOWN 1

#define FIRE_PISTOL 0
#define FIRE_SHOTGUN 1
#define FIRE_MACHINEGUN 2
#define FIRE_HEAVY_MACHINEGUN 3

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect Camera;
extern int bullets;

Vec2d camera;
Player playerData;

Sprite * health_bar;
Sprite * floor;
Sprite * frontTrees;
Sprite * backTrees;
Sprite * mainScreen;
Sprite * selection;
Sprite * levelTwoFloor;
Sprite * levelTwoFrontTrees;
Sprite * levelTwoBackTrees;
Sprite * moonBack;

Entity *health_title;
Entity *player;
Entity *enemy;
Entity *healthPickup;
Entity *pistolAmmoPickup;
Entity *shotgunAmmoPickup;
Entity *machinegunAmmoPickup;
Entity *pistolPickup;
Entity *shotgunPickup;
Entity *machinegunPickup;
Entity *heavyMachinegunPickup;

void moveCharacter(int direction, Entity *entity);


void Init_All();

int getImagePathFromFile(char *filepath,char * filename);
int getCoordinatesFromFile(int *x, int *y,char * filename);
float xDistance;
float yDistance;
int backOffset;
int offset;
int gameState = 0;
int done;
int moving = 0;
void addCoordinateToFile(char *filepath,int x, int y);
void healthBar();
void fire_Pistol();
void fire_Shotgun();
void fire_Machinegun();
void fire_Heavy_Machinegun();
extern Uint32 gt_graphics_now;
extern Uint32 gt_graphics_then;
const Uint8 *keys;
SDL_Rect bulletTarget;
  SDL_Rect healthTrack;
  SDL_Surface *background = NULL;
  SDL_Surface *ground = NULL;
  SDL_Surface *foreground = NULL;
  SDL_Surface *test_player = NULL;
  SDL_Surface *main_Screen = NULL;
  SDL_Surface *bullet_Surface;
  SDL_Texture *bullet_Texture;
  SDL_Surface *health_Surface;
  SDL_Texture *health_Texture;
  TTF_Font *bulletFont = NULL;
  TTF_Font *healthFont = NULL;
  int diff;
  
  char bulletText[100];
  char healthText[5];
  
  SDL_Color projectileColor = {255,255,0};
  SDL_Color healthColor = {255,255,255};
  char shellText[100];
  TTF_Font *shellFont = NULL;
  int i;
  int tx = 0,ty = 0;
  Vec2d vector;
 
  
  
  char imagepath[512];
  SDL_Rect srcRect={0,0,1600,600};


int main_menu();
int pause_menu();
int levelOne();
int levelTwo();

/*this program must be run from the directory directly below images and src, not from within src*/
/*notice the default arguments for main.  SDL expects main to look like that, so don't change it*/
/**
 * @brief Main function for the game.
 * @param argc count of arguments provided to the game.
 * @param argv an array of the arguments provided to the game.
 * @return always 0.
 */
int main(int argc, char *argv[])
{	
  Init_All();
  done = 0;
  do
  {
	if(gameState == 0)
	{
		  main_menu();
	 }
	else if(gameState == 1)
	{
		levelOne();
	}
	else if(gameState == 2)
	{
		levelTwo();
	}
	else if(gameState ==3)
	{

	}
  }while(!done);
  
  slog("got here");
  exit(0);		/*technically this will end the program, but the compiler likes all functions that can return a value TO return a value*/
  SDL_DestroyTexture(bullet_Texture);
  return 1;
}


void init_player()
{
	if(player != NULL)
	{
		return;
	}
  player = entity_new();
  player->sprite = loadSpriteGraphics("images/player_sheet.png","text/player.anim",128,128,9);
  player->cameraEnt = 1;
  player->position.x = 0;
  player->position.y = 350;
  player->bounds.x = 40;
  player->bounds.y = 32;
  player->bounds.w = 40;
  player->bounds.h = 68;

  playerData.health = 75;
  playerData.maxhealth = 100;
  playerData.bullets = 0;
  playerData.shells = 0;
  playerData.rounds = 0;
}

void init_UI()
{
	health_title = entity_new();
	health_title->sprite = loadSprite("images/healthbartitle.png",216,36,1);
    health_title->position.x = 0;
    health_title->position.y = 0;
	bulletFont = TTF_OpenFont("fonts/FantasqueSansMono-Regular.ttf",12);
	healthFont = TTF_OpenFont("fonts/FantasqueSansMono-Regular.ttf",14);
    health_bar  = loadSprite("images/healthbar.png",352,32,1);

	healthTrack.x = 176;
	healthTrack.y = 60;
	sprintf(healthText,"%3d%%",playerData.health);
	health_Surface = TTF_RenderText_Solid(healthFont,healthText,healthColor);
	health_Texture = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(),health_Surface);
	healthTrack.w = health_Surface->w;
	healthTrack.h = health_Surface->h;
	SDL_FreeSurface(health_Surface);
	SDL_RenderCopy(gt_graphics_get_active_renderer(),health_Texture,NULL,&healthTrack);
	

	bulletTarget.x = 700;
    bulletTarget.y = 100;
	sprintf(bulletText,"Bullets: %d",playerData.bullets);
	bullet_Surface = TTF_RenderText_Solid(bulletFont,bulletText,projectileColor);
	bullet_Texture = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(),bullet_Surface);
	bulletTarget.w = bullet_Surface->w;
	bulletTarget.h = bullet_Surface->h;
	SDL_FreeSurface(bullet_Surface);
	SDL_RenderCopy(gt_graphics_get_active_renderer(),bullet_Texture,NULL,&bulletTarget);
	

	bulletTarget.x = 700;
    bulletTarget.y = 125;
	sprintf(bulletText,"shells: %d",playerData.shells);
	bullet_Surface = TTF_RenderText_Solid(bulletFont,bulletText,projectileColor);
	bullet_Texture = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(),bullet_Surface);
	bulletTarget.w = bullet_Surface->w;
	bulletTarget.h = bullet_Surface->h;
	SDL_FreeSurface(bullet_Surface);
	SDL_RenderCopy(gt_graphics_get_active_renderer(),bullet_Texture,NULL,&bulletTarget);
	

	bulletTarget.x = 700;
    bulletTarget.y = 150;
	sprintf(bulletText,"Rounds: %d",playerData.rounds);
	bullet_Surface = TTF_RenderText_Solid(bulletFont,bulletText,projectileColor);
	bullet_Texture = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(),bullet_Surface);
	bulletTarget.w = bullet_Surface->w;
	bulletTarget.h = bullet_Surface->h;
	SDL_FreeSurface(bullet_Surface);
	SDL_RenderCopy(gt_graphics_get_active_renderer(),bullet_Texture,NULL,&bulletTarget);
	
}
void init_lvl1Graphics()
{
	camera = vec2d(0,0);
	floor = loadSprite("images/ground.png",1600,600,1);
    backTrees = loadSprite("images/back_treeline.png",1600,600,1);
    frontTrees = loadSprite("images/front_treeline.png",1600,600,1);
}
void init_lvlOnePickups()
{
	healthPickup = entity_new();
  healthPickup->sprite = loadSpriteGraphics("images/health.png","text/health.anim",20,20,4);
  resetAnimation(healthPickup,0);
  healthPickup->bounds.x = 0;
  healthPickup->bounds.y = 0;
  healthPickup->bounds.w = 20;
  healthPickup->bounds.h = 21;
  healthPickup->touch = pickup_Health;

  pistolAmmoPickup = entity_new();
  pistolAmmoPickup->sprite = loadSpriteGraphics("images/pistolAmmo_sheet.png","text/pistol.anim",19,17,4);
  pistolAmmoPickup->bounds.x = 0;
  pistolAmmoPickup->bounds.y = 0;
  pistolAmmoPickup->bounds.w = 19;
  pistolAmmoPickup->bounds.h = 17;
  pistolAmmoPickup->touch = pickup_PistolAmmo;

  shotgunAmmoPickup = entity_new();
  shotgunAmmoPickup->sprite = loadSpriteGraphics("images/shotgunAmmo_sheet.png","text/pistol.anim",20,18,4);
  shotgunAmmoPickup->bounds.x = 0;
  shotgunAmmoPickup->bounds.y = 0;
  shotgunAmmoPickup->bounds.w = 20;
  shotgunAmmoPickup->bounds.h = 18;
  shotgunAmmoPickup->touch = pickup_ShotgunAmmo;

  machinegunAmmoPickup = entity_new();
  machinegunAmmoPickup->sprite = loadSpriteGraphics("images/machinegunAmmo_sheet.png","text/pistol.anim",17,18,4);
  machinegunAmmoPickup->bounds.x = 0;
  machinegunAmmoPickup->bounds.y = 0;
  machinegunAmmoPickup->bounds.w = 17;
  machinegunAmmoPickup->bounds.h = 18;
  machinegunAmmoPickup->touch = pickup_MachinegunAmmo;

  pistolPickup = entity_new();
  shotgunPickup = entity_new();
  machinegunPickup = entity_new();
  heavyMachinegunPickup = entity_new();

  healthPickup->position.x = 50;
  healthPickup->position.y = 400;

  pistolAmmoPickup->position.x = 100;
  pistolAmmoPickup->position.y = 400;
  
  shotgunAmmoPickup->position.x = 150;
  shotgunAmmoPickup->position.y = 400;

  machinegunAmmoPickup->position.x = 200;
  machinegunAmmoPickup->position.y = 400;
}
void init_lvl2Graphics()
{
	camera = vec2d(0,0);
	levelTwoFloor = loadSprite("images/level2_ground.png",1600,600,1);
    levelTwoBackTrees = loadSprite("images/level2_back_treeline.png",1600,600,1);
    levelTwoFrontTrees = loadSprite("images/level2_front_treeline.png",1600,600,1);
    moonBack = loadSprite("images/moon.png",800,600,1);
}

void init_lvl1()
{
	init_UI();
	init_player();
	init_lvl1Graphics();
	init_lvlOnePickups();
}
void init_lvl2()
{
	init_player();
	init_UI();
	init_lvl2Graphics();
}

int levelOne()
{
	
	
	moving = 0;
	SDL_RenderClear(gt_graphics_get_active_renderer());
	ResetBuffer();

	backOffset = ((int)camera.x+3200)%3200;
	offset = ((int)camera.x+1600)%1600;

	drawSprite(backTrees,0,vec2d(-backOffset,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
    drawSprite(frontTrees,0,vec2d(-offset,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	drawSprite(floor,0,vec2d(-offset,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	
	drawSprite(backTrees,0,vec2d(-backOffset+1600,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
    drawSprite(frontTrees,0,vec2d(-offset+1600,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	drawSprite(floor,0,vec2d(-offset+1600,0),vec2d(1,1),0,gt_graphics_get_active_renderer());

	entity_think_all();
	entity_touch_all();
	entity_update_all();
	entity_draw_all();
	if(player->position.x >= 300)
		{
			camera.x = player->position.x - 300;
			
		}
	if(player->position.x < 300)
		{
			camera.x = 0;
			
		}


	healthBar();
    NextFrame();
    SDL_PumpEvents();
	diff = (gt_graphics_now - gt_graphics_then);
	
	if(playerData.shotTimer>0)
	{
		playerData.shotTimer -= diff;
	}
    keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_ESCAPE])
    {
        done = 1;
    }
	else if(keys[SDL_SCANCODE_W])
	{
		moving = 1;
		moveCharacter(DIR_UP,player);
		yDistance = player->position.y;
	}
	else if(keys[SDL_SCANCODE_A])
	{
		moving = 1;
		moveCharacter(DIR_LEFT,player);
		xDistance =player->position.x;
	}
	else if(keys[SDL_SCANCODE_D])
	{
		moving = 1;
		moveCharacter(DIR_RIGHT,player);
		xDistance = player->position.x;
		
	}
	else if(keys[SDL_SCANCODE_S])
	{
		moving = 1;
		moveCharacter(DIR_DOWN,player);
		yDistance = player->position.y;
	}
	else if(keys[SDL_SCANCODE_1])
	{
		fire_Pistol();
	}
	else if(keys[SDL_SCANCODE_2])
	{
		fire_Shotgun();
	}
	else if(keys[SDL_SCANCODE_3])
	{
		fire_Machinegun();
	}
	else if(keys[SDL_SCANCODE_4])
	{
		fire_Heavy_Machinegun();
	}
	else if(keys[SDL_SCANCODE_P])
	{
		saveState();
	}
	else if(keys[SDL_SCANCODE_O])
	{
		loadState();
	}
	if(moving == 0)
	{
		resetAnimation(player, 0);
	}
	if(player->position.x >= 1200.00 && (gameState != 2))
		{
			gameState = 2;
			init_lvl2();
		}

	
  
  return 0;
}
int levelTwo()
{
	

	moving = 0;
	SDL_RenderClear(gt_graphics_get_active_renderer());
	ResetBuffer();

	backOffset = ((int)camera.x+3200)%3200;
	offset = ((int)camera.x+1600)%1600;

	drawSprite(moonBack,0,vec2d(0,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	
	drawSprite(levelTwoBackTrees,0,vec2d(-backOffset,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
    drawSprite(levelTwoFrontTrees,0,vec2d(-offset,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	drawSprite(levelTwoFloor,0,vec2d(-offset,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	
	drawSprite(levelTwoFrontTrees,0,vec2d(-offset+1600,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	drawSprite(levelTwoBackTrees,0,vec2d(-backOffset+1600,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	drawSprite(levelTwoFloor,0,vec2d(-offset+1600,0),vec2d(1,1),0,gt_graphics_get_active_renderer());

	



	entity_think_all();
	entity_touch_all();
	entity_update_all();
	entity_draw_all();
	if(player->position.x >= 300)
		{
			camera.x = player->position.x - 300;
			
		}
	if(player->position.x < 300)
		{
			camera.x = 0;
			
		}
	if(xDistance == 12000.00)
	{
		done = 1;
	}

	healthBar();
    NextFrame();
    SDL_PumpEvents();
	diff = (gt_graphics_now - gt_graphics_then);
	
	if(playerData.shotTimer>0)
	{
		playerData.shotTimer -= diff;
	}
    keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_ESCAPE])
    {
        done = 1;
    }
	else if(keys[SDL_SCANCODE_W])
	{
		moving = 1;
		moveCharacter(DIR_UP,player);
		yDistance = player->position.y;
	}
	else if(keys[SDL_SCANCODE_A])
	{
		moving = 1;
		moveCharacter(DIR_LEFT,player);
		xDistance = player->position.x;
	}
	else if(keys[SDL_SCANCODE_D])
	{
		moving = 1;
		moveCharacter(DIR_RIGHT,player);
		xDistance = player->position.x;
		
	}
	else if(keys[SDL_SCANCODE_S])
	{
		moving = 1;
		moveCharacter(DIR_DOWN,player);
		yDistance = player->position.y;
	}
	else if(keys[SDL_SCANCODE_1])
	{
		fire_Pistol();
	}
	else if(keys[SDL_SCANCODE_2])
	{
		fire_Shotgun();
	}
	else if(keys[SDL_SCANCODE_3])
	{
		fire_Machinegun();
	}
	else if(keys[SDL_SCANCODE_4])
	{
		fire_Heavy_Machinegun();
	}
	else if(keys[SDL_SCANCODE_P])
	{
		saveState();
	}
	else if(keys[SDL_SCANCODE_O])
	{
		loadState();
	}
	if(moving == 0)
	{
		resetAnimation(player, 0);
	}


	
  
  return 0;
}

int main_menu()
{
  static int i = 0;
  static int upHeld = 0;
  static int downHeld = 0;
  SDL_Renderer *render;
  render = gt_graphics_get_active_renderer();
  SDL_RenderClear(gt_graphics_get_active_renderer());
  ResetBuffer();
  mainScreen = loadSprite("images/opening_screen.png",800,600,1);
  selection = loadSprite("images/selection.png",149,53,1);
  printf("%i", i);
  if(i == 0)
	{
		drawSprite(mainScreen,0,vec2d(0,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
		drawSprite(selection,0,vec2d(311,292),vec2d(1,1),0,gt_graphics_get_active_renderer());
  }else if(i == 1)
  {
	  drawSprite(mainScreen,0,vec2d(0,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	  drawSprite(selection,0,vec2d(275,376),vec2d(1.5,1),0,gt_graphics_get_active_renderer());
	  
  }else if(i == 2)
  {
	  drawSprite(mainScreen,0,vec2d(0,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	  drawSprite(selection,0,vec2d(311,460),vec2d(1,1),0,gt_graphics_get_active_renderer());
	  
  }
  NextFrame();
  SDL_PumpEvents();
  keys = SDL_GetKeyboardState(NULL);
  if(keys[SDL_SCANCODE_ESCAPE])
    {
        done = 1;
    }
  if(keys[SDL_SCANCODE_DOWN]&&downHeld == 0)
    {
	  i=(i+1)%3;
	  downHeld = 1;
	}else if(!keys[SDL_SCANCODE_DOWN])
  {
		downHeld = 0;
  }
  if(keys[SDL_SCANCODE_UP]&&upHeld == 0)
  {
	  if((i = i-1)<0)
		  i = 2;

	  upHeld = 1;
	}else if(!keys[SDL_SCANCODE_UP])
  {
		upHeld = 0;
  }
  if(keys[SDL_SCANCODE_RETURN])
  {
	  if(i == 0)
	  {
		  gameState = 1;
		  init_lvl1();
	  }
	  else if(i == 2)
	  {
		  done = 1;
	  }
	  else if(i == 1)
	  {
		  init_player();

		  loadState();
		  if(gameState == 1)
		  {
			  init_lvl1();
		  }else if(gameState == 2)
		  {
			  init_lvl2();
		  }
	  }
  }	  
   return 1;
}
int pause_menu()
{
  static int i = 0;
  SDL_Renderer *render;
  render = gt_graphics_get_active_renderer();
  SDL_RenderClear(gt_graphics_get_active_renderer());
  ResetBuffer();
  mainScreen = loadSprite("images/pause_screen.png",800,600,1);
  selection = loadSprite("images/selection.png",149,53,1);
  if(i == 0)
	{
		drawSprite(mainScreen,0,vec2d(0,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
		drawSprite(selection,0,vec2d(311,294),vec2d(1,1),0,gt_graphics_get_active_renderer());
  }else if(i ==1)
  {
	  drawSprite(mainScreen,0,vec2d(0,0),vec2d(1,1),0,gt_graphics_get_active_renderer());
	  drawSprite(selection,0,vec2d(311,377),vec2d(1,1),0,gt_graphics_get_active_renderer());
	  
  }
  NextFrame();
  SDL_PumpEvents();
  keys = SDL_GetKeyboardState(NULL);
  if(keys[SDL_SCANCODE_ESCAPE])
    {
        done = 1;
    }
  if(keys[SDL_SCANCODE_DOWN])
    {
	  i = 1;
	}
  if(keys[SDL_SCANCODE_UP])
  {
	  i = 0;
  }
  if(keys[SDL_SCANCODE_RETURN])
  {
	  if(i == 0)
	  {
		  gameState = 1;
	  }
	  else
	  {
		  done = 1;
	  }
  }	  
   return 1;
}

/**
* @brief function that initializes graphics and sets a background color.
*/
void Init_All()
{
	float bgcolor[] = {1,1,1,1};
  Init_Graphics("Game Test",800,600,800,600,bgcolor,0);
  TTF_Init();
  initEntitySystem(100);
  initSpriteSystem(100);
  
}

int getImagePathFromFile(char *filepath,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int returnValue = -1;
    if (!filepath)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getImagePathFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    if (fscanf(fileptr,"%s",buf))
    {
        if (strcmp(buf,"image:")==0)
        {
            fscanf(fileptr,"%s",filepath);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    return returnValue;
}

void addCoordinateToFile(char *filepath,int x, int y)
{
    FILE *fileptr = NULL;
    if (!filepath)
    {
        fprintf(stdout,"addCoordinateToFile: warning, no input file path provided\n");
        return;
    }
    fileptr = fopen(filepath,"a");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filepath);
        return;
    }
    fprintf(fileptr,"%s:%i:newcoordinate: %i %i\n",__FILE__,__LINE__,x,y);
    fclose(fileptr);
}

int getCoordinatesFromFile(int *x, int *y,char * filename)
{
    FILE *fileptr = NULL;
    char buf[255];
    int tx,ty;
    int returnValue = -1;
    if ((!x)&&(!y))
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no output parameter provided\n");
        return -1;
    }
    if (!filename)
    {
        fprintf(stdout,"getCoordinatesFromFile: warning, no input file path provided\n");
        return -1;
    }
    fileptr = fopen(filename,"r");
    if (!fileptr)
    {
        fprintf(stderr,"unable to open file: %s\n",filename);
        return -1;
    }
    while (fscanf(fileptr,"%s",buf) != EOF)
    {
        fprintf(stdout,"buf is: %s\n",buf);
        if (strcmp(buf,"position:")==0)
        {
            fscanf(fileptr,"%i %i",&tx,&ty);
            fprintf(stdout,"as read: %i, %i\n",tx,ty);
            returnValue = 0;
        }
    }
    fclose(fileptr);
    if (x)*x = tx;
    if (y)*y = ty;
    return returnValue;
}



/**
* @brief Moves the character on screen.
* @param direction specifies which direction the character moves.
*/
void moveCharacter(int direction, Entity *entity)
{
	
	resetAnimation(entity,1);
	switch(direction)
	{
	case DIR_UP:
		if(entity->position.y < 245)
		{
			return;
		}
		entity->position.y -= 3;

		break;
	case DIR_LEFT:
		entity->facingLeft = 1;
		if(entity->position.x < 0)
		{
			return;
		}
		entity->position.x -= 3;
		break;
	case DIR_RIGHT:
		entity->facingLeft = 0;

		entity->position.x += 3;
		
		break;
	case DIR_DOWN:
		if(entity->position.y > 475-64)
		{
			return;
		}
		entity->position.y += 3;
		break;
	}
}


/**
* @brief Fires the players pistol.
*/
void fire_Pistol()
{
	Entity *bullet;
	int clip = 8;
	if(playerData.bullets > 0 && playerData.shotTimer <= 0)
	{

		playerData.bullets--;
		playerData.shotTimer = 500;
		bullet = entity_new();
		bullet->position.x = player->position.x + 45;
		bullet->position.y = player->position.y + 42;
		bullet->sprite = loadSprite("images/bullet.png",10,10,1);
		bullet->velocity.x = 30;
		if(player->facingLeft == 1)
		{
			bullet->velocity.x =-30;
		}
	}
}



/**
* @brief Fires the players shotgun.
*/
void fire_Shotgun()
{
	Entity *shellHigh;
	Entity *shellLow;
	Entity *shellMid;
	if(playerData.shells > 0 && playerData.shotTimer <= 0)
	{
		playerData.shells--;
		playerData.shotTimer = 1400;
		shellHigh = entity_new();
		shellLow = entity_new();
		shellMid = entity_new();


		shellMid->position.x = player->position.x + 45;
		shellMid->position.y = player->position.y + 42;
		shellMid->sprite = loadSprite("images/shell.png",20,10,1);
		shellMid->velocity.x = 30;
		if(player->facingLeft == 1)
		{
			shellMid->velocity.x =-30;
		}

		shellHigh->position.x = player->position.x + 45;
		shellHigh->position.y = player->position.y + 42;
		shellHigh->sprite = loadSprite("images/shell.png",20,10,1);
		shellHigh->velocity.x = 30;
		shellHigh->velocity.y = 1;
		if(player->facingLeft == 1)
		{
			shellHigh->velocity.x =-30;
		}

		shellLow->position.x = player->position.x + 45;
		shellLow->position.y = player->position.y + 42;
		shellLow->sprite = loadSprite("images/shell.png",20,10,1);
		shellLow->velocity.x = 30;
		shellLow->velocity.y = -1;
		if(player->facingLeft == 1)
		{
			shellLow->velocity.x =-30;
		}
	}
}

/**
* @brief Fires the players machinegun.
*/
void fire_Machinegun()
{
	Entity *round;
	if(playerData.rounds > 0&& playerData.shotTimer <= 0)
	{
		playerData.rounds--;
		if(playerData.shotCounter >= 3)
		{
			playerData.shotCounter = 0;
			playerData.shotTimer = 750;
		}else{
			playerData.shotCounter++;
			playerData.shotTimer = 75;
		}
		round = entity_new();
		round->position.x = player->position.x + 45;
		round->position.y = player->position.y + 42;
		round->sprite = loadSprite("images/round.png",20,10,1);
		round->velocity.x = 30;
		if(player->facingLeft == 1)
		{
			round->velocity.x =-30;
		}
	}
}

void fire_Heavy_Machinegun()
{
	Entity *round;
	if(playerData.rounds > 0&& playerData.shotTimer <= 0)
	{
		playerData.rounds--;
		if(playerData.shotCounter >= 80)
		{
			playerData.shotCounter = 0;
			playerData.shotTimer = 700;
		}else{
			playerData.shotCounter++;
			playerData.shotTimer = 70;
		}
		round = entity_new();
		round->position.x = player->position.x + 45;
		round->position.y = player->position.y + 42;
		round->sprite = loadSprite("images/round.png",20,10,1);
		round->velocity.x = 35;
		if(player->facingLeft == 1)
		{
			round->velocity.x =-35;
		}
	}
}

/**
* @brief Displays the players health bar.
*/
void healthBar()
{
	Vec2d scale;
	scale.y= 1;
	scale.x = (float)(playerData.health)/playerData.maxhealth;
	drawSprite(health_bar,0,vec2d(10,50),scale,0,gt_graphics_get_active_renderer());
}


void saveState()
{
	FILE *saveFile;
	saveFile = fopen("text/saveFile.txt","w");
	fprintf(saveFile,"%d %d %d %d %d %f %f", gameState,playerData.health,playerData.bullets,playerData.shells,playerData.rounds,player->position.x,player->position.y);
	fclose(saveFile);
}

void loadState()
{
	FILE *loadFile;
	loadFile = fopen("text/saveFile.txt","r");
	fscanf(loadFile,"%d %d %d %d %d %f %f", &gameState,&playerData.health,&playerData.bullets,&playerData.shells,&playerData.rounds,&player->position.x,&player->position.y);
	fclose(loadFile);
}

//void enemyPistol(ENEMY * e,


