#include "cursesw.h"
#include "user_interface.h"
#include <string>
#include <stdint.h>
#include <utf8.h>

int main() {
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, true);
    noecho();

    std::string tmp;
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
            case KEY_BACKSPACE: {
                size_t pos = user_interface.move_cursor(DIRECTION_LEFT);
                /* Don't delete anything if the cursor is positioned
                 * before the start of the query string. */
                if (pos > 0) {
                    user_interface.remove_from_query();
                    user_interface.search();
                }
                break;
            }
            case KEY_RESIZE: {
                user_interface.print_history();
                break;
            }
            default: {
                tmp.push_back(user_input);
                if (utf8::is_valid(tmp)) {
                    user_interface.set_error(nullptr);
                    user_interface.insert_into_query(tmp);
                    user_interface.move_cursor(DIRECTION_RIGHT);
                    user_interface.search();
                    tmp.clear();
                } else {
                    user_interface.set_error("Buffer contains invalid input.");
                }
                break;
            }
        }
    }
    endwin();
}