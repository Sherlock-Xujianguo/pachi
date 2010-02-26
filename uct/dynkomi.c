#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "debug.h"
#include "uct/dynkomi.h"
#include "uct/internal.h"
#include "uct/tree.h"
#include "uct/walk.h"


static void
uct_dynkomi_generic_done(struct uct_dynkomi *d)
{
	if (d->data) free(d->data);
	free(d);
}


/* NONE dynkomi strategy - never fiddle with komi values. */

struct uct_dynkomi *
uct_dynkomi_init_none(struct uct *u, char *arg, struct board *b)
{
	struct uct_dynkomi *d = calloc(1, sizeof(*d));
	d->uct = u;
	d->permove = NULL;
	d->persim = NULL;
	d->done = uct_dynkomi_generic_done;
	d->data = NULL;

	if (arg) {
		fprintf(stderr, "uct: Dynkomi method none accepts no arguments\n");
		exit(1);
	}

	return d;
}


/* LINEAR dynkomi strategy - Linearly Decreasing Handicap Compensation. */
/* At move 0, we impose extra komi of handicap_count*handicap_value, then
 * we linearly decrease this extra komi throughout the game down to 0
 * at dynkomi_moves moves. */

float
uct_dynkomi_linear_permove(struct uct_dynkomi *d, struct board *b, struct tree *tree)
{
	if (b->moves >= d->uct->dynkomi_moves)
		return 0;
	return uct_linear_dynkomi(d->uct, b);
}

float
uct_dynkomi_linear_persim(struct uct_dynkomi *d, struct board *b, struct tree *tree, struct tree_node *node)
{
	/* We don't reuse computed value from tree->extra_komi,
	 * since we want to use value correct for this node depth.
	 * This also means the values will stay correct after
	 * node promotion. */
	return uct_dynkomi_linear_permove(d, b, tree);
}

struct uct_dynkomi *
uct_dynkomi_init_linear(struct uct *u, char *arg, struct board *b)
{
	struct uct_dynkomi *d = calloc(1, sizeof(*d));
	d->uct = u;
	d->permove = uct_dynkomi_linear_permove;
	d->persim = uct_dynkomi_linear_persim;
	d->done = uct_dynkomi_generic_done;
	d->data = NULL;

	if (arg) {
		fprintf(stderr, "uct: Dynkomi method linear accepts no arguments\n");
		exit(1);
	}

	return d;
}
