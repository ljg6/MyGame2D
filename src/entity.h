#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "vector.h"
#include "sprite.h"
#include "enemy.h"


typedef enum 
{
	PLAYER,
	HEALTHPACK,
	PISTOLPACK,
	SHOTGUNPACK,
	MGPACK
} ENTITYTYPES;

/**
 * @brief the core data structure for our entity system
 */
typedef struct Entity_S
{
    int     inuse;              /**<flag for tracking resource use*/
	int cleanUp;
	char name[128];
    Vec2d  position;
    Vec2d  velocity;
	SDL_Rect bounds;
    Sprite *sprite;
	int cameraEnt; /**<true if the entity is drawn relative to the camera*/
    int     frame;
	int		nextStep;
	int		step;
	int		animation;
	int		facingLeft;
	ENEMY soldierPistol;
	ENEMY soldierShotgun;
	ENEMY soldierMachinegun;
	ENEMY soldierHeavyMachinegun;
    void    (*draw)(struct Entity_S *self,SDL_Renderer *renderer);
    int     nextThink; /**<time index for next think*/
    int     thinkRate; /**<how often to run think*/
    void    (*think)(struct Entity_S *self); /**<think function for entity*/
    void    (*update)(struct Entity_S *self);
    void    (*touch)(struct Entity_S *self, struct Entity_S *other);
    void    (*free)(struct Entity_S *self); /**<cleanup function called on free*/
	
}Entity;


/**
 * @brief initializes entity system and queues up cleanup on exit
 * @param maxEntities how many entities the system should support.  Should not be zero
 */
void initEntitySystem(int maxEntities);

void resetAnimation(Entity *e, int animation);

/**
 * @brief return a pointer to an empty entity structure
 * @return NULL on error or no more space for entities or a valid entity pointer otherwise.
 */
Entity *entity_new();



void entity_free(Entity **entity);

void entity_think_all();
void entity_update_all();
void entity_draw_all();
void entity_touch_all();
void pickup_Health(Entity *player,Entity *healthPickup);
void pickup_PistolAmmo(Entity *player,Entity *PistolAmmoPickup);
void pickup_ShotgunAmmo(Entity *player,Entity *ShotgunAmmoPickup);
void pickup_MachinegunAmmo(Entity *player,Entity *MachinegunAmmoPickup);

#endif
