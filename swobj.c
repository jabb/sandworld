
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
	o->tile = NULL;
	o->handle_event = blank_handleevent;
	sw_rucksack_empty(&o->rucksack);
	return o;
}

void sw_obj_free(struct sw_obj *o)
{
	free(o);
}

int sw_obj_attack(struct sw_obj *def, struct sw_obj *att)
{
	/* Possible formula for damage:
	 * D = rand(0, a + d/2) - d/2
	 * where D = damage, a = attack, d = defense
	 */
	def->life -= 1;
	return 1;
}
