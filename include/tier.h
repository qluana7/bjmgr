#pragma once

#include <string>
#include <tuple>

#include "intdef.h"

struct tier_t {
    tier_t() = default;
    tier_t(const tier_t&) = default;
    tier_t(tier_t&&) = default;

    tier_t(char t, i32 l)
    { set_tier(t); set_level(l); }
    tier_t(i32 n) {
        if (n == 0) { tier = '\0', level = 0; return; }
        set_tier("BSGPDR"[(n - 1) / 5]); set_level(5 - (n - 1) % 5);
    }
    tier_t(const std::string& s) {
        set_tier(std::toupper(s[0])); set_level(s.back() - '0');
    }

    char tier;
    i32 level;

private:
    const static std::tuple<i32, i32, i32> color_table[31];

public:
    void set_tier(char c) {
        if (c == 'B' || c == 'S' || c == 'G' || c == 'P' || c == 'D' || c == 'R')
            tier = c;
        else tier = '\0';
    }

    void set_level(i32 l) {
        if (l >= 0 && l <= 5) level = l;
        else level = -1;
    }

    char get_tier() const { return tier; }
    i32 get_level() const { return level; }

    bool valid() const { return tier != '\0' && level != -1; }

    std::string short_name() const
    { return (valid() ? std::string(1, tier) + (char)(this->level + '0') : "U"); }

    std::string long_name() const
    { return tier_name() + (valid() ? std::string(" ") + (char)(this->level + '0') : ""); }

    std::string tier_name() const {
        if (!valid()) return "Unrated";

        switch (this->tier) {
            case 'B': return "Bronze";
            case 'S': return "Silver";
            case 'G': return "Gold";
            case 'P': return "Platinum";
            case 'D': return "Diamond";
            case 'R': return "Ruby";
        }

        return "";
    }

    std::string path() const {
        if (!valid()) return tier_name();

        return tier_name() + "/" + long_name();
    }

    std::tuple<i32, i32, i32> color() const {
        if (!this->valid()) return { 0, 0, 0 };
        
        return color_table[i32(*this)];
    }

    bool operator==(const tier_t& t) const
    { return i32(*this) == i32(t); }

    bool operator!=(const tier_t& t) const
    { return !(*this == t); }

    bool operator<(const tier_t& t) const
    {  return i32(*this) < i32(t); }

    bool operator>(const tier_t& t) const
    {  return i32(*this) > i32(t); }

    bool operator<=(const tier_t& t) const
    {  return i32(*this) <= i32(t); }

    bool operator>=(const tier_t& t) const
    {  return i32(*this) >= i32(t); }

    tier_t& operator=(const tier_t& t) {
        this->set_tier(t.tier);
        this->set_level(t.level);
        return *this;
    }

    tier_t& operator=(tier_t&& t) {
        this->set_tier(t.tier);
        this->set_level(t.level);
        return *this;
    }

    explicit operator i32() const {
        if (!this->valid()) return 0;

        i32 k = -1;

        switch (this->tier) {
            case 'B': k = 0; break;
            case 'S': k = 1; break;
            case 'G': k = 2; break;
            case 'P': k = 3; break;
            case 'D': k = 4; break;
            case 'R': k = 5; break;
        }

        return k * 5 + (5 - this->level) + 1;
    }
};