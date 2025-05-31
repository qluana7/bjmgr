#include "arg.h"

// common return value
#define SUCCESS 0

// return value of parse_command
#define NOT_INIT -1
#define OPTION_NAME_ERROR 1
#define DQUOTE_ERROR 2
#define UNKNOWN_OPTION 3
#define PARAMETER_MISSING 4

// return value of init_option
#define DUPLICATED_NAME 1
#define DUPLICATED_SHORT_NAME 2
#define INVALID_SHORT_NAME 3

// o = option, d = index
#define SET_OPT(o, d) ((o) |= (1 << (d)))
// o = option, d = index
#define GET_OPT(o, d) ((o) &  (1 << (d)))

static std::unordered_map<std::string, opt_t> _opts;
static std::vector<opt_t> _short_opts(52);
static u64 _short_opts_flag = 0;
static bool _init = false;

enum class token_t {
    NONE, ARG, SHORT_OPT, LONG_OPT, EOO
};

static std::vector<std::string> _init_option_errstr {
    "Unknown Error",
    "Option name duplicated",
    "Option short name duplicated",
    "Invalid short name"
};

static std::vector<std::string> _parse_command_errstr {
    "Parser isn't have been initialized",
    "Unknown Error",
    "Invalid option name",
    "Illegal Error (DQUOTE_ERROR)",
    "Unknown option name",
    "Missing parameter"
};

template <typename T>
inline T xclamp(T value, T range_s, T range_e, T def_value) {
    return range_s <= value && value < range_e ? value : def_value;
}

std::string __err2str(void* f, i32 res) {
    if (f == (void*)init_options)
        return _init_option_errstr[xclamp(res, 0, (i32)_init_option_errstr.size(), 0)];
    else if (f == (void*)parse_command)
        return _parse_command_errstr[xclamp(res + 1, 0, (i32)_parse_command_errstr.size(), 1)];
    else
        return "Unknown Error (Reason : Unknown Function)";
}

inline constexpr i32 sn2i(char c) {
    if ('a' <= c && c <= 'z') return c - 'a';
    if ('A' <= c && c <= 'Z') return c - 'A' + 26;
    return -1;
}

i32 init_options(const std::vector<opt_t>& __opts) {
    _opts.clear();
    _short_opts_flag = 0;
    
    for (const auto& o : __opts) {
        if (_opts.count(o.name))
            return DUPLICATED_NAME;

        _opts[o.name] = o;

        if (o.short_name != '\0') {
            i32 d = sn2i(o.short_name);

            if (d == -1) return INVALID_SHORT_NAME;

            if (GET_OPT(_short_opts_flag, d))
                return DUPLICATED_SHORT_NAME;
            
            _short_opts[d] = o;
            SET_OPT(_short_opts_flag, d);
        }
    }

    _init = true;
    return SUCCESS;
}

i32 parse_command(const std::vector<std::string>& vs, args& ret) {
    if (!_init) return NOT_INIT;

    std::vector<std::pair<token_t, std::string>> tokens;

    // Scan and tokenize
    for (const auto& s : vs) {
        std::string tks = strlib::trim(s);
        if (s.empty()) continue;

        if (s[0] == '-') {
            if (s.size() == 1)
                return OPTION_NAME_ERROR;
            
            if (s[1] == '-') {
                if (s.size() == 2)
                    tokens.emplace_back(token_t::EOO, "--");
                else if (s.size() == 3)
                    return OPTION_NAME_ERROR;
                else
                    tokens.emplace_back(token_t::LONG_OPT, s.substr(2));
            } else {
                tokens.emplace_back(token_t::SHORT_OPT, s.substr(1));
            }
        } else
            tokens.emplace_back(token_t::ARG, s);
    }


    // Parse
    bool opt_flag = true;
    for (size_t i = 0; i < tokens.size(); i++) {
        auto [t, s] = tokens[i];

        if (!opt_flag) {
            switch (t) {
                case token_t::SHORT_OPT: ret.args.push_back("-"  + s); break;
                case token_t::LONG_OPT:  ret.args.push_back("--" + s); break;
                case token_t::NONE: break;
                default: ret.args.push_back(s); break;
            }
            continue;
        }

        switch (t) {
            case token_t::NONE: break;
            case token_t::ARG: ret.args.push_back(s); break;
            case token_t::SHORT_OPT: {
                for (size_t j = 0; j < s.size(); j++) {
                    i32 d = sn2i(s[j]);
                    if (d == -1) return OPTION_NAME_ERROR;

                    if (!GET_OPT(_short_opts_flag, d))
                        return UNKNOWN_OPTION;

                    auto& o = _short_opts[d];

                    if (o.has_parameter) {
                        if (j + 1 == s.size()) {
                            if (i + 1 == tokens.size())
                                return PARAMETER_MISSING;
                            
                            if (opt_flag && tokens[i + 1].first != token_t::ARG)
                                return PARAMETER_MISSING;
                            
                            ret.options[o.name] = {o.name, tokens[++i].second};
                            break;
                        }
                        
                        ret.options[o.name] = {o.name, s.substr(j + 1)};
                        break;
                    } else 
                        ret.options[o.name] = {o.name, std::nullopt};
                }
            } break;
            case token_t::LONG_OPT: {
                if (!_opts.count(s))
                    return UNKNOWN_OPTION;
                
                auto& o = _opts[s];

                if (o.has_parameter) {
                    if (i + 1 == tokens.size())
                        return PARAMETER_MISSING;
                    
                    if (opt_flag && tokens[i + 1].first != token_t::ARG)
                        return PARAMETER_MISSING;
                    
                    ret.options[o.name] = {o.name, tokens[++i].second};
                } else
                    ret.options[o.name] = {o.name, std::nullopt};
            } break;
            case token_t::EOO: opt_flag = false; break;
        }
    }

    return SUCCESS;
}