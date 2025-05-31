#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <utility>

#include "intdef.h"
#include "strlib.h"

struct opt_t {
    opt_t() = default;
    // Create new opt_t object.
    // if option hasn't short name, set __sname to '\0'.
    //
    // short name range : [a-zA-Z]
    opt_t(const std::string& __name, bool __has_para, char __sname = '\0')
    : name(__name), short_name(__sname), has_parameter(__has_para) { }

    std::string name;
    char short_name;
    bool has_parameter;
};

struct option {
    std::string name;
    std::optional<std::string> value;
};

struct args {
    std::unordered_map<std::string, option> options;
    std::vector<std::string> args;
};

std::string __err2str(void* f, i32 res);
i32 init_options(const std::vector<opt_t>& __opts);
i32 parse_command(const std::vector<std::string>& vs, args& ret);

#define err2str(f, r) __err2str((void*)(f), (r))