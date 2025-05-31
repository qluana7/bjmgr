#pragma once

#include <string>

#include "tier.h"
#include "intdef.h"

struct problem_t {
    std::string name, url;
    i32 id;

    tier_t tier;
};