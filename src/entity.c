#include <stdio.h>
#include <SDL.h>
#include "entity.h"
#include "graphics.h"
#include "simple_logger.h"
#include "vector.h"

static Entity * EntityList = NULL;
static Uint32 MaxEntities = 0;


extern Entity *player;


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
		player->health += 25;
		slog("healthPickup touched");
}

void pickup_PistolAmmo(Entity *pistolAmmoPickup,Entity *other)
{
		if(other != player)
			return;

		pistolAmmoPickup->cleanUp =1;
		player->bullets += 10;
		slog("pistolammoPickup touched");
}

void pickup_ShotgunAmmo(Entity *shotgunAmmoPickup,Entity *other)
{
		if(other != player)
			return;

		shotgunAmmoPickup->cleanUp =1;
		player->shells += 5;
		slog("shotgunammoPickup touched");
}

void pickup_MachinegunAmmo(Entity *machinegunAmmoPickup,Entity *other)
{
		if(other != player)
			return;

		machinegunAmmoPickup->cleanUp =1;
		player->rounds += 20;
		slog("machinegunammoPickup touched");
}

void fire_Pistol( Entity *bullet)
{

}


void entity_update_all()
{
    int i;
	int j;
	Entity *e;
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
    for (i = 0; i < MaxEntities;i++)
    {
        if (!EntityList[i].inuse)
        {
            continue;
        }
        
	/*	column = EntityList[i].frame % EntityList[i].sprite->framesPerLine;
		row = EntityList[i].frame / EntityList[i].sprite->framesPerLine;*/

		drawSprite(EntityList[i].sprite,EntityList[i].frame,EntityList[i].position,vec2d(1,1),gt_graphics_get_active_renderer());

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