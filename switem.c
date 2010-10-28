
#include "switem.h"

#include <string.h>

#define ITEM_TABLE_SIZE	32

/* Perhaps later store these into a file?
 */
static struct sw_item item_table[ITEM_TABLE_SIZE];
static int items = 0;

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
	item.max_power = power;
	item.cur_power = power;
	item.max_resist = resist;
	item.cur_resist = resist;
	item.max_uses = uses;
	item.cur_uses = uses;

	item_table[items++] = item;

	return 0;
}
#include <assert.h>
int sw_item_inittable(void)
{
	add_to_itemtable(SW_ITEM_TYPE_NONE, "Nothing", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, "Dirt", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_MATERIAL, "Wood", 1, 0, 0, 0);
	add_to_itemtable(SW_ITEM_TYPE_WEAPON, "Pulverizer", 1, 5, 0, 0);

	return 0;
}

struct sw_item sw_item_gen(unsigned long id)
{
	return item_table[id];
}

int sw_item_areequal(struct sw_item i1, struct sw_item i2)
{
	/* This will be improved as time goes on. */
	return i1.id == i2.id &&
		i1.max_power == i2.max_power &&
		i1.cur_power == i2.cur_power &&
		i1.max_resist == i2.max_resist &&
		i1.cur_resist == i2.cur_resist &&
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
