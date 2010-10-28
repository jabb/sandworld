

#include "sandworld.h"
#include "swobj.h"

static int blank_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	return -1; /* Always returns -1 by default (failure) */
}

struct sw_obj *sw_obj_alloc(void)
{
	struct sw_obj *o = malloc(sizeof(struct sw_obj));
	if (!o)
		return NULL;
	o->type = SW_OBJ_NONE;
	o->attr = SW_ATTR_NONE;
	o->fg = SW_BLACK;
	o->bg = SW_BLACK;
	o->display = ' ';
	o->x = -1;
	o->y = -1;
	o->life = 1;
	o->power = 1;
	o->resist = 0;
	o->tile = NULL;
	o->handle_event = blank_handleevent;
	sw_rucksack_empty(&o->rucksack);
	return o;
}

void sw_obj_free(struct sw_obj *o)
{
	free(o);
}

int sw_obj_getpower(struct sw_obj *o)
{
	int p = o->power;
	if (sw_rucksack_iswielding(&o->rucksack, SW_ITEM_WEAPON))
		p += sw_rucksack_getitemp(&o->rucksack, SW_INHAND_POS)->power;
	return p;
}

int sw_obj_getresist(struct sw_obj *o)
{
	int r = o->resist;
	return r;
}

void sw_obj_takedamage(struct sw_obj *o, int amount)
{
	o->life -= amount;
}

int sw_obj_attack(struct sw_obj *def, struct sw_obj *att)
{
	/* D = rand(a*0.5, a*2) - d*0.5
	 * if (D <= d*0.5)
	 * 	miss();
	 *
	 * a = 10
	 * d = 10
	 * D = rand(5, 20) - 5
	 * D = rand(0, 15)
	 * 0-5 = miss
	 * 6-15 = hit
	 *
	 * a = 20
	 * d = 10
	 * D = rand(10, 40) - 5
	 * D = rand(5, 35)
	 * 0-5 = miss
	 * 6-35 = hit
	 *
	 * a = 10
	 * d = 20
	 * D = rand(5, 20) - 10
	 * D = rand(-5, 10)
	 * -5-10 = miss
	 */
	int power = sw_obj_getpower(att);
	int resist = sw_obj_getresist(def);
	int dmg = sw_randint(power >> 1, power << 1) - (resist >> 1);

	if (dmg <= (resist >> 1))
		return 0;

	if (dmg > 0)
		sw_obj_takedamage(def, dmg);

	return dmg;
}

void sw_obj_draw(struct sw_obj *o, int x, int y)
{
	sw_setfgbg(o->fg, o->bg, o->attr);
	sw_putch(x, y, o->display);
}
