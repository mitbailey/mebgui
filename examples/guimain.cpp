/**
 * @file guimain.cpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief An example file to show what a user could program given mebgui.
 * @version See Git tags for version information.
 * @date 2022.03.19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <ncurses.h>
#include <menu.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "mebgui.hpp"
#include "guimain.hpp"

///////////////////////////////////////
/// USER DEFINED EXAMPLE CODE BELOW ///
///////////////////////////////////////

char *menu1_choices[] = {
    (char *)"1:",
    (char *)"2:",
    (char *)"3:",
    (char *)"4:",
    (char *)"Exit:",
    (char *)NULL,
};

char *menu1_choices_desc[] = {
    (char *)"Choice 1",
    (char *)"Choice 2",
    (char *)"Choice 3",
    (char *)"Choice 4",
    (char *)"Exits the Program",
    (char *)NULL,
};

// Enables the graceful handling of Ctrl+C and other program-ending key combinations.
volatile sig_atomic_t done;
void sighandler(int sig)
{
    done = 1;
}

int main()
{
    signal(SIGINT, sighandler);

    // Fetch current terminal dimensions.
    int t_cols = 0, t_rows = 0;
    getmaxyx(stdscr, t_rows, t_cols);

    // Initial setup.
    ncurses_init();
    // Draws a 10x10 window at (0, 0) with title "My Window".
    MEBWindow *win1 = Window(0, 0, 10, 10, "My Window");
    MoveWindow(win1, 1, 0);

    // Main loop variables
    int forward = 1;

    MEBWindow *win2 = Window(0, 0, 10, 10, "Child Window", win1);

    MEBWindow *win3 = Window(25, 15, 30, 15, "Input Window");
    MEBMenu *mebmenu1 = Menu(win3, 2, 2, 25, 6, ARRAY_SIZE(menu1_choices), menu1_choices, menu1_choices_desc, "*");

    // Main loop.
    int c;
    while (!done)
    {
        // INPUT DETECTION AND HANDLING
        c = wgetch(stdscr);
        {
            // Menu handling.
            switch (c)
            {
            case KEY_DOWN:
                // goto program_end;
                // exit(1);
                menu_driver(mebmenu1->menu, REQ_DOWN_ITEM);
                wrefresh(mebmenu1->parent->win);
                break;
            case KEY_UP:
                menu_driver(mebmenu1->menu, REQ_UP_ITEM);
                wrefresh(mebmenu1->parent->win);
                break;
            case '\n':
                // TODO: Add functionality for other buttons.
                switch (item_index(current_item(mebmenu1->menu)))
                {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    goto program_end;
                }
                break;
            case KEY_RESIZE:

                // NOTE: !!! Everything that needs to be redrawn, etc, on resize needs to be done here.

                break;
            }
        }

        if (win1->x + win1->cols > t_cols)
            forward = 0;
        else if (win1->x < 1)
            forward = 1;

        if (forward)
        {
            MoveWindow(win1, 1, 0);
        }
        else
        {
            MoveWindow(win1, -1, 0);
        }

        RefreshWindow(win2);
        getmaxyx(stdscr, t_rows, t_cols);

        usleep(10000);
    }

program_end:
    // Cleanup.
    DestroyMEBWindow(win1);
    DestroyMEBMenu(mebmenu1);
    ncurses_cleanup();

    done = 1;

    printf("Everything is OK.\n");

    return 0;
}