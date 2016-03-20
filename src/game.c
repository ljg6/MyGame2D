#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

extern SDL_Surface *screen;
extern SDL_Surface *buffer; /*pointer to the draw buffer*/
extern SDL_Rect Camera;

Entity *player;
Entity *enemy;
Entity *itemPickup;
Entity *weaponPickup;
void moveCharacter(int direction, Entity *entity);


void Init_All();

int getImagePathFromFile(char *filepath,char * filename);
int getCoordinatesFromFile(int *x, int *y,char * filename);
void addCoordinateToFile(char *filepath,int x, int y);


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
  SDL_Surface *background = NULL;
  SDL_Surface *ground = NULL;
  SDL_Surface *foreground = NULL;
  SDL_Surface *test_player = NULL;
  int i;
  int done;
  int tx = 0,ty = 0;
  const Uint8 *keys;
  char imagepath[512];
  SDL_Rect srcRect={0,0,1600,600};
  Init_All();
  player = entity_new();
  player->sprite = loadSprite("images/test_player.png",40,40,1);
  player->health = 100;
  player->position.x = 0;
  player->position.y = 400;
  player->velocity.x = 0;
  player->velocity.y=0;
  


  
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
	entity_draw_all();
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
