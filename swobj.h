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
	SW_OBJ_EV_INTERACT,
	SW_OBJ_EV_ATTACK,
	SW_OBJ_EV_TOOL
};

enum sw_obj_type {
	SW_OBJ_NONE,	/* This can not be interacted with at all. */
	SW_OBJ_PLAYER,	/* The player! */
	SW_OBJ_ABYSS,	/* This can be dug into. */
	SW_OBJ_ITEMS,	/* Simple item batch. */
	SW_OBJ_TREE,
	SW_OBJ_BOULDER
};

struct sw_obj {
	enum sw_obj_type type;
	attr_t attr;
	color_t fg;
	color_t bg;
	int display;
	int x;
	int y;
	int life;
	int power;
	int resist;
	struct sw_tile *tile;
	struct sw_rucksack rucksack;
	/* This function returns 0 when a given object event is successful */
	int (*handle_event) (struct sw_world *,
		struct sw_obj *, struct sw_obj *, enum sw_obj_ev);
};

struct sw_obj *sw_obj_alloc(void);
void sw_obj_free(struct sw_obj *o);
struct sw_obj *sw_obj_gentype(enum sw_obj_type type);
int sw_obj_attack(struct sw_obj *def, struct sw_obj *att);
void sw_obj_draw(struct sw_obj *o, int x, int y);
void sw_obj_showstats(struct sw_obj *o);

#endif
