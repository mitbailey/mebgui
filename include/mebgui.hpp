/**
 * @file mebgui.hpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief
 * @version See Git tags for version information.
 * @date 2022.03.17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef MEBGUI_HPP
#define MEBGUI_HPP

#define MIN_WIN_WIDTH 10
#define MAX_WIN_TITLE 64

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// TODO: Add automatic terminal-size-based resizing.
// typedef struct MEBWindow MEBWindow;
// struct MEBWindow
// {
//     WINDOW *win;
//     // If 0, uses x y as absolute screen position.
//     // If 1, uses the position of the parent window and x y as offsets from the parent's top right corner.
//     uint8_t rel_pos;
//     int x;
//     int y;
//     MEBWindow *parent;
//     int cols;
//     int rows;
//     char title[32];
// };

/**
 * @brief To be called at the beginning of the program, initializes NCURSES-specific items.
 *
 */
void ncurses_init();

/**
 * @brief To be called at the end of the program, cleans up NCURSES-specific items.
 *
 */
void ncurses_cleanup();

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

    int get_x() { return x; };
    int get_y() { return y; };
    int get_cols() { return cols; };
    int get_rows() { return rows; };

    bool is_pos_rel() { return rel_pos; };
    char *get_title() { return title; };

private:
    // FOR INTERNAL USE ONLY
    void instantiate_window();
    void destroy_window();

    int x;
    int y;
    int cols;
    int rows;

    bool rel_pos; // Relative positioning.
    char title[MAX_WIN_TITLE];
};

/**
 * @brief The MEBMenu type, a wrapper around NCURSES' MENU.
 *
 */
typedef struct
{
    MENU *menu;
    ITEM **items;
    int n_items;
    MEBWindow *parent;
} MEBMenu;

/**
 * @brief Spawns a menu.
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
MEBMenu *Menu(MEBWindow *w, int x, int y, int cols, int rows, int n_items, char *item_titles[], char *item_desc[], const char *mark);

/**
 * @brief Must be called when the menu is done with.
 *
 * @param m The MEBMenu.
 */
void DestroyMEBMenu(MEBMenu *m);

// FOR INTERNAL USE ONLY
void destroy_menu(MENU *menu, int n_items, ITEM **items);

#endif // MEBGUI_HPP