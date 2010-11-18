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
#ifndef SWWORLD_H
#define SWWORLD_H

#include "swobj.h"
#include "swtile.h"

struct sw_world {
	int home_x;
	int home_y;
	struct sw_obj *player;
	struct sw_tile tiles[SW_WORLD_WIDTH][SW_WORLD_HEIGHT];
	struct sw_world_link {
		int x;
		int y;
		int dest_x;
		int dest_y;
		struct sw_world *world;
	} *linkto, *linkfrom;
};

struct sw_world *sw_world_alloc(void);
void sw_world_free(struct sw_world *world);

struct sw_tile *sw_world_gettilep(struct sw_world *world, int x, int y);
/* Shortcut (damn lazy programmers) */
#define SW_TILEP(world, x, y) (sw_world_gettilep(world, x, y))
#define SW_OBJP(world, x, y) (SW_TILEP(world, x, y)->object)

void sw_world_draw(struct sw_world *world, int x, int y);

int sw_world_placeobj(struct sw_world *world, struct sw_obj *o, int x, int y);
int sw_world_placeobjhome(struct sw_world *world, struct sw_obj *o);

struct sw_obj *sw_world_removeobj(struct sw_world *world, int x, int y);
void sw_world_freeobj(struct sw_world *world, int x, int y);
void sw_world_freeallobj(struct sw_world *world);

int sw_world_inbounds(struct sw_world *world, int x, int y);

/* These functions trigger object events. These check for out of bounds. */
void sw_world_updateobjs(struct sw_world *world);
void sw_world_moveobjby(struct sw_world *world, int x, int y, int dx, int dy);
void sw_world_moveobjto(struct sw_world *world, int x, int y, int nx, int ny);
void sw_world_interactobj(struct sw_world *world, int x, int y, int nx, int ny);
void sw_world_attackobj(struct sw_world *world, int x, int y, int nx, int ny);
void sw_world_toolobj(struct sw_world *world, int x, int y, int nx, int ny);

void sw_world_nullobj(struct sw_world *world, int x, int y, struct sw_obj *o,
	enum sw_obj_ev ev);

struct sw_world *sw_world_genstart(void);

#endif
