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
    size_t current_codepoint = 0;
    for (size_t i = 0; i < q.length(); ) {
        if (current_codepoint == pos) break;
        if (!is_continuation_byte(q[i])) {
            current_codepoint++;
        }
        int bc = byte_count(q[i]);
        idx += bc;
        i += bc;
    }
    return idx;
}