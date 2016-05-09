#include <stdio.h>
#include <SDL.h>
#include "animation.h"
#include "entity.h"
#include "graphics.h"
#include "simple_logger.h"
#include "vector.h"
#include "player.h"

static Entity * EntityList = NULL;
static Uint32 MaxEntities = 0;
extern Uint32 gt_graphics_now;
extern Uint32 gt_graphics_then;


extern Entity *player;
extern Vec2d camera;

int standAnimations[2][20];
void closeEntitySystem();
int entity_intersect(Entity *a, Entity *b);
int entity_intersect_rect(Entity *a,SDL_Rect r);

void initEntitySystem(int maxEntities)
{
    if (maxEntities == 0)
    {
        slog("cannot allocate zero Entities!");
        return;
    }
    EntityList = (Entity *)malloc(sizeof(Entity) * maxEntities);
    if (!EntityList)
    {
        slog("failed to allocate Entity system.");
        return;
    }

    memset(EntityList,0,sizeof(Entity)*maxEntities);
    MaxEntities = maxEntities;
    atexit(closeEntitySystem);
}

void closeEntitySystem()
{
    int i;
    Entity *ent;
    /*for (i = 0; i < MaxEntities;i++)
    {
        ent= &EntityList[i];
        entity_free(&ent);
    }*/
    free(EntityList);
}


void resetAnimation(Entity *e, int animation)
{
	if(e->animation == animation)
	{
		return;
	}
	e->animation = animation;
	e->step = 0;
	e->nextStep = e->sprite->animations[animation].timer[e->step];
	e->frame = e->sprite->animations[animation].frame[e->step];
}

Entity *entity_new()
{
    int i;
    for (i = 0; i < MaxEntities;i++)
    {
        if (EntityList[i].inuse)
        {
            continue;
        }
        memset(&EntityList[i],0,sizeof(Entity));
        /*set some default values here*/
        EntityList[i].inuse = 1;



        return &EntityList[i];
    }
    return NULL;
}




void entity_free(Entity **entity)
{
    Entity *self;
    if (!entity)return;
    if (!*entity)return;
    self = *entity;
    freeSprite(&self->sprite);
	self->inuse = 0;
    *entity = NULL;
}

void entity_think_all()
{
    int i;
    for (i = 0; i < MaxEntities;i++)
    {
        if (!EntityList[i].inuse)
        {
            continue;
        }
        
        if (!EntityList[i].think)
        {
            continue;
        }
        
        EntityList[i].think(&EntityList[i]);
    }
}



void pickup_Health(Entity *healthPickup,Entity *other)
{
		if(other != player)
			return;

		healthPickup->cleanUp =1;
		playerData.health += 25;
		slog("healthPickup touched");
}


/**
* @brief Gives the player the ability to pick up the ammo for the pistol using the established touch function.
* @param pistolAmmoPickup is the pistol ammo entity.
* @param other is the entity touching pistolAmmoPickup (player).
*/
void pickup_PistolAmmo(Entity *pistolAmmoPickup,Entity *other)
{
		if(other != player)
			return;

		pistolAmmoPickup->cleanUp =1;
		playerData.bullets += 10;
		slog("pistolammoPickup touched");
}



void pickup_ShotgunAmmo(Entity *shotgunAmmoPickup,Entity *other)
{
		if(other != player)
			return;

		shotgunAmmoPickup->cleanUp =1;
		playerData.shells += 5;
		slog("shotgunammoPickup touched");
}



void pickup_MachinegunAmmo(Entity *machinegunAmmoPickup,Entity *other)
{
		if(other != player)
			return;

		machinegunAmmoPickup->cleanUp =1;
		playerData.rounds += 200;
		slog("machinegunammoPickup touched");
}



