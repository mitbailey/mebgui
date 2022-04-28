/**
 * @file mebgui.hpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief
 * @version See Git tags for version information.
 * @date 2022.03.17
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

#ifndef MEBGUI_HPP
#define MEBGUI_HPP

#define MIN_WIN_WIDTH 10
#define MAX_WIN_TITLE 64
#define MAX_MENU_MARK 64

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define DEFAULT_W_TIMEOUT 5

/**
 * @brief To be called at the beginning of the program, initializes NCURSES-specific items.
 *
 * @param timeout Input timeout.
 */
void ncurses_init(int timeout);

/**
 * @brief To be called at the end of the program, cleans up NCURSES-specific items.
 *
 */
void ncurses_cleanup();

// TODO: Add automatic terminal-size-based resizing.
class MEBWindow
{
public:
    /**
     * @brief Spawns a MEBWindow; constructor.
     *
     * @param x X-positional coordinate (left-right).
     * @param y Y-positional coordinate (up-down).
     * @param cols Columns wide.
     * @param rows Rows tall.
     * @param title Title of the window (maximum 32 characters).
     * @param parent (Optional) Pointer to the parent window.
     */
    MEBWindow(int x, int y, int cols, int rows, const char *title, MEBWindow *parent = nullptr);

    /**
     * @brief Frees window memory; destructor.
     *
     */
    ~MEBWindow();

    /**
     * @brief Moves a MEBWindow by some delta-x and delta-y.
     *
     * @param w The MEBWindow to be moved.
     * @param dx Change in x-position.
     * @param dy Change in y-position.
     */
    void Move(int dx, int dy);

    /**
     * @brief Moves a MEBWindow to a specific location.
     *
     * @param w The MEBWindow.
     * @param x The x-axis positional coordinate.
     * @param y The y-axis positional coordinate.
     */
    void MoveTo(int x, int y);

    /**
     * @brief Resizes a MEBWindow by some delta-width and delta-height.
     *
     * @param w The MEBWindow to be resized.
     * @param dcols Change in column-width.
     * @param drows Change in row-height.
     */
    void Resize(int dcols, int drows);

    /**
     * @brief Resizes a MEBWindow to a specific size.
     *
     * @param w The MEBWindow.
     * @param cols Width to resize to.
     * @param rows Height to resize to.
     */
    void ResizeTo(int cols, int rows);

    /**
     * @brief Refreshes a MEBWindow.
     *
     * @param w The MEBWindow.
     */
    void Refresh();

    WINDOW *win;
    MEBWindow *parent;

    int X() { return x_; };
    int Y() { return y_; };
    int Cols() { return cols_; };
    int Rows() { return rows_; };

    bool IsPosRel() { return rel_pos; };
    char *GetTitle() { return title; };

private:
    // FOR INTERNAL USE ONLY
    void instantiate_window();
    void destroy_window();

    int x_;
    int y_;
    int cols_;
    int rows_;

    bool rel_pos; // Relative positioning.
    char title[MAX_WIN_TITLE];
};

/**
 * @brief The MEBMenu class, a wrapper around NCURSES' MENU.
 *
 */
class MEBMenu
{
public:
    /**
     * @brief Spawns a menu; constructor.
     *
     * @param w Parent MEBWindow.
     * @param x Parent window relative x-axis offset.
     * @param y Parent window relative y-axis offset.
     * @param cols Width.
     * @param rows Height.
     * @param n_items Number of items listed in the menu.
     * @param item_titles List of strings to title each item.
     * @param item_desc List of strings to describe each item.
     * @param mark The "You Are Here" indicator.
     * @return MEBMenu*
     */
    MEBMenu(MEBWindow *w, int x, int y, int cols, int rows, int n_items, char *item_titles[], char *item_desc[], const char *mark);

    /**
     * @brief Moves the menu some delta-position.
     *
     * @param dx
     * @param dy
     */
    void Move(int dx, int dy);

    /**
     * @brief Refreshes the menu.
     *
     */
    void Refresh();

    /**
     * @brief Destructor.
     *
     */
    ~MEBMenu();

    /**
     * @brief Automatically handles up / down navigation, and returns the index of the choice selected or -1 if no selection was made.
     * 
     * @param in The user's input, retrieved via wgetch(...); 
     */
    int Update(int in);

    MENU *GetMenu() { return menu; };
    MEBWindow *GetParent() { return parent; };

private:
    void InstantiateMenu();
    void DestroyMenu();

    int x;
    int y;
    int rows;
    int cols;
    char mark[MAX_MENU_MARK];
    MENU *menu;
    ITEM **items;
    int n_items;
    MEBWindow *parent;
};

// TODO: Split declaration and definition of input(...).
/**
 * @brief Method for taking input from the user.
 *
 * @param mwin MEBWindow the input is being taken in.
 * @param x The window relative x-coordinate to begin at.
 * @param y The window relative y-coordinate to begin at.
 * @param input_msg Optional
 * @param fmt Formatted input.
 * @param ... Formatted input's arguments.
 */
template <typename... Args>
static void input(MEBWindow *mwin, uint32_t x, uint32_t y, char *input_msg, char *fmt, Args... args)
{
    WINDOW *lwin = mwin->win;
    wmove(lwin, x, y);
    nodelay(lwin, false);
    wrefresh(lwin);
    echo();

    if (input_msg != NULL)
    {
        // Print the input prompt if provided and refresh
        mvwprintw(lwin, x, y, "%s", input_msg);
        wrefresh(lwin);
    }

    wscanw(lwin, fmt, args...);

    noecho();
    wtimeout(lwin, DEFAULT_W_TIMEOUT);
    wrefresh(lwin);
}

#endif // MEBGUI_HPP