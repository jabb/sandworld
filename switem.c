
#include "sandworld.h"
#include "switem.h"

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
} *create_list = NULL;

static int add_to_itemtable(unsigned long flags, const char *name, int amount,
	int power, int resist, int uses)
{
	struct sw_item item;

	if (items >= ITEM_TABLE_SIZE)
		return -1;

	item.flags = flags;
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

	if (create_list) {
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
	add_to_itemtable(SW_ITEM_TYPE_NONE, "Nothing", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, "Dirt", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, "Dirt Ball", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, "Wood", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_WEAPON, "Pulverizer", 1, 5, 0, 0);

	add_to_createtable(SW_ITEM_NONE, SW_ITEM_DIRT, 2, SW_ITEM_NONE, 0,
		SW_ITEM_DIRTBALL, 1);

	return 0;
}

void sw_item_freetables(void)
{

}

struct sw_item sw_item_gen(unsigned long id)
{
	return item_table[id];
}

int sw_item_areequal(struct sw_item i1, struct sw_item i2)
{
	/* This will be improved as time goes on. */
	return i1.id == i2.id &&
		i1.power == i2.power &&
		i1.resist == i2.resist &&
		i1.max_uses == i2.max_uses &&
		i1.cur_uses == i2.cur_uses;
}

int sw_item_isnone(struct sw_item i)
{
	return i.flags & SW_ITEM_TYPE_NONE;
}

int sw_item_is(struct sw_item i, unsigned long flags)
{
	/* Check if any of the high order bits (type) are set. */
	return i.flags & flags;
}

struct sw_item sw_item_create(struct sw_item tool, struct sw_item on,
	struct sw_item with)
{
	/* TODO: This. :P */

	return sw_item_gen(SW_ITEM_NONE);
}
