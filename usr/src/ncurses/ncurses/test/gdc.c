/*
 * Grand digital clock for curses compatible terminals
 * Usage: gdc [-s] [n]   -- run for n seconds (default infinity)
 * Flags: -s: scroll
 *
 * modified 10-18-89 for curses (jrl)
 * 10-18-89 added signal handling
 */

#include <time.h>
#include <signal.h>
#include <ncurses.h>

/* it won't be */
long now; /* yeah! */
struct tm *tm;

short disp[11] = {
	075557, 011111, 071747, 071717, 055711,
	074717, 074757, 071111, 075757, 075717, 002020
};
long old[6], next[6], new[6], mask;
char scrol;

int sigtermed=0;

void sighndl(signo)
int signo;
{
	signal(signo, sighndl);
	sigtermed=signo;
}

main(argc, argv)
char **argv;
{
long t, a;
int i, j, s, k;
int n = 0;

	signal(SIGINT,sighndl);
	signal(SIGTERM,sighndl);
	signal(SIGKILL,sighndl);

	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_RED);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	clear();
	refresh();
	while(--argc > 0) {
		if(**++argv == '-')
			scrol = 1;
		else
			n = atoi(*argv);
	}
	do {
		mask = 0;
		time(&now);
		tm = localtime(&now);
		set(tm->tm_sec%10, 0);
		set(tm->tm_sec/10, 4);
		set(tm->tm_min%10, 10);
		set(tm->tm_min/10, 14);
		set(tm->tm_hour%10, 20);
		set(tm->tm_hour/10, 24);
		set(10, 7);
		set(10, 17);
		for(k=0; k<6; k++) {
			if(scrol) {
				for(i=0; i<5; i++)
					new[i] = new[i]&~mask | new[i+1]&mask;
				new[5] = new[5]&~mask | next[k]&mask;
			} else
				new[k] = new[k]&~mask | next[k]&mask;
			next[k] = 0;
			for(s=1; s>=0; s--)
			{
				standt(s);
				for(i=0; i<6; i++)
				{
					if(a = (new[i]^old[i])&(s ? new : old)[i])
					{
						for(j=0,t=1<<26; t; t>>=1,j++)
						{
							if(a&t)
							{
								if(!(a&(t<<1)))
								{
									movto(i, 2*j);
								}
								addstr("  ");
							}
						}
					}
					if(!s)
					{
						old[i] = new[i];
					}
				}
				if(!s)
				{
					refresh();
				}
			}
		}
		movto(6, 0);
		refresh();
		sleep(1);
		if (sigtermed)
		{
			standend();
			clear();
			refresh();
			endwin();
			fprintf(stderr, "gdc terminated by signal %d\n", sigtermed);
			exit(1);
		}
	} while(--n);
	standend();
	clear();
	refresh();
	endwin();
	return(0);
}

set(t, n)
	register n;
{
	register i, m;

	m = 7<<n;
	for(i=0; i<5; i++) {
		next[i] |= ((disp[t]>>(4-i)*3)&07)<<n;
		mask |= (next[i]^old[i])&m;
	}
	if(mask&m)
		mask |= m;
}

standt(on)
int on;
{
	if (on)
	{
		attron(COLOR_PAIR(1));
	}
	else
	{
		attron(COLOR_PAIR(2));
	}
}

movto(line,col)
{
	move(line, col);
}

