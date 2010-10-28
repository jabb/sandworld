
#include "swobj.h"
#include "swui.h"
#include "swworld.h"

static int items_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("Space->Direction to pick up items.");
		return -1; /* Return -1 saying, "you can't move here yet" */
	/* Interacting with an item attempts pickup. */
	case SW_OBJ_EV_INTERACT:
		sw_rucksack_compare(&from->rucksack, &self->rucksack);
		if (sw_rucksack_takenslots(&self->rucksack) == 0)
			sw_world_freeobj(world, self->x, self->y);
		return 0; /* Object interaction always succeeeds? */
	/* Attacking an item destroys it. */
	case SW_OBJ_EV_ATTACK:
		sw_obj_attack(self, from);
		sw_ui_addalert("You stomp on the items.");
		if (self->life <= 0) {
			sw_ui_addalert("They are smushed.");
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
		}
		return 0;
	default:
		return -1;
	}
	return -1;
}

static int abyss_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("You cannot move there!");
		return -1; /* Return -1 saying, "you can't move here yet" */
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("You need to use a tool on abyss.");
		return 0; /* Object interaction always succeeeds? */
	case SW_OBJ_EV_TOOL:
		x = self->x;
		y = self->y;
		sw_world_freeobj(world, x, y);
		sw_world_placeobj(world, sw_obj_gentype(SW_OBJ_ITEMS), x, y);
		return 0; /* Dig succeeded. */
	case SW_OBJ_EV_ATTACK:
		sw_obj_attack(self, from);
		if (self->life <= 0) {
			sw_ui_addalert("Abyss is tough stuff, but you're "
				"tougher.");
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
		}
		else {
			sw_ui_addalert("I dunno... do you really want to attack"
				" abyss?");
		}
	default:
		return -1;
	}

	return -1;
}

static int tree_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	struct sw_obj *tmpobj = NULL;
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("You cannot move there!");
		return -1; /* Return -1 saying, "you can't move here yet" */
	case SW_OBJ_EV_ATTACK:
		sw_obj_attack(self, from);
		if (self->life <= 0) {
			sw_ui_addalert("You deal a feirce blow to the tree!");
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
			tmpobj = sw_obj_gentype(SW_OBJ_ITEMS);
			sw_rucksack_additem(&tmpobj->rucksack,
				sw_item_make(SW_ITEM_WOOD));
			sw_world_placeobj(world, tmpobj, x, y);
		}
		else {
			sw_ui_addalert("Bark IS stronger than bite!");
		}
		return 0;
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Nice tree. Kinda mangly.");
		return 0; /* Object interaction always succeeeds? */
	default:
		return -1;
	}
	return -1;
}

static int boulder_handleevent(struct sw_world *world,
	struct sw_obj *self, struct sw_obj *from, enum sw_obj_ev ev)
{
	struct sw_obj *tmpobj = NULL;
	int x;
	int y;

	switch (ev) {
	case SW_OBJ_EV_MOVE:
		sw_ui_addalert("You cannot move there!");
		return -1; /* Return -1 saying, "you can't move here yet" */
	case SW_OBJ_EV_INTERACT:
		sw_ui_addalert("Mighty big boulder there.");
		return 0; /* Object interaction always succeeeds? */
	case SW_OBJ_EV_ATTACK:
		sw_obj_attack(self, from);
		if (self->life <= 0) {
			sw_ui_addalert("You injure yourself on the boulder,"
				"but it cracks.");
			x = self->x;
			y = self->y;
			sw_world_freeobj(world, self->x, self->y);
			tmpobj = sw_obj_gentype(SW_OBJ_ITEMS);
			sw_rucksack_additem(&tmpobj->rucksack,
				sw_item_make(SW_ITEM_DIRT));
			sw_world_placeobj(world, tmpobj, x, y);
		}
		else {
			sw_ui_addalert("You smack the boulder helplessly.");
		}
		return 0;
	default:
		return -1;
	}
	return -1;
}

struct sw_obj *sw_obj_gentype(enum sw_obj_type type)
{
	struct sw_obj *o = sw_obj_alloc();

	if (!o)
		return NULL;

	switch (type) {
	case SW_OBJ_ABYSS:
		o->type = SW_OBJ_ABYSS;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_BLACK;
		o->display = ';';
		o->handle_event = abyss_handleevent;
		break;
	case SW_OBJ_PLAYER:
		o->type = SW_OBJ_PLAYER;
		o->attr = SW_ATTR_NONE;
		o->fg = SW_RED;
		o->display = '@';
		break;
	case SW_OBJ_ITEMS:
		o->type = SW_OBJ_ITEMS;
		o->fg = SW_YELLOW;
		o->display = ',';
		o->handle_event = items_handleevent;
		break;
	case SW_OBJ_TREE:
		o->type = SW_OBJ_TREE;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_GREEN;
		o->display = 'T';
		o->handle_event = tree_handleevent;
		break;
	case SW_OBJ_BOULDER:
		o->type = SW_OBJ_BOULDER;
		o->attr = SW_ATTR_BRIGHT;
		o->fg = SW_BLACK;
		o->display = 'o';
		o->life = 2;
		o->resist = 3;
		o->handle_event = boulder_handleevent;
		break;
	default:
		break;
	}

	return o;
}
