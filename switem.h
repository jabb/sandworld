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
#ifndef SWITEM_H
#define SWITEM_H

/* Item types.
 */
#define SW_ITEM_TYPE_NONE		0
#define SW_ITEM_TYPE_WEAPON		(1 << 0)
#define SW_ITEM_TYPE_TOOL		(1 << 1)
#define SW_ITEM_TYPE_MATERIAL		(1 << 2)

/* Item uses.
 */
#define SW_ITEM_USE_NONE		0
#define SW_ITEM_USE_DIG			(1 << 0)

#define SW_ITEM_NAME_LEN	32

/* The item!
 */
struct sw_item {
	unsigned long type_flags;
	unsigned long use_flags;
	int id;
	char name[SW_ITEM_NAME_LEN];
	int amount;
	int power;
	int resist;
	int max_uses;
	int cur_uses;	/* Durabibility or something. :P */
};

/* Look up table values.
 */
enum {
	SW_ITEM_NONE = 0,
	SW_ITEM_DIRT,
	SW_ITEM_TREESEED,
	SW_ITEM_DIRTBALL,
	SW_ITEM_WOOD,
	SW_ITEM_PULVERIZER
};

int sw_item_alloctables(void);
void sw_item_freetables(void);

/* Create an item based on one of the lookup table values.
 */
struct sw_item sw_item_gen(unsigned long tabid);
int sw_item_areequal(struct sw_item i1, struct sw_item i2);
int sw_item_isnone(struct sw_item i);
/* Use item types for the flag.
 */
int sw_item_istype(struct sw_item i, unsigned long flags);
int sw_item_hasuse(struct sw_item i, unsigned long flags);

struct sw_item sw_item_create(struct sw_item tool, struct sw_item on,
	struct sw_item with);

void sw_item_showstats(struct sw_item i);

#endif
