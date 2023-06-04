#include "state.h"
#include "util.h"

size_t Query::character_count() {
    size_t total = 0;
    for (size_t i = 0; i < q.length(); i++) {
        if (!is_continuation_byte(q[i])) {
            total++;
        }
    }
    return total;
}

size_t Query::byte_index(size_t pos) {
    size_t idx = 0;
    size_t current_char = 0;
    while (idx < q.length()) {
        if (current_char == pos) break;
        if (!is_continuation_byte(q[idx])) {
            current_char++;
        }
        idx += byte_count(q[idx]);
    }
    return idx;
}
