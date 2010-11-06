
#include "sandworld.h"
#include "switem.h"
#include "swlog.h"

#include <stdlib.h>
#include <string.h>

#define ITEM_TABLE_SIZE	32

/* Perhaps later store these into a file?
 */
static struct sw_item item_table[ITEM_TABLE_SIZE];
static int items = 0;

struct create_node {
	int toolid;
	int onid;
	int onamount;
	int withid;
	int withamount;
	int resid;
	int resamount;
	struct create_node *next;
};

static struct create_node *create_list = NULL;

static int add_to_itemtable(unsigned long tflags, unsigned long uflags,
	const char *name, int amount, int power, int resist, int uses)
{
	struct sw_item item;

	if (items >= ITEM_TABLE_SIZE)
		return -1;

	item.type_flags = tflags;
	item.use_flags = uflags;
	item.id = items; /* Assign as we go... */
	strncpy(item.name, name, SW_ITEM_NAME_LEN);
	item.amount = amount;
	item.power = power;
	item.resist = resist;
	item.max_uses = uses;
	item.cur_uses = uses;

	item_table[items++] = item;

	return 0;
}

static int add_to_createtable(int toolid, int onid, int onamount, int withid,
	int withamount, int resid, int resamount)
{
	struct create_node *iter = NULL;

	if (!create_list) {
		create_list = malloc(sizeof(struct create_node));

		if (!create_list)
			return SW_ERR_NOMEM;

		create_list->toolid = toolid;
		create_list->onid = onid;
		create_list->onamount = onamount;
		create_list->withid = withid;
		create_list->withamount = withamount;
		create_list->resid = resid;
		create_list->resamount = resamount;
		create_list->next = NULL;
	} else {
		iter = create_list;
		while (iter->next)
			iter = iter->next;

		iter->next = malloc(sizeof(struct create_node));

		if (!iter->next)
			return SW_ERR_NOMEM;

		iter->next->toolid = toolid;
		iter->next->onid = onid;
		iter->next->onamount = onamount;
		iter->next->withid = withid;
		iter->next->withamount = withamount;
		iter->next->resid = resid;
		iter->next->resamount = resamount;
		iter->next->next = NULL;
	}

	return 0;
}

int sw_item_alloctables(void)
{
	add_to_itemtable(SW_ITEM_TYPE_NONE, SW_ITEM_USE_NONE,
		"Nothing", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, SW_ITEM_USE_NONE,
		 "Dirt", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, SW_ITEM_USE_NONE,
		 "Tree Seed", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, SW_ITEM_USE_NONE,
		 "Dirt Ball", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, SW_ITEM_USE_NONE,
		 "Wood", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_WEAPON, SW_ITEM_USE_NONE,
		 "Pulverizer", 1, 5, 0, 0);

	add_to_createtable(SW_ITEM_NONE, SW_ITEM_DIRT, 2, SW_ITEM_NONE, 0,
		SW_ITEM_DIRTBALL, 1);

	sw_logmsg("allocated item tables successfully");
	return 0;
}

void sw_item_freetables(void)
{

}

struct sw_item sw_item_gen(unsigned long id)
{
	sw_logmsg("generated item %d: \"%s\"", item_table[id].id, item_table[id].name);
	return item_table[id];
}

int sw_item_areequal(struct sw_item i1, struct sw_item i2)
{
	/* This will be improved as time goes on. */
	return i1.id == i2.id &&
		i1.max_uses == i2.max_uses &&
		i1.cur_uses == i2.cur_uses;
}

int sw_item_isnone(struct sw_item i)
{
	return i.id == SW_ITEM_NONE;
}

int sw_item_istype(struct sw_item i, unsigned long flags)
{
	if (i.type_flags == SW_ITEM_TYPE_NONE)
		return 0;
	return i.type_flags & flags;
}

int sw_item_hasuse(struct sw_item i, unsigned long flags)
{
	if (i.use_flags == SW_ITEM_USE_NONE)
		return 0;
	return i.use_flags & flags;
}

struct sw_item sw_item_create(struct sw_item tool, struct sw_item on,
	struct sw_item with)
{
	/* TODO: This. :P */

	return sw_item_gen(SW_ITEM_NONE);
}

void sw_item_draw(struct sw_item i, int x, int y)
{
	sw_clearinfo();
	sw_logmsg("%d %d %s", i.power, i.resist, i.name);
	sw_addinfo("%s", i.name);
	sw_addinfo("Power: %d", i.power);
	sw_addinfo("Resist: %d", i.resist);
	sw_infobox(x, y);
}
