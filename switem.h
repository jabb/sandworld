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
#define SW_ITEM_NONE		0
#define SW_ITEM_WEAPON		(1 << 0)
#define SW_ITEM_TOOL		(1 << 1)
#define SW_ITEM_MATERIAL	(1 << 2)

/* The item!
 */
struct sw_item {
	unsigned long flags;
	int id;
	const char *name;
	int amount;
};

/* Look up table values.
 */
enum {
	/*SW_ITEM_NONE,*/
	SW_ITEM_DIRT=1,
	SW_ITEM_WOOD,
	SW_ITEM_PULVERIZER
};

/* Perhaps later store these into a file?
 */
static struct sw_item sw_item_table[] = {
	/* TYPE			ID	NAME			AMOUNT */
	{  SW_ITEM_NONE, 	0,	"Nothing",		1},
	{  SW_ITEM_MATERIAL, 	1,	"Dirt",			1},
	{  SW_ITEM_MATERIAL, 	2,	"Wood",			1},
	{  SW_ITEM_WEAPON, 	3,	"Pulverizer",		1},
};

/* Create an item based on one of the lookup table values.
 */
struct sw_item sw_item_make(unsigned long flags);
int sw_item_areequal(struct sw_item i1, struct sw_item i2);
int sw_item_isnone(struct sw_item i);
/* Use item types for the flag.
 */
int sw_item_is(struct sw_item i, unsigned long flags);

#endif
