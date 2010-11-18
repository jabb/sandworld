
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

/* Shortcut. */
#define ITEM_NONE sw_item_gen(SW_ITEM_NONE)

/* Item creation stuff. */
struct create_node {
	struct sw_item tool;
	struct sw_item on;
	struct sw_item with;
};

#define LH_K struct create_node *
#define LH_V struct sw_item
#include "linear_hash.impl"

static unsigned long ct_hashkfn(struct create_node *node, unsigned long size)
{
	unsigned long hash = 0;
	hash += node->tool.id * 2654435761;
	hash += node->tool.amount * 2654435761;
	hash += node->on.id * 2654435761;
	hash += node->on.amount * 2654435761;
	hash += node->with.id * 2654435761;
	hash += node->with.amount * 2654435761;
	return hash % size;
}

static int ct_compkfn(struct create_node *node1, struct create_node *node2)
{
	/* We're only concerned with the amount and type. */
	int test1, test2, test3;

	test1 = node1->tool.id == node2->tool.id &&
		node1->tool.amount == node2->tool.amount;
	test2 = node1->on.id == node2->on.id &&
		node1->on.amount == node2->on.amount;
	test3 = node1->with.id == node2->with.id &&
		node1->with.amount == node2->with.amount;
	return (test1 && test2 && test3) ? 0 : -1;
}

static struct create_node *ct_copykfn(struct create_node *node)
{
	struct create_node *newnode = malloc(sizeof(struct create_node));
	newnode->tool = node->tool;
	newnode->on = node->on;
	newnode->with = node->with;
	return newnode;
}

struct linear_hash *creation_table = NULL;

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

static int add_to_createtable(struct sw_item tool, struct sw_item on,
	struct sw_item with, struct sw_item result)
{
	struct create_node node;
	node.tool = tool;
	node.on = on;
	node.with = with;
	lh_add(creation_table, &node, result);

	return 1;
}

int sw_item_alloctables(void)
{
	/* Items must be allocated before the item creation table. */
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

	/* Allocate the item creation table. */
	lh_hashkfn = ct_hashkfn;
	lh_compkfn = ct_compkfn;
	lh_copykfn = ct_copykfn;
	creation_table = lh_alloc(32);

	add_to_createtable(
		sw_item_genamount(SW_ITEM_DIRT, 2),
		ITEM_NONE,
		ITEM_NONE,
		sw_item_genamount(SW_ITEM_DIRTBALL, 2));

	sw_logmsg("allocated item tables successfully");
	return 0;
}

void sw_item_freetables(void)
{
	lh_free(creation_table);
}

struct sw_item sw_item_gen(unsigned long tabid)
{
	return sw_item_genamount(tabid, 1);
}

struct sw_item sw_item_genamount(unsigned long tabid, int amount)
{
	struct sw_item rv = item_table[tabid];
	if (tabid != SW_ITEM_NONE)
		sw_logmsg("generated item %d: \"%s\"", rv.id, rv.name);
	rv.amount = amount;
	return rv;
}

int sw_item_areequal(struct sw_item i1, struct sw_item i2)
{
	/* This will be improved as time goes on. */
	return i1.id == i2.id;
}

int sw_item_arestackable(struct sw_item i1, struct sw_item i2)
{
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
	struct create_node node;
	struct sw_item rv = ITEM_NONE;
	node.tool = tool;
	node.on = on;
	node.with = with;
	sw_logmsg("HASH: %lu\n", ct_hashkfn(&node, 64));
	if (lh_exists(creation_table, &node))
		rv = lh_get(creation_table, &node);

	return rv;
}

void sw_item_showstats(struct sw_item i)
{
	sw_setfg(SW_BLUE);
	sw_clearinfo();
	sw_addinfo("%s", i.name);
	sw_addinfo("--------------------------------");
	sw_addinfo("Power: %d", i.power);
	sw_addinfo("Resist: %d", i.resist);
	sw_infobox(0, 0);
}
