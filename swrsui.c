
#include "swrucksack.h"
#include "swui.h"
#include "swworld.h"

void sw_rsui_show(struct sw_world *world, struct sw_rucksack *rs)
{
	int tmp = 0;
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
			case SW_CMD_ACTION: case SW_CMD_ACTION2:
				sw_ui_clearmenu();
				sw_ui_addmenuheader(SW_ITEMP(rs, sel)->name);
				sw_ui_addmenu("Info");
				sw_ui_addmenu("Destroy");
				sw_ui_addmenu("Place");
				sw_ui_addmenu("Split");
				sw_ui_addmenu("Swap");
				sw_ui_addmenu("Equip as Weapon");
				sw_ui_addmenu("Equip as Armor");
				tmp = sw_ui_menubox(0, 0);
				if (tmp == 0) {
					sw_item_showstats(*SW_ITEMP(rs, sel));
				} else if (tmp == 1) {
					sw_rucksack_removeitem(rs, sel);
				} else if (tmp == 2) {
					if (sw_item_hasuse(*SW_ITEMP(rs, sel),
						SW_ITEM_USE_PLACE)) {
						tmp = sw_ui_getdir("Where?");

					} else {
						sw_ui_addalert(
							"You can't place %s!\n",
							SW_ITEMP(rs, sel)->name
							);
					}
				} else if (tmp == 3) {
					tmp = sw_ui_getnumber(0, "How many?");
					sw_rucksack_splitn(rs, sel, tmp);
				} else if (tmp == 4) {
					tmp = sw_ui_getnumber(-1, "Swap with?");
					if (tmp >= 0 &&
						tmp < SW_RUCKSACK_SIZE) {
						sw_rucksack_swap(rs, sel, tmp);
					}
				} else if (tmp == 5) {
					sw_rucksack_swap(rs, sel,
						SW_INHAND_POS);
				} else if (tmp == 6) {
					sw_rucksack_swap(rs, sel,
						SW_ONSELF_POS);
				}
				break;
			case SW_CMD_QUIT: case SW_CMD_QUIT2:
				return;
			default:
				break;
		}

		sw_rucksack_draw(rs, 0, 0);
		for (i = 0; i < SW_RUCKSACK_SIZE; ++i) {
			if (i == sel) {
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

		sw_ui_draw();
		cmd = sw_getcmd();
	} while (1);

	return;
}

void sw_rsui_compare(struct sw_world *world, struct sw_rucksack *rs,
	struct sw_rucksack *rs2)
{
	int tmp;
	struct sw_rucksack *rsp = NULL;
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
			case SW_CMD_LEFT: case SW_CMD_LEFT2:
			case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
				which = which == 0 ? 1 : 0;
				break;
			case SW_CMD_ACTION: case SW_CMD_ACTION2:
				rsp = which ? rs2 : rs;
				sw_ui_clearmenu();
				sw_ui_addmenuheader(SW_ITEMP(rsp, sel)->name);
				sw_ui_addmenu("Info");
				sw_ui_addmenu("Destroy");
				sw_ui_addmenu("Split");
				sw_ui_addmenu("Swap");
				sw_ui_addmenu("Transfer");
				sw_ui_addmenu("Transfer All");
				tmp = sw_ui_menubox(0, 0);
				if (tmp == 0) {
					sw_item_showstats(*SW_ITEMP(rsp, sel));
				} else if (tmp == 1) {
					sw_rucksack_removeitem(rsp, sel);
				} else if (tmp == 2) {
					tmp = sw_ui_getnumber(0, "How many?");
					sw_rucksack_splitn(rsp, sel, tmp);
				} else if (tmp == 3) {
					tmp = sw_ui_getnumber(-1, "Swap with?");
					if (tmp >= 0 &&
						tmp < SW_RUCKSACK_SIZE) {
						sw_rucksack_swap(rsp, sel, tmp);
					}
				} else if (tmp == 4) {
					if (which == 0)
						sw_rucksack_trans(rs2, rs, sel);
					else
						sw_rucksack_trans(rs, rs2, sel);
				} else if (tmp == 5) {
					if (sw_rucksack_addrucksack(rs, rs2)
						== 0) {
						return;
					}
				}
				break;
			case SW_CMD_QUIT: case SW_CMD_QUIT2:
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

	return;
}

void sw_rsui_create(struct sw_world *world, struct sw_rucksack *rs)
{
	int tmp;
	int which = 0;
	int sel = 0;
	int cmd = 0;
	int i;
	struct sw_item tmpitem;
	struct sw_rucksack tr;
	struct sw_rucksack *rsp = NULL;
	sw_rucksack_empty(&tr);

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
			case SW_CMD_LEFT: case SW_CMD_LEFT2:
			case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
				which = which == 0 ? 1 : 0;
				break;
			case SW_CMD_ACTION: case SW_CMD_ACTION2:
				rsp = which ? &tr : rs;
				sw_ui_clearmenu();
				sw_ui_addmenuheader(SW_ITEMP(rs, sel)->name);
				sw_ui_addmenu("Info");
				sw_ui_addmenu("Destroy");
				sw_ui_addmenu("Split");
				sw_ui_addmenu("Swap");
				sw_ui_addmenu("Transfer");
				sw_ui_addmenu("CREATE");
				tmp = sw_ui_menubox(0, 0);
				if (tmp == 0) {
					sw_item_showstats(*SW_ITEMP(rsp, sel));
				} else if (tmp == 1) {
					sw_rucksack_removeitem(rsp, sel);
				} else if (tmp == 2) {
					tmp = sw_ui_getnumber(0, "How many?");
					sw_rucksack_splitn(rsp, sel, tmp);
				} else if (tmp == 3) {
					tmp = sw_ui_getnumber(-1, "Swap with?");
					if (tmp >= 0 &&
						tmp < SW_RUCKSACK_SIZE) {
						sw_rucksack_swap(rsp, sel, tmp);
					}
				} else if (tmp == 4) {
					if (which == 0)
						sw_rucksack_trans(&tr, rs, sel);
					else
						sw_rucksack_trans(rs, &tr, sel);
				} else if (tmp == 5) {
					tmpitem = sw_item_create(
						*SW_ITEMP(&tr, 0),
						*SW_ITEMP(&tr, 1),
						*SW_ITEMP(&tr, 2));
					if (!sw_item_isnone(tmpitem)) {
						*SW_ITEMP(&tr, 3) = tmpitem;
						sw_rucksack_removeitem(&tr, 0);
						sw_rucksack_removeitem(&tr, 1);
						sw_rucksack_removeitem(&tr, 2);
					}
				}
				break;
			case SW_CMD_QUIT: case SW_CMD_QUIT2:
				return;
			default:
				break;
		}

		sw_rucksack_draw(rs, 0, 0);
		sw_rucksack_draw(&tr, SW_COLS/2, 0);
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

			if (i == 0)
				sw_putstr(SW_COLS/2 + 4, i + 1, "TOOL");
			else if (i == 1)
				sw_putstr(SW_COLS/2 + 4, i + 1, "ON");
			else if (i == 2)
				sw_putstr(SW_COLS/2 + 4, i + 1, "WITH");
			else if (i == 3)
				sw_putstr(SW_COLS/2 + 4, i + 1, "RES");
		}

		cmd = sw_getcmd();
	} while (1);

	return;
}
