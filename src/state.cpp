#include "state.h"
#include "util.h"

size_t Query::char_count() {
    size_t total = 0;
    for (size_t i = 0; i < q.length(); i++) {
        if (!is_continuation_byte(q[i])) {
            total++;
        }
    }
    return total;
}

