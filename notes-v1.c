#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

const double rate = 44100;

double randd(void) {
	return ((double)rand()) / RAND_MAX;
}

double wl(void) {
	return (rate / 160) * pow(0.87055, randd() * 10);
}

int main(int argc, char **argv) {
	double wla = wl();
	double wlb;
	double a, b, c, d;
	double ca, cb;
	double de, di;

	int i;
	while(1) {
		wlb = wla;
		wla = wl();

		if (wla == wlb)
			wla *= 2;
		d = (randd() * 10 + 5) * rate / 4;
		a = 0;
		b = 0;
		c = 128;
		ca = 40 / wla;
		cb = 20 / wlb;
		de = rate / 10;
		di = 0;
		for (i = 0; i < d; i++) {
			a++;
			b++;
			di++;
			c = c + (ca + cb);
			if (a > wla) {
				a = 0;
				ca *= -1;
			}
			if (b > wlb) {
				b = 0;
				cb *= -1;
			}
			if (di > de) {
				di = 0;
				ca *= 0.9;
				cb *= 0.9;
			}
			printf("%c", (int)c);
		}

		if (c < 128) {
			while (c++ < 128)
				printf("%c", (int)c);
		}
		else {
			while (c-- > 128)
				printf("%c", (int)c);
		}
	}

	return 0;
}
