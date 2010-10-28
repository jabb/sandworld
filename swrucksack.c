
#include "sandworld.h"
#include "swrucksack.h"
#include "swui.h"

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

int sw_rucksack_iswielding(struct sw_rucksack *rs, unsigned long flag)
{
	return sw_item_is(*SW_ITEMP(rs, SW_INHAND_POS), flag);
}

int sw_rucksack_iswearing(struct sw_rucksack *rs, unsigned long flag)
{
	/* TODO: Implement wearing stuff. */
	return 0;
}

int sw_rucksack_additem(struct sw_rucksack *rs, struct sw_item item)
{
	int i;

	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (sw_item_isnone(*SW_ITEMP(rs, i))) {
			*SW_ITEMP(rs, i) = item;
			return 0;
		}
		/* Stack. TODO: Fix items that are the same only in type. */
		else if (sw_item_areequal(*SW_ITEMP(rs, i), item)) {
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
		if (!sw_item_isnone(*SW_ITEMP(rs, i))) {
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


int sw_rucksack_trans(struct sw_rucksack *rs, struct sw_rucksack *rs2, int pos)
{
	struct sw_item tmp;

	tmp = *SW_ITEMP(rs2, pos);

	if (sw_rucksack_additem(rs, tmp) == 0) {
		sw_rucksack_removeitem(rs2, pos);
		return -1;
	}

	return -1;
}

int sw_rucksack_split(struct sw_rucksack *rs, int pos)
{
	int i;

	/* First pass, check for a second stack of items. */
	for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
		if (i != pos &&
			sw_item_areequal(*SW_ITEMP(rs, i),
				*SW_ITEMP(rs, pos))) {
			SW_ITEMP(rs, pos)->amount -= 1;
			SW_ITEMP(rs, i)->amount += 1;
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
				SW_ITEMP(rs, i)->amount = 1;
				SW_ITEMP(rs, pos)->amount -= 1;
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
			sw_putstr(x + 1, i + y + 1, "\t%s (%d)",
				SW_ITEMP(rs, i)->name,
				SW_ITEMP(rs, i)->amount);
		} else {
			sw_putstr(x + 1, i + y + 1, "\t----");
		}
	}
	sw_box(x, y, SW_COLS/2 - 1, SW_RUCKSACK_SIZE + 2);
}

void sw_rucksack_show(struct sw_rucksack *rs)
{
	int tmp = 0;
	int selected = 0;
	int cmd = 0;
	int i;

	do {
		switch (cmd) {
			case SW_CMD_UP: case SW_CMD_UP2:
				selected--;
				if (selected < 0)
					selected = SW_RUCKSACK_SIZE - 1;
				break;
			case SW_CMD_DOWN: case SW_CMD_DOWN2:
				selected++;
				if (selected >= SW_RUCKSACK_SIZE)
					selected = 0;
				break;
			case SW_CMD_ACTION:
				goto exit;
				break;
			case SW_CMD_SWAP:
				tmp = sw_ui_getnumber(-1, "Swap with?");
				if (tmp >= 0 && tmp < SW_RUCKSACK_SIZE) {
					sw_rucksack_swap(rs, selected, tmp);
				}
				break;
			case SW_CMD_LEFT: case SW_CMD_LEFT2:
			case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
				sw_rucksack_split(rs, selected);
				break;
			case SW_CMD_QUIT:
				return;
			default:
				break;
		}

		sw_rucksack_draw(rs, 0, 0);
		for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
			if (i == selected) {
				sw_setfg(SW_YELLOW);
				sw_putstr(1, i + 1, "%d", i);
			} else {
				sw_setfg(SW_WHITE);
				sw_putstr(1, i + 1, "%d", i);
			}

			if (i == SW_INHAND_POS)
				sw_putstr(3, i + 1, "WEP");
			else if (i == SW_ONSELF_POS)
				sw_putstr(3, i + 1, "ARM");
		}

		cmd = sw_getcmd();
	} while (1);
exit:
	return;
}

void sw_rucksack_compare(struct sw_rucksack *rs, struct sw_rucksack *rs2)
{
	int tmp;
	int which = 1;
	int sel = 0;
	int cmd = 0;
	int i;

	do {
		switch (cmd) {
			case SW_CMD_UP: case SW_CMD_UP2:
				sel--;
				if (sel < 0)
					sel = SW_RUCKSACK_SIZE - 1;
				break;
			case SW_CMD_DOWN: case SW_CMD_DOWN2:
				sel++;
				if (sel >= SW_RUCKSACK_SIZE)
					sel = 0;
				break;
			case SW_CMD_SWITCH:
				which = which == 0 ? 1 : 0;
				break;
			case SW_CMD_ACTION:
				if (sw_rucksack_freeslots(rs) >=
					sw_rucksack_takenslots(rs2)) {
					sw_rucksack_addrucksack(rs, rs2);
					goto exit;
				}
				break;
			case SW_CMD_SWAP:
				tmp = sw_ui_getnumber(-1, "Swap with?");
				if (tmp >= 0 && tmp < SW_RUCKSACK_SIZE) {
					if (which == 0)
						sw_rucksack_swap(rs, sel, tmp);
					else
						sw_rucksack_swap(rs2, sel, tmp);
				}
				break;
			case SW_CMD_LEFT: case SW_CMD_LEFT2:
				if (which == 0) {
					sw_rucksack_split(rs, sel);
				} else {
					sw_rucksack_trans(rs, rs2, sel);
				}
				break;
			case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
				if (which == 0) {
					sw_rucksack_trans(rs2, rs, sel);
				} else {
					sw_rucksack_split(rs2, sel);
				}
				break;
			case SW_CMD_QUIT:
				return;
			default:
				break;
		}

		sw_rucksack_draw(rs, 0, 0);
		sw_rucksack_draw(rs2, SW_COLS/2, 0);
		for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
			/* First rucksack. */
			if (i == sel && which == 0) {
				sw_setfg(SW_YELLOW);
				sw_putstr(1, i + 1, "%d", i);
			} else {
				sw_setfg(SW_WHITE);
				sw_putstr(1, i + 1, "%d", i);
			}

			if (i == SW_INHAND_POS)
				sw_putstr(3, i + 1, "WEP");
			else if (i == SW_ONSELF_POS)
				sw_putstr(3, i + 1, "ARM");

			/* Second rucksack. */
			if (i == sel && which == 1) {
				sw_setfg(SW_YELLOW);
				sw_putstr(SW_COLS/2 + 2, i + 1, "%d", i);
			} else {
				sw_setfg(SW_WHITE);
				sw_putstr(SW_COLS/2 + 2, i + 1, "%d", i);
			}
		}

		cmd = sw_getcmd();
	} while (1);
exit:
	return;
}
