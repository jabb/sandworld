
#include "sandworld.h"
#include "swrucksack.h"

struct sw_item *sw_rucksack_getitemp(struct sw_rucksack *rs, int pos)
{
	return &rs->items[pos];
}

int sw_rucksack_empty(struct sw_rucksack *rs)
{
	int i;
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i)
		*SW_ITEMP(rs, i) = sw_item_gen(SW_ITEM_NONE);
	return 0;
}

int sw_rucksack_freeslots(struct sw_rucksack *rs)
{
	int i;
	int count = 0;
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i)
		if (sw_item_isnone(*SW_ITEMP(rs, i)))
			count ++;
	return count;
}

int sw_rucksack_takenslots(struct sw_rucksack *rs)
{
	return SW_RUCKSACK_SIZE - sw_rucksack_freeslots(rs);
}

struct sw_item *sw_rucksack_wielding(struct sw_rucksack *rs)
{
	return SW_ITEMP(rs, SW_ONSELF_POS);
}

struct sw_item *sw_rucksack_wearing(struct sw_rucksack *rs)
{
	return SW_ITEMP(rs, SW_ONSELF_POS);
}

int sw_rucksack_wieldingcan(struct sw_rucksack *rs, unsigned long flags)
{
	return sw_item_istype(*SW_ITEMP(rs, SW_INHAND_POS), flags);
}

int sw_rucksack_wearingcan(struct sw_rucksack *rs, unsigned long flags)
{
	return sw_item_istype(*SW_ITEMP(rs, SW_ONSELF_POS), flags);
}

int sw_rucksack_wieldingis(struct sw_rucksack *rs, unsigned long flags)
{
	return sw_item_istype(*SW_ITEMP(rs, SW_INHAND_POS), flags);
}

int sw_rucksack_wearingis(struct sw_rucksack *rs, unsigned long flags)
{
	return sw_item_hasuse(*SW_ITEMP(rs, SW_ONSELF_POS), flags);
}

int sw_rucksack_additem(struct sw_rucksack *rs, struct sw_item item)
{
	int i;

	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (sw_item_isnone(*SW_ITEMP(rs, i))) {
			*SW_ITEMP(rs, i) = item;
			return 0;
		}
		else if (sw_item_arestackable(*SW_ITEMP(rs, i), item)) {
			SW_ITEMP(rs, i)->amount += item.amount;
			return 0;
		}
	}

	return -1; /* Couldn't find a place for the item. */
}

int sw_rucksack_addrucksack(struct sw_rucksack *rs, struct sw_rucksack *rs2)
{
	int i;

	if (sw_rucksack_freeslots(rs) < sw_rucksack_takenslots(rs2))
		return -1;

	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (!sw_item_isnone(*SW_ITEMP(rs2, i))) {
			sw_rucksack_additem(rs, *SW_ITEMP(rs2, i));
			*SW_ITEMP(rs2, i) = sw_item_gen(SW_ITEM_NONE);
		}
	}

	return 0;
}

void sw_rucksack_swap(struct sw_rucksack *rs, int pos1, int pos2)
{
	struct sw_item tmp = *SW_ITEMP(rs, pos2);
	*SW_ITEMP(rs, pos2) = *SW_ITEMP(rs, pos1);
	*SW_ITEMP(rs, pos1) = tmp;
}


int sw_rucksack_trans(struct sw_rucksack *dst, struct sw_rucksack *src, int pos)
{
	struct sw_item tmp;

	tmp = *SW_ITEMP(src, pos);

	if (sw_rucksack_additem(dst, tmp) == 0) {
		sw_rucksack_removeitem(src, pos);
		return -1;
	}

	return -1;
}

int sw_rucksack_split(struct sw_rucksack *rs, int pos)
{
	return sw_rucksack_splitn(rs, pos, 1);
}

int sw_rucksack_splitn(struct sw_rucksack *rs, int pos, int num)
{
	int i;

	if (SW_ITEMP(rs, pos)->amount < num)
		return -1;

	/* First pass, check for a second stack of items. */
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (i != pos && sw_item_areequal(*SW_ITEMP(rs, i),
				*SW_ITEMP(rs, pos))) {
			SW_ITEMP(rs, pos)->amount -= num;
			SW_ITEMP(rs, i)->amount += num;
			if (SW_ITEMP(rs, pos)->amount <= 0)
				sw_rucksack_removeitem(rs, pos);
			return 0;
		}
	}

	/* Second pass, just add the item to an empty space. */
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (!sw_item_areequal(*SW_ITEMP(rs, i), *SW_ITEMP(rs, pos))) {
			if (sw_item_isnone(*SW_ITEMP(rs, i))) {
				*SW_ITEMP(rs, i) = rs->items[pos];
				SW_ITEMP(rs, i)->amount = num;
				SW_ITEMP(rs, pos)->amount -= num;
				if (SW_ITEMP(rs, pos)->amount <= 0)
					sw_rucksack_removeitem(rs, pos);
				return 0;
			}
		}
	}

	return -1;
}

struct sw_item sw_rucksack_removeitem(struct sw_rucksack *rs, int pos)
{
	struct sw_item item;
	item = *SW_ITEMP(rs, pos);
	*SW_ITEMP(rs, pos) = sw_item_gen(SW_ITEM_NONE);
	return item;
}

void sw_rucksack_draw(struct sw_rucksack *rs, int x, int y)
{
	int i;

	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		sw_setfg(SW_BLACK);
		sw_clearlineto(i + 1, x, x + SW_COLS/2 - 1);
		sw_setfg(SW_WHITE);
		/* The '\t' is just arbitrary padding */
		if (!sw_item_isnone(*SW_ITEMP(rs, i))) {
			sw_putstr(x + 1, i + y + 1, "\t %s (%d)",
				SW_ITEMP(rs, i)->name,
				SW_ITEMP(rs, i)->amount);
		} else {
			sw_putstr(x + 1, i + y + 1, "\t ----");
		}
	}
	sw_box(x, y, SW_COLS/2 - 1, SW_RUCKSACK_SIZE + 2);
}
