#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL.h>
#include "vector.h"

typedef struct
{
   int refCount;        /**<keeps track of references to this element*/
   char *filename;  /**<name of the image file loaded*/
   SDL_Texture * image;   /**<texture of the image loaded*/
   //Vec2d imageSize;     /**<x and y dimensions of the image loaded*/
   int framesPerLine; /**<how many frames per line in this sprite sheet*/
   //int imageFPL;        /**<how many frames per line in this sprite sheet*/
   Vec2d frameSize;    /**<the dimensions of a frame in the sprite sheet*/
   Animation animations[2];
   int positionX;
   

}Sprite;

void initSpriteSystem(int maxSprites);
void readAnimations(char *filename, Animation *a);
int fread_num(FILE *f);
extern Vec2d scaleFactor;

Sprite *loadSprite(char *filename,int frameW,int frameH, int fpl);
Sprite * loadSpriteGraphics(char * spriteFile, char * animationFile,int frameW,int frameH,int fpl);
void drawSprite(Sprite *sprite,int frame,Vec2d position,Vec2d scaleFactor,int flip,SDL_Renderer *renderer);

/**
 * @brief frees a loaded sprite from memory
 * @param sprite the sprite to be freed
 */
void freeSprite(Sprite **sprite);

#endif
