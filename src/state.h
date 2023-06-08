#ifndef HSTR_STATE_H
#define HSTR_STATE_H

#include <string>
#include <cstddef>

class Query : public std::string {
public:
    size_t char_count();
};

#endif