
#include "switem.h"

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
