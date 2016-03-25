#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL_ttf.h"
#include "SDL.h"
#include "SDL_image.h"
#include "graphics.h"
#include "simple_logger.h"
#include "sprite.h"
#include "entity.h"

#define DIR_UP 0
#define DIR_LEFT 1
#define DIR_RIGHT 2
#define DIR_DOWN 3

#define FIRE_PISTOL 0
#define FIRE_SHOTGUN 1
#define FIRE_MACHINEGUN 2
#define FIRE_HEAVY_MACHINEGUN 3

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect Camera;
extern int bullets;


Sprite * health_bar;

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
void addCoordinateToFile(char *filepath,int x, int y);
void healthBar();
void fire_Pistol();
void fire_Shotgun();

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
  SDL_Rect bulletTarget;
  SDL_Surface *background = NULL;
  SDL_Surface *ground = NULL;
  SDL_Surface *foreground = NULL;
  SDL_Surface *test_player = NULL;
  SDL_Surface *bullet_Surface;
  SDL_Texture *bullet_Texture;
  char bulletText[100];
  TTF_Font *bulletFont = NULL;
  SDL_Color fontColor = {255,255,0};
  char shellText[100];
  TTF_Font *shellFont = NULL;
  int i;
  int done;
  int tx = 0,ty = 0;
  const Uint8 *keys;
  char imagepath[512];
  SDL_Rect srcRect={0,0,1600,600};
  Init_All();

  bulletFont = TTF_OpenFont("fonts/FantasqueSansMono-Regular.ttf",12);
  health_bar  = loadSprite("images/healthbar.png",352,32,1);

  health_title = entity_new();
  health_title->sprite = loadSprite("images/healthbartitle.png",216,36,1);
  health_title->position.x = 0;
  health_title->position.y = 0;


  player = entity_new();
  player->sprite = loadSprite("images/test_player.png",40,40,1);
  player->health = 75;
  player->maxhealth = 100;
  player->bullets = 0;
  player->shells = 0;
  player->rounds = 0;
  player->position.x = 0;
  player->position.y = 400;
  player->bounds.x = 0;
  player->bounds.y = 0;
  player->bounds.w = 40;
  player->bounds.h = 40;


  
  healthPickup = entity_new();
  healthPickup->sprite = loadSprite("images/health.png",20,21,1);
  healthPickup->bounds.x = 0;
  healthPickup->bounds.y = 0;
  healthPickup->bounds.w = 20;
  healthPickup->bounds.h = 21;
  healthPickup->touch = pickup_Health;

  pistolAmmoPickup = entity_new();
  pistolAmmoPickup->sprite = loadSprite("images/pistolAmmo.png",19,17,1);
  pistolAmmoPickup->bounds.x = 0;
  pistolAmmoPickup->bounds.y = 0;
  pistolAmmoPickup->bounds.w = 19;
  pistolAmmoPickup->bounds.h = 17;
  pistolAmmoPickup->touch = pickup_PistolAmmo;

  shotgunAmmoPickup = entity_new();
  shotgunAmmoPickup->sprite = loadSprite("images/shotgunAmmo.png",20,18,1);
  shotgunAmmoPickup->bounds.x = 0;
  shotgunAmmoPickup->bounds.y = 0;
  shotgunAmmoPickup->bounds.w = 20;
  shotgunAmmoPickup->bounds.h = 18;
  shotgunAmmoPickup->touch = pickup_ShotgunAmmo;

  machinegunAmmoPickup = entity_new();
  machinegunAmmoPickup->sprite = loadSprite("images/machinegunAmmo.png",17,18,1);
  machinegunAmmoPickup->bounds.x = 0;
  machinegunAmmoPickup->bounds.y = 0;
  machinegunAmmoPickup->bounds.w = 17;
  machinegunAmmoPickup->bounds.h = 18;
  machinegunAmmoPickup->touch = pickup_MachinegunAmmo;

  pistolPickup = entity_new();


  shotgunPickup = entity_new();


  machinegunPickup = entity_new();


  heavyMachinegunPickup = entity_new();

  




  /*pistolPickup->sprite = loadSprite("images/test_player.png",40,40,1);
  shotgunPickup->sprite = loadSprite("images/test_player.png",40,40,1);
  machinegunPickup->sprite = loadSprite("images/test_player.png",40,40,1);
  heavyMachinegunPickup->sprite = loadSprite("images/test_player.png",40,40,1);*/


  healthPickup->position.x = 50;
  healthPickup->position.y = 400;

  pistolAmmoPickup->position.x = 100;
  pistolAmmoPickup->position.y = 400;
  
  shotgunAmmoPickup->position.x = 150;
  shotgunAmmoPickup->position.y = 400;

  machinegunAmmoPickup->position.x = 200;
  machinegunAmmoPickup->position.y = 400;


  
  background = IMG_Load("images/back_treeline.png");/*notice that the path is part of the filename*/
  if(background == NULL)
  {
	  slog("COuld not load file: %s\n",SDL_GetError());
  }

  ground = IMG_Load("images/ground.png");/*notice that the path is part of the filename*/
  if(ground == NULL)
  {
	  slog("COuld not load file: %s\n",SDL_GetError());
  }

  foreground = IMG_Load("images/front_treeline.png");/*notice that the path is part of the filename*/
  if(foreground == NULL)
  {
	  slog("COuld not load file: %s\n",SDL_GetError());
  }
  /*test_player = IMG_Load("images/test_player.png");/*notice that the path is part of the filename
  if(foreground == NULL)
  {
	  slog("COuld not load file: %s\n",SDL_GetError());
  }*/
  /*if(temp != NULL)
  {
        for(i = 0;i < 12;i++)
        {
            drawSprite();
			SDL_FreeSurface(temp);
        }

  }*/
  slog("got here");
  
  gt_graphics_render_surface_to_screen(background,srcRect,0,0);
  gt_graphics_render_surface_to_screen(ground,srcRect,0,0);
  gt_graphics_render_surface_to_screen(foreground,srcRect,0,0);
  //gt_graphics_render_surface_to_screen(test_player,srcRect,2,5);

 
  //SDL_FreeSurface(test_player);
  slog("got here");
  done = 0;
  do
  {
	SDL_RenderClear(gt_graphics_get_active_renderer());
	gt_graphics_render_surface_to_screen(background,srcRect,0,0);
    gt_graphics_render_surface_to_screen(ground,srcRect,0,0);
    gt_graphics_render_surface_to_screen(foreground,srcRect,0,0);
  //gt_graphics_render_surface_to_screen(test_player,srcRect,2,5);

	ResetBuffer();
	entity_think_all();
	entity_touch_all();
	entity_update_all();
	entity_draw_all();
	  bulletTarget.x = 700;
  bulletTarget.y = 100;
	sprintf(bulletText,"Bullets: %d",player->bullets);
	bullet_Surface = TTF_RenderText_Solid(bulletFont,bulletText,fontColor);
	bullet_Texture = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(),bullet_Surface);
	bulletTarget.w = bullet_Surface->w;
	bulletTarget.h = bullet_Surface->h;
	SDL_FreeSurface(bullet_Surface);
	SDL_RenderCopy(gt_graphics_get_active_renderer(),bullet_Texture,NULL,&bulletTarget);
	SDL_DestroyTexture(bullet_Texture);
	  bulletTarget.x = 700;
  bulletTarget.y = 125;
	sprintf(bulletText,"shells: %d",player->shells);
	bullet_Surface = TTF_RenderText_Solid(bulletFont,bulletText,fontColor);
	bullet_Texture = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(),bullet_Surface);
	bulletTarget.w = bullet_Surface->w;
	bulletTarget.h = bullet_Surface->h;
	SDL_FreeSurface(bullet_Surface);
	SDL_RenderCopy(gt_graphics_get_active_renderer(),bullet_Texture,NULL,&bulletTarget);
	SDL_DestroyTexture(bullet_Texture);
	  bulletTarget.x = 700;
  bulletTarget.y = 150;
	sprintf(bulletText,"Rounds: %d",player->rounds);
	bullet_Surface = TTF_RenderText_Solid(bulletFont,bulletText,fontColor);
	bullet_Texture = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(),bullet_Surface);
	bulletTarget.w = bullet_Surface->w;
	bulletTarget.h = bullet_Surface->h;
	SDL_FreeSurface(bullet_Surface);
	SDL_RenderCopy(gt_graphics_get_active_renderer(),bullet_Texture,NULL,&bulletTarget);
	SDL_DestroyTexture(bullet_Texture);
	healthBar();
    NextFrame();
    SDL_PumpEvents();
	
    keys = SDL_GetKeyboardState(NULL);
    if(keys[SDL_SCANCODE_ESCAPE])
    {
        done = 1;
    }
	else if(keys[SDL_SCANCODE_W])
	{
		moveCharacter(DIR_UP,player);
	}
	else if(keys[SDL_SCANCODE_A])
	{
		moveCharacter(DIR_LEFT,player);
	}
	else if(keys[SDL_SCANCODE_D])
	{
		moveCharacter(DIR_RIGHT,player);
	}
	else if(keys[SDL_SCANCODE_S])
	{
		moveCharacter(DIR_DOWN,player);
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
		//fire_Machinegun();
	}
	else if(keys[SDL_SCANCODE_4])
	{
		//fire_Heavy_Machinegun();
	}
	
  }while(!done);
  slog("got here");
  SDL_FreeSurface(background);
  SDL_FreeSurface(ground);
  SDL_FreeSurface(foreground);
  exit(0);		/*technically this will end the program, but the compiler likes all functions that can return a value TO return a value*/
  return 0;
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
	switch(direction)
	{
	case DIR_UP:
		if(entity->position.y < 310)
		{
			return;
		}
		entity->position.y -= 3;

		break;
	case DIR_LEFT:
		if(entity->position.x < 0)
		{
			return;
		}
		entity->position.x -= 3;
		break;
	case DIR_RIGHT:
		if(entity->position.x > 800-64)
		{
			return;
		}
		entity->position.x += 3;
		break;
	case DIR_DOWN:
		if(entity->position.y > 525-64)
		{
			return;
		}
		entity->position.y += 3;
		break;
	}
}

void fire_Pistol()
{
	Entity *bullet;
	if(player->bullets > 0)
	{
		player->bullets--;
		bullet = entity_new();
		bullet->position = player->position;
		bullet->sprite = loadSprite("images/bullet.png",10,10,1);
		bullet->velocity.x = 10;
	}
}

void fire_Shotgun()
{
	Entity *shellHigh;
	Entity *shellLow;
	if(player->shells > 0)
	{
		player->shells--;
		shellHigh = entity_new();
		shellLow = entity_new();
		shellHigh->position = player->position;
		shellHigh->sprite = loadSprite("images/bullet.png",10,10,1);
		shellHigh->velocity.x = 10;
		shellHigh->velocity.y = 5;

		shellLow->position = player->position;
		shellLow->sprite = loadSprite("images/bullet.png",10,10,1);
		shellLow->velocity.x = 10;
		shellLow->velocity.y = -5;
	}
}


void healthBar()
{
	Vec2d scale;
	scale.y= 1;
	scale.x = player->health/player->maxhealth;
	drawSprite(health_bar,0,vec2d(10,50),scale,gt_graphics_get_active_renderer());
}



