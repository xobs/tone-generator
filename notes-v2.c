#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

#define TONES_MAX 10

/*
 * Initial algorithm and idea taken from:
 * http://kmkeen.com/awk-music/2011-03-07-20-10-07-394.html
 */

static double pl_note;
static double pl_lastnote;
static double pl_a, pl_b, pl_d;
static double pl_s; /* Current sample buffer */
static double pl_ca, pl_cb;
static double pl_de, pl_di;
static double pl_rate;

static double tones[TONES_MAX];

double pl_rand(void) {
	return ((double)rand()) / RAND_MAX;
}

double pleasant_wl(void) {
	int index = pl_rand() * TONES_MAX;
	double val = tones[index];
	//static int index = 0;
	//double val = tones[index++];
	fprintf(stderr, "Val[%d]: %lf\n", index, val);
	if (index >= TONES_MAX)
		index = 0;
	return val;
}

static int pleasant_reset(uint32_t rate) {

	pl_rate = rate;
	pl_s = 128;

#if 0
	double pl_scale;
	int i;
	pl_scale = pow(0.5, 1.0/5); /* Fifth root, pentatonic */
	for (i = 0; i < TONES_MAX; i++)
		tones[i] = (pl_rate / 160.0) * pow(pl_scale, i);
#endif
#if 0
	double pl_scale;
	pl_scale = pow(0.5, 1.0/12); /* Fifth root, pentatonic */
	tones[0] = (pl_rate / 160.0) * pow(pl_scale, 0);
	tones[1] = (pl_rate / 160.0) * pow(pl_scale, 1);
	tones[2] = (pl_rate / 160.0) * pow(pl_scale, 2);
	tones[3] = (pl_rate / 160.0) * pow(pl_scale, 4);
	tones[4] = (pl_rate / 160.0) * pow(pl_scale, 5);
	tones[5] = (pl_rate / 160.0) * pow(pl_scale, 7);
	tones[6] = (pl_rate / 160.0) * pow(pl_scale, 8);
	tones[7] = (pl_rate / 160.0) * pow(pl_scale, 9);
	tones[8] = (pl_rate / 160.0) * pow(pl_scale, 11);
	tones[9] = (pl_rate / 160.0) * pow(pl_scale, 12);
#endif
	tones[0] = 56;
	tones[1] = 64;
	tones[2] = 72;
	tones[3] = 81;
	tones[4] = 96;
	tones[5] = tones[0] * 2;
	tones[6] = tones[1] * 2;
	tones[7] = tones[2] * 2;
	tones[8] = tones[3] * 2;
	tones[9] = tones[4] * 2;

	pl_note = pleasant_wl();
	return 0;
}

static void pleasant_putsample(uint16_t *bfr, int offset, int nchan, double s) {
	int chan;

	for (chan = 0; chan < nchan; chan++)
		bfr[offset * nchan + chan] = (s - 128) * (256.0);
}

int pleasant_gen(uint16_t *bfr, int nchan, int nsamples) {
	static int i;
	int off;
	static enum {
		state_new,
		state_drain,

		/* Anti-pop, return value to center */
		state_clt,
		state_cgt,
	} state = state_new;

	for (off = 0; off < nsamples; off++) {

		switch (state) {

		case state_new:
			pl_lastnote = pl_note;
			pl_note = pleasant_wl();

			/* Avoid picking a new note that's the same as the
			 * last note.
			 */
			if (pl_note == pl_lastnote)
				pl_note *= 2;

			pl_d = (pl_rand() * 10.0 + 5.0) * pl_rate / 4.0;
			pl_a = 0;
			pl_b = 0;
			pl_s = 128;
			pl_ca = 40 / pl_note;
			pl_cb = 20 / pl_lastnote;
			pl_de = pl_rate / 10;
			pl_di = 0;

			state = state_drain;
			i = 0;
			/* Fallthrough */

		case state_drain:
			pl_a++;
			pl_b++;
			pl_di++;
			pl_s = pl_s + (pl_ca + pl_cb);
			if (pl_a > pl_note) {
				pl_a = 0;
				pl_ca *= -1;
			}
			if (pl_b > pl_lastnote) {
				pl_b = 0;
				pl_cb *= -1;
			}
			if (pl_di > pl_de) {
				pl_di = 0;
				pl_ca *= 0.9;
				pl_cb *= 0.9;
			}
			pleasant_putsample(bfr, off, nchan, pl_s);

			i++;
			if (i >= pl_d) {
				pl_s = (int)pl_s;
				if (pl_s < 128)
					state = state_clt;
				else
					state = state_cgt;
			}
			break;

		case state_clt:
			pleasant_putsample(bfr, off, nchan, pl_s);
			pl_s++;

			if (pl_s >= 128)
				state = state_new;
			break;

		case state_cgt:
			pleasant_putsample(bfr, off, nchan, pl_s);
			pl_s--;

			if (pl_s <= 128)
				state = state_new;
			break;

		default:
			fprintf(stderr, "Unknown state: %d\n", state);
			return -1;
		}
	}

	return 0;
}

int main(int argc, char **argv) {
	uint16_t bfr[1024 * 2];

	pleasant_reset(22050);

	while (1) {
		pleasant_gen(bfr, 2, 1024);
		write(1, bfr, sizeof(bfr));
	}


	return 0;
}
