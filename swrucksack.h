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
/*
 * TODO: Make rucksacks more memory efficient. Right now they're huge
 * hogs.
 */
#ifndef SWRUCKSACK_H
#define SWRUCKSACK_H

#include "switem.h"

/* Max size for a rucksack.
 */
#define SW_RUCKSACK_SIZE 16

/* These positions in the rucksack have special meaning.
 */
#define SW_INHAND_POS 0
#define SW_ONSELF_POS 1

struct sw_rucksack {
	struct sw_item items[SW_RUCKSACK_SIZE];
};

struct sw_item *sw_rucksack_getitemp(struct sw_rucksack *rs, int pos);
/* Shortcut (damn lazy programmers) */
#define SW_ITEMP(rs, pos) (sw_rucksack_getitemp(rs, pos))

int sw_rucksack_empty(struct sw_rucksack *rs);

int sw_rucksack_freeslots(struct sw_rucksack *rs);
int sw_rucksack_takenslots(struct sw_rucksack *rs);

struct sw_item *sw_rucksack_wielding(struct sw_rucksack *rs);
struct sw_item *sw_rucksack_wearing(struct sw_rucksack *rs);
int sw_rucksack_wieldingcan(struct sw_rucksack *rs, unsigned long flags);
int sw_rucksack_wearingcan(struct sw_rucksack *rs, unsigned long flags);
int sw_rucksack_wieldingis(struct sw_rucksack *rs, unsigned long flags);
int sw_rucksack_wearingis(struct sw_rucksack *rs, unsigned long flags);

int sw_rucksack_additem(struct sw_rucksack *rs, struct sw_item item);
int sw_rucksack_addrucksack(struct sw_rucksack *rs, struct sw_rucksack *rs2);

void sw_rucksack_swap(struct sw_rucksack *rs, int pos1, int pos2);
int sw_rucksack_trans(struct sw_rucksack *rs, struct sw_rucksack *rs2, int pos);
int sw_rucksack_split(struct sw_rucksack *rs, int pos);

struct sw_item sw_rucksack_removeitem(struct sw_rucksack *rs, int pos);

void sw_rucksack_draw(struct sw_rucksack *rs, int x, int y);
void sw_rucksack_show(struct sw_rucksack *rs);
void sw_rucksack_compare(struct sw_rucksack *rs, struct sw_rucksack *rs2);
void sw_rucksack_create(struct sw_rucksack *rs);

#endif
