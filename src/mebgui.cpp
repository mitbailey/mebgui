/**
 * @file ui.cpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief https://tldp.org/HOWTO/NCURSES-Programming-HOWTO
 * @version See Git tags for version information.
 * @date 2022.03.16
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
#include "meb_print.h"

#include "guimain.hpp"

/**
 * @brief To be called at the beginning of the program, initializes NCURSES-specific items.
 * 
 */
void ncurses_init()
{
    initscr();
    cbreak();
    noecho(); // Doesn't echo input during getch().
    keypad(stdscr, TRUE);
    nodelay(stdscr, true);
    refresh();
}

/**
 * @brief To be called at the end of the program, cleans up NCURSES-specific items.
 * 
 */
void ncurses_cleanup()
{
    endwin();
    clear();
}

// Spawns a MEBWindow.
MEBWindow *Window(int x, int y, int cols, int rows, char *title, MEBWindow *parent /* = nullptr */)
{
    if (strlen(title) > MAX_WIN_TITLE)
        return NULL;

    int min_w = MIN_WIN_WIDTH;

    if (title != NULL)
    {
        if ((strlen(title) + 6) > min_w)
            min_w = strlen(title) + 6;
    }

    if (cols < min_w)
        cols = min_w;

    MEBWindow *w = (MEBWindow *)malloc(sizeof(MEBWindow));

    w->x = x;
    w->y = y;
    w->parent = parent;
    w->cols = cols;
    w->rows = rows;
    strcpy(w->title, title);

    instantiate_window(w);

    return w;
}

// Frees window memory; to be called by user at end of window use.
void DestroyMEBWindow(MEBWindow *w)
{
    destroy_window(w->win);
    free(w);
}

// Moves a MEBWindow by some delta-x and delta-y.
void MoveWindow(MEBWindow *w, int dx, int dy)
{
    destroy_window(w->win);
    w->x += dx;
    w->y += dy;

    instantiate_window(w);
}

// Moves a MEBWindow to a specific location.
void MoveWindowTo(MEBWindow *w, int x, int y)
{
    destroy_window(w->win);
    w->x = x;
    w->y = y;

    instantiate_window(w);
}

// Resizes a MEBWindow by some delta-width and delta-height.
void ResizeWindow(MEBWindow *w, int dcols, int drows)
{
    destroy_window(w->win);

    w->cols += dcols;
    w->rows += drows;

    instantiate_window(w);
}

// Resizes a MEBWindow to a specific size.
void ResizeWindowTo(MEBWindow *w, int cols, int rows)
{
    destroy_window(w->win);

    w->cols = cols;
    w->rows = rows;

    instantiate_window(w);
}

// Refreshes a MEBWindow.
void RefreshWindow(MEBWindow *w)
{
    destroy_window(w->win);
    instantiate_window(w);
}

// Spawns a menu.
MEBMenu *Menu(MEBWindow *w, int x, int y, int cols, int rows, int n_items, char *item_titles[], char *item_desc[], const char *mark)
{
    MEBMenu *m = (MEBMenu *)malloc(sizeof(MEBMenu));
    m->n_items = n_items;
    m->parent = w;
    m->items = (ITEM **)calloc(n_items, sizeof(ITEM *));
    for (int i = 0; i < n_items; ++i)
    {
        m->items[i] = new_item(item_titles[i], item_desc[i]);
    }
    m->menu = new_menu((ITEM **)m->items);
    set_menu_win(m->menu, w->win);
    set_menu_sub(m->menu, derwin(w->win, rows, cols, x, y));
    set_menu_mark(m->menu, mark);
    post_menu(m->menu);
    wrefresh(w->win);

    return m;
}

// Must be called when the menu is done with.
void DestroyMEBMenu(MEBMenu *m)
{
    destroy_menu(m->menu, m->n_items, m->items);
    free(m->items);
}

// FOR INTERNAL USE ONLY
void instantiate_window(MEBWindow *w)
{
    if (w->parent == nullptr)
    {
        w->win = newwin(w->rows, w->cols, w->y, w->x);
    }
    else
    {
        w->win = newwin(w->rows, w->cols, w->y + w->parent->y, w->parent->x + w->parent->cols);
    }

    box(w->win, 0, 0); // 0, 0 gives default characters for the vertical and horizontal lines.

    // Draw the title.
    mvwprintw(w->win, 0, 2, " %s ", w->title);

    // Draw the window size.
    mvwprintw(w->win, w->rows - 1, w->cols - 10, " %dx%d ", w->cols, w->rows);

    wrefresh(w->win); // Show that box.
}

// FOR INTERNAL USE ONLY
void destroy_window(WINDOW *win)
{
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window
    wrefresh(win);                                        // Refresh it (to leave it blank)
    delwin(win);                                          // and delete
}

// FOR INTERNAL USE ONLY
void destroy_menu(MENU *menu, int n_items, ITEM **items)
{
    unpost_menu(menu);
    free_menu(menu);
    for (int i = 0; i < n_items; ++i)
    {
        free_item(items[i]);
    }
}