

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
	o->name = "?";
	o->x = -1;
	o->y = -1;
	o->cur_life = 1;
	o->max_life = 1;
	o->cur_power = 0;
	o->max_power = 0;
	o->cur_resist = 0;
	o->max_resist = 0;
	o->handle_event = blank_handleevent;
	sw_rucksack_empty(&o->rucksack);
	return o;
}

void sw_obj_free(struct sw_obj *o)
{
	free(o);
}

int sw_obj_isdestroyed(struct sw_obj *o)
{
	return o->cur_life <= 0;
}

int sw_obj_getpower(struct sw_obj *o)
{
	int p = o->cur_power;
	if (sw_rucksack_wieldingis(&o->rucksack, SW_ITEM_TYPE_WEAPON) ||
		sw_rucksack_wieldingis(&o->rucksack, SW_ITEM_TYPE_TOOL))
		p += sw_rucksack_getitemp(&o->rucksack, SW_INHAND_POS)->power;
	return p;
}

int sw_obj_getresist(struct sw_obj *o)
{
	int r = o->cur_resist;
	return r;
}

void sw_obj_takedamage(struct sw_obj *o, int amount)
{
	o->cur_life -= amount;
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
	sw_setfg(SW_BLUE);
	sw_clearinfo();
	sw_addinfo("%s", o->name);
	sw_addinfo("--------------------------------");
	sw_addinfo("Life: %d / %d", o->cur_life, o->max_life);
	sw_addinfo("Power: %d (%d to %d)", sw_obj_getpower(o), sw_obj_dmgmin(o),
		sw_obj_dmgmax(o));
	sw_addinfo("Resist: %d", sw_obj_getresist(o));
	sw_infobox(0, 0);
}
