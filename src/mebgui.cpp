/**
 * @file ui.cpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief https://tldp.org/HOWTO/NCURSES-Programming-HOWTO
 * @version See Git tags for version information.
 * @date 2022.03.16
 *
 * @copyright Copyright (c) 2022
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

// #include "meb_print.h"
#include "mebgui.hpp"

#include "guimain.hpp"

// To be called at the beginning of the program, initializes NCURSES-specific items.
void ncurses_init(int timeout)
{
    initscr();
    cbreak();
    noecho(); // Doesn't echo input during getch().
    keypad(stdscr, TRUE);
    wtimeout(stdscr, timeout);
    refresh();
}

// To be called at the end of the program, cleans up NCURSES-specific items.
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

    this->x_ = x;
    this->y_ = y;
    this->parent = parent;
    this->cols_ = cols;
    this->rows_ = rows;
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
    this->x_ += dx;
    this->y_ += dy;

    Refresh();
}

// Moves a MEBWindow to a specific location.
void MEBWindow::MoveTo(int x, int y)
{
    this->x_ = x;
    this->y_ = y;

    Refresh();
}

// Resizes a MEBWindow by some delta-width and delta-height.
void MEBWindow::Resize(int dcols, int drows)
{
    this->cols_ += dcols;
    this->rows_ += drows;

    Refresh();
}

// Resizes a MEBWindow to a specific size.
void MEBWindow::ResizeTo(int cols, int rows)
{
    this->cols_ = cols;
    this->rows_ = rows;

    Refresh();
}

// Refreshes a MEBWindow.
void MEBWindow::Refresh()
{
    destroy_window();
    instantiate_window();
}

// FOR INTERNAL USE ONLY
void MEBWindow::instantiate_window()
{
    if (this->parent == nullptr)
    {
        this->win = newwin(this->rows_, this->cols_, this->y_, this->x_);
    }
    else
    {
        this->win = newwin(this->rows_, this->cols_, this->y_ + this->parent->y_, this->parent->x_ + this->parent->cols_);
    }

    box(this->win, 0, 0); // 0, 0 gives default characters for the vertical and horizontal lines.

    // Draw the title.
    mvwprintw(this->win, 0, 2, " %s ", this->title);

    // Draw the window size.
    mvwprintw(this->win, this->rows_ - 1, this->cols_ - 10, " %dx%d ", this->cols_, this->rows_);

    wrefresh(this->win); // Show that box.
}

// FOR INTERNAL USE ONLY
void MEBWindow::destroy_window()
{
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window
    wrefresh(win);                                        // Refresh it (to leave it blank)
    delwin(win);                                          // and delete
}


// Spawns a menu.
MEBMenu::MEBMenu(MEBWindow *w, int x, int y, int cols, int rows, int n_items, char *item_titles[], char *item_desc[], const char *mark)
{
    // MEBMenu *m = (MEBMenu *)malloc(sizeof(MEBMenu));
    strcpy(this->mark, mark);
    this->x = x;
    this->y = y;
    this->rows = rows;
    this->cols = cols;
    this->n_items = n_items;
    this->parent = w;

    this->items = (ITEM **)calloc(n_items, sizeof(ITEM *));
    for (int i = 0; i < n_items; ++i)
    {
        this->items[i] = new_item(item_titles[i], item_desc[i]);
    }

    InstantiateMenu();
}

void MEBMenu::InstantiateMenu()
{
    this->menu = new_menu((ITEM **)this->items);
    set_menu_win(menu, parent->win);
    set_menu_sub(menu, derwin(parent->win, rows, cols, y, x));
    set_menu_mark(menu, mark);
    post_menu(menu);
    wrefresh(parent->win);
}

// Destructor.
MEBMenu::~MEBMenu()
{
    DestroyMenu();
    free(items);
}

void MEBMenu::Move(int dx, int dy)
{
    x += dx;
    y += dy;

    Refresh();
};

void MEBMenu::Refresh()
{
    // destroy_menu();
    unpost_menu(menu);
    free_menu(menu);
    InstantiateMenu();
}

// FOR INTERNAL USE ONLY
void MEBMenu::DestroyMenu()
{
    unpost_menu(menu);
    free_menu(menu);
    for (int i = 0; i < n_items; ++i)
    {
        free_item(items[i]);
    }
}

int MEBMenu::Update(int in)
{
    switch(in)
    {
    case KEY_DOWN:
        menu_driver(menu, REQ_DOWN_ITEM);
        wrefresh(parent->win);
        break;
    case KEY_UP:
        menu_driver(menu, REQ_UP_ITEM);
        wrefresh(parent->win);
        break;
    case '\n':
        return item_index(current_item(menu));
    }
    
    return -1;
}