void entity_update_all()
{
    int i;
	int j;
	int anim;
	Entity *e;
	int diff = (gt_graphics_now - gt_graphics_then);
    for (i = 0; i < MaxEntities;i++)
    {
		e = &EntityList[i];
		if(EntityList[i].cleanUp)
		{
			entity_free(&e);
			continue;
		}
        if (!EntityList[i].inuse)
        {
            continue;
        }
        EntityList[i].nextStep-=diff;
		if(EntityList[i].sprite != NULL && EntityList[i].nextStep <=0)
		{
			anim = EntityList[i].animation;
			EntityList[i].step++;
			if(EntityList[i].step > EntityList[i].sprite->animations[anim].max_steps)
			{
				EntityList[i].step = 0;
			}
			EntityList[i].nextStep =EntityList[i].sprite->animations[anim].timer[EntityList[i].step];
			EntityList[i].frame = EntityList[i].sprite->animations[anim].frame[EntityList[i].step];

		}
        vec2d_add(EntityList[i].position,EntityList[i].velocity,EntityList[i].position);
        
        if (!EntityList[i].update)
        {
            continue;
        }
        
        EntityList[i].update(&EntityList[i]);

		


		for(j=0;j < EntityList[j].inuse && j != i;j++)
		{
			if(EntityList[i].touch != NULL)
			{
				(*EntityList[i].touch)(&EntityList[i],&EntityList[j]);
			}
		}
    }
}


void entity_touch_all()
{
    int i;
	int j;
	Entity *e;
    for (i = 0; i < MaxEntities;i++)
    {
		e = &EntityList[i];
        if (!EntityList[i].inuse)
        {
            continue;
        }

		for(j=0;j < MaxEntities && j != i;j++)
		{
			if(!EntityList[j].inuse)
				continue;
			if(i == j)
				continue;
			if(!entity_intersect(&EntityList[i],&EntityList[j]))
				continue;
			if(EntityList[i].touch != NULL)
			{
				(*EntityList[i].touch)(&EntityList[i],&EntityList[j]);
			}
		}
    }
}

void entity_draw_all()
{
    int i;
	int column;
	int row;
	Vec2d target;
    for (i = 0; i < MaxEntities;i++)
    {
        if (!EntityList[i].inuse)
        {
            continue;
        }
        
	/*	column = EntityList[i].frame % EntityList[i].sprite->framesPerLine;
		row = EntityList[i].frame / EntityList[i].sprite->framesPerLine;*/
		if(EntityList[i].cameraEnt)
		{
			vec2d_sub(EntityList[i].position,camera,target);
			drawSprite(EntityList[i].sprite,EntityList[i].frame,target,vec2d(1,1),EntityList[i].facingLeft,gt_graphics_get_active_renderer());
		}else
		drawSprite(EntityList[i].sprite,EntityList[i].frame,EntityList[i].position,vec2d(1,1),EntityList[i].facingLeft,gt_graphics_get_active_renderer());

    }
}



Entity *entity_intesect_all(Entity *a)
{
    int i;
    if (!a)return NULL;
    for (i = 0; i < MaxEntities;i++)
    {
        if (!EntityList[i].inuse)
        {
            continue;
        }
        if (a == &EntityList[i])
        {
            continue;
            /*don't clip self*/
        }
        if (entity_intersect(a, &EntityList[i]))
        {
            return &EntityList[i];
        }
    }
    return NULL;
}

int entity_intersect_rect(Entity *a,SDL_Rect r)
{
    SDL_Rect aB;
    if (!a)
    {
        slog("ERROR: Missing entity for check");
        return 0;
    }
    aB = rect(
        a->position.x + a->bounds.x,
        a->position.y + a->bounds.y,
        a->bounds.w,
        a->bounds.h);
    return rect_intersect(aB,r);
}

int entity_intersect(Entity *a, Entity *b)
{
    SDL_Rect aB,bB;
    if ((!a) || (!b))
    {
        slog("ERROR: Missing entity for check");
        return 0;
    }
    aB = rect(
        a->position.x + a->bounds.x,
        a->position.y + a->bounds.y,
        a->bounds.w,
        a->bounds.h);
    bB = rect(
        b->position.x + b->bounds.x,
        b->position.y + b->bounds.y,
        b->bounds.w,
        b->bounds.h);
    return rect_intersect(aB,bB);
}




/*eol@eof*/