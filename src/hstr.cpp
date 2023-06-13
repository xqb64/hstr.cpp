#include <string>
#include <utf8.h>
#include "curses.h"
#include "hstr.h"
#include "user_interface.h"

const int CTRL_E = 5;
const int CTRL_T = 20;
const int TAB = 9;
const int ENTER = 10;

void mainloop() {
    std::string buffer;
    UserInterface user_interface;

    for (;;) {
        int user_input = getch();
        switch (user_input) {
            case KEY_UP: {
                user_interface.move_highlighted(DIRECTION_UP);
                break;
            }
            case KEY_DOWN: {
                user_interface.move_highlighted(DIRECTION_DOWN);
                break;
            }
            case KEY_LEFT: {
                user_interface.move_cursor(DIRECTION_LEFT);
                break;
            }
            case KEY_RIGHT: {
                user_interface.move_cursor(DIRECTION_RIGHT);
                break;
            }
            case KEY_DC: {
                user_interface.remove_from_query();
                user_interface.search();
                break;
            }
            case CTRL_E: {
                user_interface.toggle_search_mode();
                user_interface.search();
                break;
            }
            case CTRL_T: {
                user_interface.toggle_case_sensitivity();
                user_interface.search();
                break;
            }
            case KEY_BACKSPACE: {
                /* Don't delete anything if the cursor is positioned
                 * before the start of the query string. */
                if (user_interface.get_cursor_position() > 0) {
                    user_interface.move_cursor(DIRECTION_LEFT);
                    user_interface.remove_from_query();
                    user_interface.search();
                }
                break;
            }
            case KEY_RESIZE: {
                user_interface.reprint();
                break;
            }
            case TAB: {
                user_interface.echo();
                return;
            }
            case ENTER: {
                user_interface.echo(true);
                return;
            }
            case KEY_PPAGE: {
                user_interface.turn_page(DIRECTION_UP);
                break;
            }
            case KEY_NPAGE: {
                user_interface.turn_page(DIRECTION_DOWN);
                break;
            }
            default: {
                buffer.push_back(user_input);
                if (utf8::is_valid(buffer)) {
                    user_interface.set_error(nullptr);
                    user_interface.insert_into_query(buffer);
                    user_interface.move_cursor(DIRECTION_RIGHT);
                    user_interface.search();
                    buffer.clear();
                } else {
                    user_interface.set_error("Buffer contains invalid input.");
                }
                break;
            }
        }
    }
}

void setup() {
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, true);
    noecho();
}

void teardown() {
    clear();
    refresh();
    doupdate();
    endwin();
}