#include <stdio.h>
#include <SDL_image.h>
#include <SDL.h>
#include "animation.h"
#include "sprite.h"
#include "simple_logger.h"
#include "graphics.h"



static Sprite * spriteList = NULL;
static Uint32 MaxSprites = 0;
extern int standAnimations[2][20];


void closeSpriteSystem();

void initSpriteSystem(int maxSprites)
{
    if (maxSprites == 0)
    {
        slog("cannot allocate zero sprites!");
        return;
    }
    spriteList = (Sprite *)malloc(sizeof(Sprite) * maxSprites);
    if (!spriteList)
    {
        slog("failed to allocate sprite system.");
        return;
    }
    memset(spriteList,0,sizeof(Sprite)*maxSprites);
    MaxSprites = maxSprites;
    atexit(closeSpriteSystem);
}

void closeSpriteSystem()
{
    Uint32 i;
    if (!spriteList)
    {
        return;
    }
    for (i = 0; i < MaxSprites; i++)
    {
        if (spriteList[i].image != 0)
        {
            SDL_DestroyTexture(spriteList[i].image);
        }
    }
    free(spriteList);
    spriteList = NULL;
    MaxSprites = 0;
}

Sprite *loadSprite(char *filename,int frameW,int frameH,int fpl)
{
    Uint32 i;
    SDL_Surface *surface;
    Sprite *sprite = NULL;
    if (!spriteList)
    {
        slog("error: using sprite system uninitialized");
        return NULL;
    }
    for (i = 0;i < MaxSprites;i++)
    {
        if (spriteList[i].refCount == 0)
        {
			if(sprite == NULL)
				sprite = &spriteList[i];
            continue;
        }
        if (strcmp(spriteList[i].filename,filename)==0)
        {
            spriteList[i].refCount++;
            return &spriteList[i];
        }
    }
    /*not already loaded, need to load a new one*/
    surface = IMG_Load(filename);
	sprite->filename = filename;
	sprite->frameSize.x = frameW;
	sprite->frameSize.y = frameH;
	sprite->image = SDL_CreateTextureFromSurface(gt_graphics_get_active_renderer(), surface);
	
	
	sprite->framesPerLine = fpl;
	sprite->refCount = 1;

	if (!surface)
    {
        slog("failed to load file %s, re: %s",filename,SDL_GetError());
        return NULL;
    }
    
    
    return sprite;
}
void drawSprite(Sprite *sprite,int frame,Vec2d position,Vec2d scaleFactor,int flip,SDL_Renderer*)
{
    SDL_Rect cell,target;
    SDL_RendererFlip flipFlags = flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE;

    Vec2d scaleOffset = {0,0};
    if (!sprite)
    {
        return;
    }
        
    gt_rect_set(
        cell,
        frame%sprite->framesPerLine * sprite->frameSize.x,
        frame/sprite->framesPerLine * sprite->frameSize.y,
        sprite->frameSize.x,
        sprite->frameSize.y);
    gt_rect_set(
        target,
        position.x - (scaleFactor.x * scaleOffset.x),
                 position.y - (scaleFactor.y * scaleOffset.y),
                 sprite->frameSize.x * scaleFactor.x,
                 sprite->frameSize.y * scaleFactor.y);
    SDL_RenderCopyEx(gt_graphics_get_active_renderer(),
                     sprite->image,
                     &cell,
                     &target,
                     0,
                     NULL,
                     flipFlags);
}





void freeSprite(Sprite **sprite)
{
    Sprite *target;
    if (!sprite)return;
    if (!*sprite)return;
    target = *sprite;
    target->refCount--;
    if (target->refCount <= 0)
    {
        SDL_DestroyTexture(target->image);
        memset(target,0,sizeof(Sprite));
    }
    *sprite = NULL;
}


int fread_num(FILE *f)
{
	char c;
	int num;
	int read = 0;
	num = 0;
	while (1) //while true
	{
		c = fgetc(f); // get char from file
		if(c < '0' || c > '9') // if  char < 0 or > 9
		{
			if(read)
				return num;
			else
			{
				continue;
			}
		}
		read = 1;
		num *=10;
		num += c - '0';
	}
}


void readAnimation(FILE *file, Animation *a)
{
	int i;
	int numFrames;

	

	a->max_steps = fread_num(file);
	for(i = 0; i < a->max_steps;i++)
	{
		a->frame[i] = fread_num(file);
	}
	for(i = 0; i < a->max_steps;i++)
	{
		a->timer[i] = fread_num(file);
	}
}

void readAnimations(char * filename, Animation *a)
{
	int numAnimations;
	int i;
	FILE * file;
	file = fopen(filename,"r");
	numAnimations = fread_num(file);
	for (i = 0;i< numAnimations;i++)
	{
		readAnimation(file,a);
		a++;
	}
}



Sprite * loadSpriteGraphics(char * spriteFile, char * animationFile,int frameW,int frameH,int fpl)
{
	Sprite * s;
	s = loadSprite(spriteFile,frameW,frameH,fpl);
	readAnimations(animationFile, &(s->animations[0]));
	return s;
	
}





/*eol@eof*/
