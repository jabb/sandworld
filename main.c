/* Copyright (c) 2010, Michael Patraw
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of Michael Patraw may not be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Michael Patraw ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Michael Patraw BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Sandworld - A sandbox game with multiple worlds.
 */
#include "sandworld.h"
#include "switem.h"
#include "swlog.h"
#include "swobj.h"
#include "swrucksack.h"
#include "swtile.h"
#include "swui.h"
#include "swworld.h"

int sw_start(void)
{
	int f;
	int b;
	int ret = 0;

	if (!initscr()) {
		ret = SW_ERR_GENERAL;
		goto failure;
	}

	if (noecho() == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (cbreak() == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (curs_set(0) == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (keypad(stdscr, 1) == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	if (start_color() == ERR) {
		ret = SW_ERR_NONCOMP;
		goto failure;
	}

	for (b = 0; b < SW_COLORS; ++b)
		for (f = 0; f < SW_COLORS; ++f)
			/* Don't care if it fails. */
			init_pair(b * SW_COLORS + f + 1, f, b);

	sw_seed(time(NULL));
	sw_item_alloctables();

	goto success;
failure:
	endwin();
success:
	return ret;
}

void sw_stop(void)
{
	sw_item_freetables();
	endwin();
}

/******************************************************************************\
 * ENTRY
\******************************************************************************/

int main(int argc, char *argv[])
{
	int dx;
	int dy;
	int x;
	int y;
	int tmpcmd = 0;
	int tmpnum = 0;
	int cmd = 0;
	int ret;
	struct sw_item tmpitem;
	struct sw_obj *tmpobj = NULL;
	struct sw_world *world = NULL;
	/* TODO: remove this player (world already has one) */
	struct sw_obj *player = NULL;

	ret = sw_start();
	if (ret != 0) {
		sw_logerr("failed to initialize sandworld %d", ret);
		return ret;
	}

	world = sw_world_genstart();
	player = sw_obj_gentype(SW_OBJ_PLAYER);

	sw_world_placeobjhome(world, player);
	sw_rucksack_additem(&player->rucksack, sw_item_gen(SW_ITEM_DIRT));
	sw_rucksack_additem(&player->rucksack, sw_item_gen(SW_ITEM_DIRT));

	do {
		switch (cmd) {
		case SW_CMD_UP: case SW_CMD_UP2:
			/* Fallthrough. */
		case SW_CMD_DOWN: case SW_CMD_DOWN2:
			/* Fallthrough. */
		case SW_CMD_LEFT: case SW_CMD_LEFT2:
			/* Fallthrough. */
		case SW_CMD_RIGHT: case SW_CMD_RIGHT2:
			sw_getdelta(cmd, &dx, &dy);
			sw_world_moveobjby(world, player->x, player->y, dx, dy);
			break;
		case SW_CMD_DROP:
			tmpnum = sw_ui_getnumber(-1, "Drop which item?");
			if (tmpnum >= 0 && tmpnum < SW_RUCKSACK_SIZE) {
				tmpitem = sw_rucksack_removeitem(
					&player->rucksack, tmpnum);
				tmpcmd = sw_ui_getdir("Drop where?");
				if (tmpcmd != SW_CMD_NONE) {
					sw_getdelta(tmpcmd, &dx, &dy);
					tmpobj = sw_obj_gentype(SW_OBJ_ITEMS);
					sw_world_placeobj(world, tmpobj,
						player->x + dx, player->y + dy);

					sw_rucksack_additem(&tmpobj->rucksack,
						tmpitem);
					if (!sw_rucksack_takenslots(
						&tmpobj->rucksack))
						sw_world_freeobj(world,
						player->x + dx, player->y + dy);
				}
			}
			else {
				sw_ui_addalert("No such item.");
			}
			break;
		case SW_CMD_ACTION:
			tmpcmd = sw_ui_getdir("Interact with?");
			if (tmpcmd != SW_CMD_NONE) {
				sw_getdelta(tmpcmd, &dx, &dy);
				sw_world_interactobj(world,
					player->x, player->y,
					player->x + dx, player->y + dy);
			}
			break;
		case SW_CMD_ATTACK:
			tmpcmd = sw_ui_getdir("Attack what?");
			if (tmpcmd != SW_CMD_NONE) {
				sw_getdelta(tmpcmd, &dx, &dy);
				sw_world_attackobj(world,
					player->x, player->y,
					player->x + dx, player->y + dy);
			}
			break;
		case SW_CMD_TOOL:
			tmpcmd = sw_ui_getdir("Use tool on what?");
			if (tmpcmd != SW_CMD_NONE) {
				sw_getdelta(tmpcmd, &dx, &dy);
				sw_world_toolobj(world,
					player->x, player->y,
					player->x + dx, player->y + dy);
			}
			break;
		case SW_CMD_SELF:
			sw_obj_showstats(player);
			break;
		case SW_CMD_RUCKSACK:
			sw_rucksack_show(&player->rucksack);
			break;
		case SW_CMD_CREATE:
			sw_rucksack_create(&player->rucksack);
			break;
		case SW_CMD_INFO:
			tmpcmd = sw_ui_getdir("Info on?");
			if (tmpcmd != SW_CMD_NONE) {
				sw_getdelta(tmpcmd, &dx, &dy);
				x = player->x + dx;
				y = player->y + dy;
				if (sw_world_inbounds(world, x, y) &&
					SW_OBJP(world, x, y))
					sw_obj_showstats(SW_OBJP(world, x, y));
			}
			break;
		case SW_CMD_HELP:
			sw_displayhelp();
			break;
		case SW_CMD_QUIT:
			if (sw_ui_confirm("Are you sure? (y/n)"))
				goto exit;
			break;
		default:
			break;
		}

		sw_world_updateobjs(world);
		sw_world_draw(world, 0, 0);
		sw_ui_draw();
		cmd = sw_getcmd();
	} while (1);

exit:
	sw_stop();

	sw_world_freeallobj(world);
	sw_world_free(world);
	return 0;
}

