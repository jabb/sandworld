

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
	int power = sw_obj_getpower(att);
	int resist = sw_obj_getresist(def);
	int dmg = sw_randint(power * 0.5, power * 1.5) + (power - resist);

	if (dmg > 0)
		sw_obj_takedamage(def, dmg);

	return dmg <= 0 ? 0 : dmg;
}

int sw_obj_dmgmin(struct sw_obj *o)
{
	/* Assume power and defense are equal. */
	int power = sw_obj_getpower(o);
	return power * 0.5;
}

int sw_obj_dmgmax(struct sw_obj *o)
{
	/* Assume power and defense are equal. */
	int power = sw_obj_getpower(o);
	return power * 1.5;
}

void sw_obj_draw(struct sw_obj *o, int x, int y)
{
	sw_setfgbg(o->fg, o->bg, o->attr);
	sw_putch(x, y, o->display);
}

void sw_obj_showstats(struct sw_obj *o)
{
	sw_setfg(SW_BLACK);
	sw_clearlineto(1, 0, SW_COLS/2 - 1);
	sw_clearlineto(2, 0, SW_COLS/2 - 1);
	sw_clearlineto(3, 0, SW_COLS/2 - 1);
	sw_setfg(SW_WHITE);
	sw_putstr(1, 1, "Life: %d", o->life);
	sw_putstr(1, 2, "Power: %d (%d to %d)", sw_obj_getpower(o),
		sw_obj_dmgmin(o), sw_obj_dmgmax(o));
	sw_putstr(1, 3, "Resist: %d", sw_obj_getresist(o));
	sw_box(0, 0, SW_COLS/2 - 1, 3 + 2);

	sw_getcmd();
}