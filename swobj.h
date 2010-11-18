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
#ifndef SWOBJ_H
#define SWOBJ_H

#include "sandworld.h"
#include "swrucksack.h"

struct sw_world;

enum sw_obj_ev {
	SW_OBJ_EV_UPDATE,
	SW_OBJ_EV_MOVE,
	SW_OBJ_EV_ATTACK,
	SW_OBJ_EV_SWAP,
	SW_OBJ_EV_SKILL,
	SW_OBJ_EV_TOOL,
	SW_OBJ_EV_INTERACT
};

enum sw_obj_type {
	/* Special objects. */
	SW_OBJ_NONE,	/* This can not be interacted with at all. */
	SW_OBJ_PLAYER,	/* The player! */

	/* Basic objects able to be dug into. */
	SW_OBJ_DIRT,
	SW_OBJ_CLAY,
	SW_OBJ_STONE,

	/* Basic core objects like items. */
	SW_OBJ_ITEMS,

	/* Other objects. */
	SW_OBJ_TREE,
	SW_OBJ_BOULDER,
	SW_OBJ_BUSH
};

struct sw_obj {
	enum sw_obj_type type;
	attr_t attr;
	color_t fg;
	color_t bg;
	/* TODO: convert this to an array. */
	const char *name;
	int display;
	int x;
	int y;
	int max_life;
	int cur_life;
	int max_power;
	int cur_power;
	int max_resist;
	int cur_resist;
	struct sw_rucksack rucksack;
	/* This function returns 0 when a given object event is successful */
	int (*handle_event) (struct sw_world *,
		struct sw_obj *, struct sw_obj *, enum sw_obj_ev);
};

struct sw_obj *sw_obj_alloc(void);
struct sw_obj *sw_obj_genfromitem(struct sw_item item);
void sw_obj_free(struct sw_obj *o);
struct sw_obj *sw_obj_gen(enum sw_obj_type type);
int sw_obj_isdestroyed(struct sw_obj *o);
int sw_obj_getpower(struct sw_obj *o);
int sw_obj_getresist(struct sw_obj *o);
void sw_obj_takedamage(struct sw_obj *o, int amount);
int sw_obj_attack(struct sw_obj *def, struct sw_obj *att);
int sw_obj_dmgmin(struct sw_obj *o);
int sw_obj_dmgmax(struct sw_obj *o);
void sw_obj_draw(struct sw_obj *o, int x, int y);
void sw_obj_showstats(struct sw_obj *o);

#endif
