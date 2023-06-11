#ifndef HSTR_USER_INTERFACE_H
#define HSTR_USER_INTERFACE_H

#include <string>
#include <vector>

enum SearchMode {
    MODE_EXACT = 0,
    MODE_REGEX = 1,
    MODE_FUZZY = 2,
};

enum VerticalDirection {
    DIRECTION_UP = -1,
    DIRECTION_DOWN = 1,
};

enum HorizontalDirection {
    DIRECTION_LEFT = -1,
    DIRECTION_RIGHT = 1,
};

class UserInterface {
public:
    UserInterface();
    void echo(bool newline = false);
    void search();
    void move_highlighted(VerticalDirection d);
    size_t move_cursor(HorizontalDirection d);
    void insert_into_query(std::string s);
    void remove_from_query();
    void turn_page(VerticalDirection d);
    void toggle_search_mode();
    void toggle_case_sensitivity();
    void print_history();
    void set_error(const char *error);
private:
    void exact_search();
    void regex_search();
    void fuzzy_search();
    void paint_matched_chars(const std::string &s, size_t row) const;
    void paint_highlighted(const std::string &s, size_t old);
    void display_status() const;
    void display_error() const;
    const char *get_search_mode_str() const;
    const char *get_case_sensitivity_str() const;
    void print(const std::string &s, size_t row, size_t column, int color_pair, bool pad = true) const;
    void reposition_cursor() const;
    void read_history();
    void init_color_pairs() const;
    void clear_row(size_t r) const;
    void pad2end() const;
    std::string get_highlighted_entry();
    size_t page_count() const;
    size_t entry_count() const;
    size_t max_entry_count() const;
    size_t max_entry_length() const;
    std::vector<std::string> search_results;
    std::vector<std::string> history;
    std::string query;
    size_t cursor_position = 0;
    size_t page = 1;
    size_t highlighted = 0;
    const char *error = NULL;
    SearchMode search_mode = MODE_EXACT;
    bool case_sensitivity = false;
};

#endif