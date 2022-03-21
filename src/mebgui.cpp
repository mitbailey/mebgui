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
#include <cstdlib>
#include <stdexcept>

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
MEBWindow::MEBWindow(int x, int y, int cols, int rows, const char *title, MEBWindow *parent /* = nullptr */)
{
    if (strlen(title) > MAX_WIN_TITLE)
        throw std::length_error("Title length exceeds maximum.");

    int min_w = MIN_WIN_WIDTH;

    if (title != NULL)
    {
        if ((strlen(title) + 6) > min_w)
            min_w = strlen(title) + 6;
    }

    if (cols < min_w)
        cols = min_w;

    this->x = x;
    this->y = y;
    this->parent = parent;
    this->cols = cols;
    this->rows = rows;
    strcpy(this->title, title);

    instantiate_window();
}

// Frees window memory; to be called by user at end of window use.
MEBWindow::~MEBWindow()
{
    destroy_window();
    // free(this);
}

// Moves a MEBWindow by some delta-x and delta-y.
void MEBWindow::Move(int dx, int dy)
{
    destroy_window();
    this->x += dx;
    this->y += dy;

    instantiate_window();
}

// Moves a MEBWindow to a specific location.
void MEBWindow::MoveTo(int x, int y)
{
    destroy_window();
    this->x = x;
    this->y = y;

    instantiate_window();
}

// Resizes a MEBWindow by some delta-width and delta-height.
void MEBWindow::Resize(int dcols, int drows)
{
    destroy_window();

    this->cols += dcols;
    this->rows += drows;

    instantiate_window();
}

// Resizes a MEBWindow to a specific size.
void MEBWindow::ResizeTo(int cols, int rows)
{
    destroy_window();

    this->cols = cols;
    this->rows = rows;

    instantiate_window();
}

// Refreshes a MEBWindow.
void MEBWindow::Refresh()
{
    destroy_window();
    instantiate_window();
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
void MEBWindow::instantiate_window()
{
    if (this->parent == nullptr)
    {
        this->win = newwin(this->rows, this->cols, this->y, this->x);
    }
    else
    {
        this->win = newwin(this->rows, this->cols, this->y + this->parent->y, this->parent->x + this->parent->cols);
    }

    box(this->win, 0, 0); // 0, 0 gives default characters for the vertical and horizontal lines.

    // Draw the title.
    mvwprintw(this->win, 0, 2, " %s ", this->title);

    // Draw the window size.
    mvwprintw(this->win, this->rows - 1, this->cols - 10, " %dx%d ", this->cols, this->rows);

    wrefresh(this->win); // Show that box.
}

// FOR INTERNAL USE ONLY
void MEBWindow::destroy_window()
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