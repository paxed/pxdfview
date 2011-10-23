
/*

 very simple file viewer, by paxed@alt.org

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <ncurses.h>

void
curses_start(void)
{
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  refresh();

  flushinp();
}


void
curses_stop(void)
{
  erase();
  refresh();
  endwin();
}


void
bail(char *message)
{
  curses_stop();
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
    int quit = 0;
    int key;
    char *fname;
    int n_lines = 0;
    char **lines = NULL;
    char tmpbuf[81];
    FILE *inf;
    int i;
    int offset = 0;
    int redraw = 1;

    curses_start();

    if (argc > 1) {
	fname = argv[1];
    } else bail("Error: no filename given");

    if (COLS < 80 || LINES < 10)
	bail("Error: your terminal is too small");

    inf = fopen(fname, "r");
    if (!inf)
	bail("Error: Couldn't read file");

    while (!feof(inf)) {

	tmpbuf[0] = '\0';
	fgets(tmpbuf, 80, inf);
	tmpbuf[80] = '\0';

	if (lines) {
	    char **tmplines = NULL;
	    tmplines = (char **)calloc(n_lines+1, sizeof(char *));
	    memcpy(tmplines, lines, n_lines*sizeof(char *));
	    free(lines);
	    lines = tmplines;
	} else {
	    lines = (char **)calloc(n_lines+1, sizeof(char *));
	}

	lines[n_lines] = strdup(tmpbuf);
	n_lines++;
    }

    fclose(inf);

    while (!quit) {

	if (redraw) {

	    for (i = 0; i < (LINES-1); i++)
		mvprintw(i,0, "%-80s",
			 (i+offset < n_lines) ? lines[i+offset] : "");

	    mvprintw(LINES-1, 0, "(%i of %i)   '<'/'>' for more/less, 'q' to quit", (offset/(LINES-1))+1, ((n_lines-1)/(LINES-1))+1);

	    redraw = 0;
	}

        refresh();

	flushinp();
	key = getch();

	switch (key) {
	case KEY_RESIZE:
	case ERR: quit = TRUE; break;
	case KEY_EXIT:
	case KEY_CLOSE:
	case '\x1b':
	case 'Q':
	case 'q': quit = TRUE; break;
	case KEY_CLEAR:
	case 'r': redraw = 1; break;
	case KEY_SHOME:
	case KEY_HOME: offset = 0; redraw = 1; break;
	case KEY_SEND:
	case KEY_END: offset = n_lines-(n_lines%(LINES-1)); redraw = 1; break;
	case KEY_SNEXT:
	case KEY_NEXT:
	case KEY_NPAGE:
	case KEY_DOWN:
	case '\n':
	case ' ':
	case '>':
	    if (offset + (LINES-1) < n_lines) {
		offset += (LINES-1); redraw = 1;
	    }
	break;
	case KEY_SPREVIOUS:
        case KEY_PREVIOUS:
	case KEY_PPAGE:
	case KEY_UP:
	case '<':
	    if (offset >= (LINES-1)) {
		offset -= (LINES-1); redraw = 1;
	    } else {
		offset = 0; redraw = 1;
	    }
	break;
	default: break;
	}
    }

    curses_stop();

    for (i = 0; i < n_lines; i++) free(lines[i]);
    free(lines);

    return 0;
}
