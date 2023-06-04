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
            case KEY_LEFT: {
                user_interface.move_cursor(CURSOR_LEFT);
                break;
            }
            case KEY_RIGHT: {
                user_interface.move_cursor(CURSOR_RIGHT);
                break;
            }
            case KEY_DC: {
                user_interface.remove_from_query();
                break;
            }
            case KEY_BACKSPACE: {
                user_interface.move_cursor(CURSOR_LEFT);
                user_interface.remove_from_query();
                break;
            }
            default: {
                tmp.push_back(user_input);
                if (utf8::is_valid(tmp)) {
                    user_interface.clear_error();
                    user_interface.insert_into_query(tmp);
                    user_interface.move_cursor(CURSOR_RIGHT);
                    tmp.clear();
                } else {
                    user_interface.display_error("Buffer contains invalid input.");
                }
                break;
            }
        }
    }
    endwin();
}