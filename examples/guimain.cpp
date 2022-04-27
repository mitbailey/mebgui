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
    // Signal handler for Ctrl+C, etc.
    signal(SIGINT, sighandler);

    // Fetch current terminal dimensions.
    int t_cols = 0, t_rows = 0;
    getmaxyx(stdscr, t_rows, t_cols);

    // Initial setup.
    ncurses_init(DEFAULT_W_TIMEOUT);

    // Windows instantiation.
    // Draws a 10x10 window at (0, 0) with title "My Window".
    MEBWindow *win1 = new MEBWindow(0, 0, 10, 10, "My Window");
    win1->Move(1, 0);

    MEBWindow *win2 = new MEBWindow(0, 0, 10, 10, "Child Window", win1);

    MEBWindow *win3 = new MEBWindow(25, 10, 60, 20, "Input Window");
    MEBMenu *mebmenu1 = new MEBMenu(win3, 2, 2, 25, 6, ARRAY_SIZE(menu1_choices), menu1_choices, menu1_choices_desc, "*");

    // Main loop variables.
    char user_input_string[256] = {0};
    int in;
    int sel = -1;

    // Main loop.
    while (!done)
    {
        // Retrieves any single keypress, and can detect Terminal resizing.
        in = wgetch(stdscr);

        // Updates the menu. Will return which choice the user has selected (-1 if none selected).
        sel = mebmenu1->Update(in);

        // Determines actions based on which menu item was selected, if any.
        switch (sel)
        {
        case 0:
            mebmenu1->GetParent()->Move(0, 1);
            mebmenu1->Refresh();
            break;
        case 1:
            mebmenu1->GetParent()->Move(0, -1);
            mebmenu1->Refresh();
            break;
        case 2:
            // Input example, reads until whitespace.
            input(mebmenu1->GetParent(), 10, 2, "Input: ", "%s", user_input_string);
            mvwprintw(mebmenu1->GetParent()->win, 11, 2, ">> %s", user_input_string);
            wrefresh(mebmenu1->GetParent()->win);
            break;
        case 3:
            // Input example which reads in 255 bytes until a newline.
            input(mebmenu1->GetParent(), 10, 2, "Input: ", "%255[^\n]", user_input_string);
            mvwprintw(mebmenu1->GetParent()->win, 11, 2, ">> %s", user_input_string);
            wrefresh(mebmenu1->GetParent()->win);
            break;
        case 4:
            goto program_end;
        }
        
        // This is where Terminal resizing should be handled if necessary. This usually entails refreshing or moving windows. 
        if (in == KEY_RESIZE)
        {
            // NOTE: !!! Everything that needs to be redrawn, etc, on resize needs to be done here.
        }

        // Code which detects the edge of the Terminal, moving and bouncing two windows.
        static int forward = 1;
        if (win1->X() + win1->Cols() > t_cols)
            forward = 0;
        else if (win1->X() < 1)
            forward = 1;
        if (forward)
        {
            win1->Move(1, 0);
        }
        else
        {
            win1->Move(-1, 0);
        }
        win2->Refresh();
        
        // Gets and stores the current width (in columns) and height (in rows) of the Terminal.
        getmaxyx(stdscr, t_rows, t_cols);

        usleep(10000);
    }

program_end:

    // Cleanup.
    delete (win1);
    delete (win2);
    delete (win3);

    delete (mebmenu1);

    ncurses_cleanup();

    done = 1;

    printf("Everything is OK.\n");

    return 0;
}