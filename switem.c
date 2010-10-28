
#include "switem.h"


/* Perhaps later store these into a file?
 */
static struct sw_item sw_item_table[] = {
	/* TYPE			ID	NAME			AMOUNT */
	{  SW_ITEM_NONE, 	0,	"Nothing",		1},
	{  SW_ITEM_MATERIAL, 	1,	"Dirt",			1},
	{  SW_ITEM_MATERIAL, 	2,	"Wood",			1},
	{  SW_ITEM_WEAPON, 	3,	"Pulverizer",		1},
};

struct sw_item sw_item_make(unsigned long id)
{
	return sw_item_table[id];
}

int sw_item_areequal(struct sw_item i1, struct sw_item i2)
{
	/* This will be improved as time goes on. */
	return i1.id == i2.id;
}

int sw_item_isnone(struct sw_item i)
{
	return i.flags == SW_ITEM_NONE;
}

int sw_item_is(struct sw_item i, unsigned long flags)
{
	/* Check if any of the high order bits (type) are set. */
	return i.flags & flags;
}
