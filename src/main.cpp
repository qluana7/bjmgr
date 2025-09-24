#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <tuple>
#include <utility>

#include <ctime>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "ioutil.h"
#include "strlib.h"
#include "arg.h"
#include "tier.h"
#include "problem.h"

#ifdef ANSI_ENABLED
#define USE_COLOR(x) "\033[" #x "m"
#else
#define USE_COLOR(x) ""
#endif
#define RESET USE_COLOR(0)
#define COLOR(x) USE_COLOR(38;5;x)
#define COLORED_TEXT(x, s) COLOR(x) s RESET
#define COLORED_APP_NAME COLOR(214) APP_NAME COLOR(0) RESET
#define COLORED_USAGE COLOR(112) "Usage" RESET
#define COLORED_MENU(s) COLOR(218) s RESET
#define COLORED_ERROR COLOR(160) "Error" RESET

#define API_VERSION "v3"
#define BASE_URL "https://solved.ac/api/" API_VERSION "/"

using namespace nlohmann;

namespace fs = std::filesystem;

problem_t get_problem(i32 n);

static std::unordered_map<std::string, std::string> help_table {
    { "info",
    COLORED_USAGE ": " APP_NAME " info [options]"                                    "\n"
    ""                                                                              "\n"
    "  Collects problems by difficulty according to the specified folder structure" "\n"
    "  and displays the results."                                                   "\n"
    ""                                                                              "\n"
    COLORED_MENU("Options")                                                         "\n"
    "  --search <tier>    -s : filter information by tier"                          "\n"
    "  --dir <path>       -d : set working directory"                               "\n"
    ""                                                                              "\n"
    COLORED_MENU("Examples")                                                        "\n"
    "  " APP_NAME " info                get all information"                        "\n"
    "  " APP_NAME " info --search s1    get s1 tier's information"                  "\n"
    "  " APP_NAME " info -s b3..s1      get information from b3 to s1 tier"         "\n"
    "  " APP_NAME " info -s d           get information from d5 to d1 tier"         "\n"
    "  " APP_NAME " info -s b3..        get information above b3 tier"              "\n"
    "  " APP_NAME " info -s ..p2        get information below p2 tier"              "\n"
    },
    { "patch", 
    COLORED_USAGE ": " APP_NAME " patch [options]"      "\n"
    ""                                                  "\n"
    "  Fetches tiers from solved.ac and moves files"    "\n"
    "  to the correct directory."                       "\n"
    ""                                                  "\n"
    COLORED_MENU("Options")                             "\n"
    "  --log <path>      -l : set log output file."     "\n"
    "  --dir <path>      -d : set working directory."   "\n"
    "  --yes             -y : skip confirmation."       "\n"
    ""                                                  "\n"
    COLORED_MENU("Examples")                            "\n"
    "  " APP_NAME " patch"                              "\n"
    "  " APP_NAME " patch --cache \"../cache\""         "\n"
    "  " APP_NAME " patch -c\"../cache/p1.txt\""        "\n"
    "  " APP_NAME " patch -l\"./log.txt\""              "\n"
    },
    { "get",
    COLORED_USAGE ": " APP_NAME " get <problem-id>"                                     "\n"
    ""                                                                                  "\n"
    "  Gets information from solved.ac with the problem id."                            "\n"
    ""                                                                                  "\n"
    COLORED_MENU("Required")                                                            "\n"
    "  <problem-id>         : problem id (required)"                                   "\n"
    ""                                                                                  "\n"
    COLORED_MENU("Examples")                                                            "\n"
    "  " APP_NAME " get 1000"                                                           "\n"
    "  " APP_NAME " get 11440"                                                          "\n"
    },
    { "new",
    COLORED_USAGE ": " APP_NAME " new <problem-id> [options]"                    "\n"
    ""                                                                           "\n"
    "  Fetches the tier from solved.ac"                                          "\n"
    "  and creates a new file in the corresponding tier folder."                 "\n"
    ""                                                                           "\n"
    COLORED_MENU("Required")                                                     "\n"
    "  <problem-id>         : problem id (required)"                             "\n"
    ""                                                                           "\n"
    COLORED_MENU("Options")                                                      "\n"
    "  --dir <path>      -d : set working directory."                            "\n"
    "  --tier <tier>     -t : force tier (do not fetch from solved.ac)."         "\n"
    "  --extension <ext> -x : set file extension (default is cpp)."              "\n"
    "  --yes             -y : skip confirmation."                                "\n"
    "  --code            -c : open file with code. " COLOR(160) "(unsafe)" RESET "\n"
    ""                                                                           "\n"
    COLORED_MENU("Examples")                                                     "\n"
    "  " APP_NAME " new 1000"                                                    "\n"
    "  " APP_NAME " new 3024 -d../ -tD3 -xcpp"                                   "\n"
        },
        { "update",
    COLORED_USAGE ": " APP_NAME " update <username> [options]"                    "\n"
    ""                                                                            "\n"
    "  Gets all solved problems of the user from solved.ac"                       "\n"
    "  and create all the files that are solved but not in the directory."        "\n"
    ""                                                                            "\n"
    COLORED_MENU("Options")                                                       "\n"
    "  --log <path>      -l : set log output file."                               "\n"
    "  --dir <path>      -d : set working directory."                             "\n"
    "  --filter <tier>   -f : filter by tier."                                    "\n"
    "  --extension <ext> -x : set file extension (default is cpp)."               "\n"
    "  --yes             -y : skip confirmation."                                 "\n"
    "  --code            -c : open files with code. " COLOR(160) "(unsafe)" RESET "\n"
    ""                                                                            "\n"
    COLORED_MENU("Examples")                                                      "\n"
    "  " APP_NAME " update solvedac"                                              "\n"
    "  " APP_NAME " update solvedac -d../"                                        "\n"
    "  " APP_NAME " update solvedac --log \"./log.txt\""                          "\n"
    }
};

static std::unordered_map<std::string, std::string> short_desc {
    { "info", "Gets tier information in current/specific directory." },
    { "patch", "Updates tier and moves files to the correct directory." },
    { "get", "Gets tier information with problem id" },
    { "new", "Create new file with tier" },
    { "update", "Updates source code that are solved but not in the directory." },
    { "help", "Show help" }
};

static std::unordered_map<std::string, std::vector<opt_t>> opt_table {
    { "help", { } },
    { "info", {
        { "search", true, 's' },
        { "dir", true, 'd' }
    } },
    { "patch", {
        { "log", true, 'l' },
        { "dir", true, 'd' },
        { "yes", false, 'y' }
    } },
    { "get", { } },
    { "new", {
        { "dir", true, 'd' },
        { "tier", true, 't' },
        { "extension", true, 'x' },
        { "yes", false, 'y' },
        { "code", false, 'c' }
    } },
    { "update", {
        { "log", true, 'l' },
        { "dir", true, 'd' },
        { "filter", true, 'f' },
        { "extension", true, 'x' },
        { "yes", false, 'y' },
        { "code", false, 'c' }
    } }
};

static std::unordered_map<std::string, i32> tables {
    { "help", 0 },
    { "info", 1 },
    { "patch", 2 },
    { "get", 3 },
    { "new", 4 },
    { "update", 5 }
};

inline std::string rgb_color(int r, int g, int b) {
#ifndef ANSI_ENABLED
    return "";
#endif
    std::stringstream ss;
    ss << "\033[38;2;" << r << ";" << g << ";" << b << "m";

    return ss.str();
}

template <typename _Clock, typename _Duration>
inline std::ostream& operator<<(std::ostream& __os, std::chrono::time_point<_Clock, _Duration> tp) {
    std::time_t t = _Clock::to_time_t(tp);
    
    return __os << std::put_time(localtime(&t), "%c %Z");
}

void help(
    const args& arg, const std::string& c,
    bool err = false, const std::string& s = ""
) {
    auto& out = err ? std::cerr : std::cout;

    if (c == "help") {
        if (err)
            out << COLORED_ERROR ": " << s << "\n";
        else if (arg.args.empty())
            out <<
                "Baekjoon source code manager with solved.ac tier.\n";
        else if (!opt_table.count(arg.args[0]))
            out << COLORED_ERROR ": Unknown command '" << arg.args[0] << "'.\n";
        else
            out << "\n" << help_table[arg.args[0]];
        
        if (arg.args.empty() || !opt_table.count(arg.args[0])) {
            out <<
                ""                                                 "\n"
                COLORED_USAGE ": " APP_NAME " <command> [options]" "\n"
                ""                                                 "\n"
                COLORED_MENU("Command List")                       "\n";
            
            for (auto& [s1, s2] : short_desc)
                out << "  " << std::left << std::setw(6) << s1 << " : " << s2 << "\n";
            
            out <<
                ""                                                                              "\n"
                "See '" APP_NAME " help <command>' for more information on a specific command." "\n";
        }
    } else {
        if (err)
            out << COLORED_ERROR ": " << s << "\n";
        out << "\n" << help_table[c];
    }
}

void rec(std::vector<std::vector<i32>>& ps, const fs::path& p) {
    if (fs::is_directory(p)) {
        for (const auto& e : fs::directory_iterator(p)) rec(ps, e.path());
    } else if (p.extension() == ".cpp") {
        int n = std::stoi(p.stem());
        ps[(int)tier_t(p.parent_path().filename())].push_back(n);
    }
}

void get_list(std::vector<std::vector<i32>>& ps, fs::path __p) {
    for (const auto& folder : { "Bronze", "Silver", "Gold", "Platinum", "Diamond", "Ruby" }) {
        fs::path p = __p / folder;
        rec(ps, p);
    }

    for (auto& v : ps) std::sort(v.begin(), v.end());
}

void info(const args& arg) {
    std::cout << "\n";

    fs::path dir = arg.options.count("dir") ? fs::path(arg.options.at("dir").value.value()) : fs::path(".");

    if (!fs::exists(dir)) {
        help(
            arg, "info", true,
            "'" + dir.string() + "': No such directory"
        ); exit(1);
    }

    if (!fs::is_directory(dir)) {
        help(
            arg, "info", true,
            "'" + dir.string() + "': Not a directory"
        ); exit(1);
    }

    std::vector<std::vector<i32>> ps(32);
    get_list(ps, dir);

    i32 c = 0;
    std::stringstream ss;

    tier_range rng;
    
    if (arg.options.count("search")) {
        const std::string& st = arg.options.at("search").value.value();
        rng = tier_range(st);

        if (!rng.valid) {
            help(arg, "info", true, "Invalid tier range '" + st + "'");
            exit(1);
        }
    }

    i32 s = (i32)rng.start, e = (i32)rng.end;

    for (i32 i = s; i <= e; i++) {
        if (ps[i].empty()) continue;
        auto [r, g, b] = tier_t(i).color();
        ss
            << rgb_color(r, g, b)
            << tier_t(i).long_name() << RESET " : " << ps[i].size() << "\n";

        i32 k = 0;

        c += ps[i].size();
        for (auto x : ps[i]) {
            if (k % 16 == 0) ss << "    ";
            ss << std::setw(5) << x << " ";
            k++;
            if (k % 16 == 0) ss << "\n";
        }

        if (k % 16) ss << "\n";
    }

    std::cout << COLORED_TEXT(210, "Total Count") " : " << c << "\n\n" << ss.str();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void patch(const args& arg) {
    std::cout << "\n";
    
    fs::path f_log = arg.options.count("log") ? fs::path(arg.options.at("log").value.value()) : fs::path("log.txt");

    if (fs::exists(f_log) && !arg.options.count("yes")) {
        std::cout << "'" << f_log.string() << "': File already exists. Overwrite? [y/N] ";

        i32 r = getch(true);
        std::cout << std::endl;

        if (r != 'y' && r != 'Y') {
            std::cout << "\nPatch canceled by user.\n";
            exit(1);
        }
    }

    std::ofstream lgout(f_log);

    lgout << std::chrono::system_clock::now() << "\n\n";

    fs::path dir = arg.options.count("dir") ? fs::path(arg.options.at("dir").value.value()) : fs::path(".");
    std::vector<std::vector<i32>> ps(32);
    std::vector<std::pair<i32, tier_t>> odat;
    std::map<i32, tier_t> ndat;
    get_list(ps, dir);

    for (i32 i = 1; i <= 30; i++) {
        for (auto x : ps[i]) {
            odat.emplace_back(x, tier_t(i));
        }
    }

    sort(odat.begin(), odat.end(), [] (const auto& a, const auto& b) {
        bool r = a.second < b.second;
        if (a.second == b.second) r = a.first < b.first;
        return r;
    });

    const auto url = BASE_URL "problem/lookup?problemIds=";
    CURL* curl = curl_easy_init();

    if (curl) {
        std::cout << "Fetching data from solved.ac... 0%" << std::flush;
    
        std::string buf;

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

        for (i32 i = 0; i < (i32)odat.size(); i += 100) {
            std::string s = url +
            strlib::join(
                odat.begin() + i,
                odat.begin() + std::min(i + 100, (i32)odat.size()),
                [] (const auto& p) { return std::to_string(p.first); },
                ","
            );
            curl_easy_setopt(curl, CURLOPT_URL, s.c_str());

            CURLcode req = curl_easy_perform(curl);

            if (req != CURLE_OK) {
                std::cerr << COLORED_ERROR ": " << curl_easy_strerror(req);
                exit(1);
            }

            long sc; curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &sc);

            if (sc != 200) {
                std::cerr <<
                    COLORED_ERROR ": Error while fetching data\n"
                    "Check your network connection or try again later.\n\n"
                    "Check log file for more information.\n";
                
                lgout <<
                    "/* Debug Informations */" "\n"
                    "HTTP Status Code : " << sc << "\n"
                    "Response : \n" << buf << "\n"
                    "/* End of Debug Informations */" "\n";
                exit(1);
            }

            auto res = json::parse(buf);

            for (auto& it : res) {
                auto pid = it["problemId"].get<i32>();
                auto lv = tier_t(it["level"].get<i32>());
                ndat[pid] = lv;
                auto [_r, _g, _b] = lv.color();
                lgout <<
                    "Data fetched : " << pid <<
                    " => " << rgb_color(_r, _g, _b) <<
                    lv.long_name() << RESET << "\n";
            }

            lgout.flush();

            buf.clear();

            std::cout << "\rFetching data from solved.ac... " << (i32)(i * 1.L / odat.size() * 100) << "%" << std::flush;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << COLORED_ERROR ": Error while initializing CURL\n";
        exit(1);
    }

    std::cout << "\rFetching data from solved.ac... Done.\n" << std::flush;

    std::vector<std::tuple<i32, tier_t, tier_t>> diff;

    for (auto [id, t] : odat) {
        if (t != ndat[id]) {
            diff.emplace_back(id, t, ndat[id]);

            auto [_or, _og, _ob] = t.color();
            auto [_nr, _ng, _nb] = ndat[id].color();

            lgout <<
                "Diff : " << id << " : " <<
                rgb_color(_or, _og, _ob) <<
                t.long_name() << RESET " <-> " <<
                rgb_color(_nr, _ng, _nb) <<
                ndat[id].long_name() << RESET << "\n";
        }
    }

    lgout.flush();

    if (diff.empty()) {
        std::cout << "Nothing to patch.\n";
        return;
    }

    std::vector<std::string> diff_str;
    for (auto& [n, ot, nt] : diff) {
        auto [_or, _og, _ob] = ot.color();
        auto [_nr, _ng, _nb] = nt.color();

        std::stringstream ss;
        ss
            << n << " : "
            << rgb_color(_or, _og, _ob)
            << ot.long_name() << RESET " -> "
            << rgb_color(_nr, _ng, _nb)
            << nt.long_name() << RESET;
        
        diff_str.push_back(ss.str());
    }

    if (!arg.options.count("yes")) {
        std::cout << "Do you want to view patch list? [y/N] ";

        i32 r = getch(true);
        std::cout << std::endl;

        if (r == 'y' || r == 'Y') {
            fs::path tmp = fs::temp_directory_path() / "bjmgr_patch_list.txt";
            std::ofstream out(tmp);
            
            for (auto& s : diff_str) out << s << "\n";
            out.close();

            [[maybe_unused]]
            int _r = system(("less " + tmp.string()).c_str());

            fs::remove(tmp);
        }
    }

    if (!arg.options.count("yes")) {
        std::cout << "Proceed to patch? [y/N] ";

        i32 r = getch(true);
        std::cout << std::endl;

        if (r != 'y' && r != 'Y') {
            std::cout << "\nPatch canceled by user.\n";
            exit(1);
        }
    }

    std::cout << "Patching files... 0%" << std::flush;

    i32 err_cnt = 0;
    for (i32 i = 0; i < (i32)diff.size(); i++) {
        auto [id, o, n] = diff[i];

        if (!n.valid()) {
            lgout << "[" COLORED_ERROR "] Cannot patch as the tier is invalid or Unrated (" << id << ")\n";
            continue;
        }

        auto str = std::to_string(id) + ".cpp";
        fs::path
            op = dir / fs::path(o.path()) / str,
            np = dir / fs::path(n.path()) / str;
        
        std::error_code err;
        fs::rename(op, np, err);
        if (err) {
            err_cnt++;
            lgout << "[" COLORED_ERROR "] Patching Failed (" << id << ") : " << err.message() << "\n";
        } else
            lgout << "Patching Success (" << id << ")\n";
        std::cout << "\rPatching files... " << (i32)(i * 1.L / diff.size() * 100) << "%" << std::flush;
    }

    std::cout
        << "\rPatching files... Done.\n\n"
        << "Total : " << diff.size() << ", Success : " << diff.size() - err_cnt << ", Error : " << err_cnt << "\n";
    
    if (err_cnt)
        std::cout << "For each issue that occurred with the problem id, please refer to the log file.\n";
}

problem_t get_problem(int n) {
    CURL* curl = curl_easy_init();
    std::string buf;
    
    if (curl) {
        auto url = BASE_URL "problem/show?problemId=" + std::to_string(n);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
        
        CURLcode req = curl_easy_perform(curl);
        
        if (req != CURLE_OK) {
            std::cerr << COLORED_ERROR ": " << curl_easy_strerror(req);
            exit(2);
        }
        
        long sc; curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &sc);
        curl_off_t rt; curl_easy_getinfo(curl, CURLINFO_RETRY_AFTER, &rt);
        
        if (sc == 400) { help(args(), "get", true, "Bad Request"); exit(1); }
        if (sc == 404) { std:: cerr << "\n" << buf << " | Problem Number : " << n; exit(4); }
        if (sc == 429) { std:: cerr << "\n" << buf << " | Wait for sec : "; printf("%" CURL_FORMAT_CURL_OFF_T "s", rt); }
        if (sc != 200) { std:: cerr << "\n" << buf; exit(1); }
        
        curl_easy_cleanup(curl);
        
        auto res = json::parse(buf);

        problem_t p;

        p.name = res["titleKo"].get<std::string>();
        p.url = "https://www.acmicpc.net/problem/" + std::to_string(n);
        p.id = n;
        p.tier = tier_t(res["level"].get<int>());
        
        return p;
    } else { std::cerr << COLORED_ERROR ": CURL INIT ERROR"; exit(-1); }
}

void get(const args& arg) {
    if (arg.args.empty()) {
        help(arg, "get", true, "Missing problem id");
        exit(1);
    }

    int n;
    if (!strlib::try_parse(n, arg.args[0])) {
        help(arg, "get", true, "Invalid problem id");
        exit(1);
    }
    auto p = get_problem(n);
    
    auto [r, g, b] = p.tier.color();

    std::cout << "\n" <<
        "[" << rgb_color(r, g, b) << p.tier.long_name() << RESET "] " <<
        p.name << "\nLink : " << p.url << "\n";
}

void new_file(const args& arg) {
    if (arg.args.empty()) {
        help(arg, "new", true, "Missing problem id");
        exit(1);
    }

    int n;
    if (!strlib::try_parse(n, arg.args[0])) {
        help(arg, "new", true, "Invalid problem id");
        exit(1);
    }
    tier_t t = arg.options.count("tier") ? tier_t(*arg.options.at("tier").value) : get_problem(n).tier;

    std::string fext = arg.options.count("extension") ? *arg.options.at("extension").value : "cpp";
    fs::path dir = arg.options.count("dir") ? fs::path(*arg.options.at("dir").value) : fs::path(".");
    fs::path p(dir / t.path() / (std::to_string(n) + "." + fext));

    std::cout << "\n";

    if (fs::exists(p) && !arg.options.count("yes")) {
        std::cout << "'" << p.string() << "': File already exists. Overwrite? [y/N] ";

        i32 r = getch(true);
        std::cout << std::endl;

        if (r != 'y' && r != 'Y') {
            std::cout << "\nCanceled by user.\n";
            exit(1);
        }
    }

    std::ofstream(p).close();

    std::cout << "File created. : " << p.string() << "\n";

    if (arg.options.count("code"))
        [[maybe_unused]]
        int _r = system(("code -r \'" + p.string() + "'").c_str());
    else
        std::cout << "Open file with 'code -r " << p.string() << "'\n";
}

void update(const args& arg) {
    std::cout << "\n";

    if (arg.args.empty()) {
        help(arg, "list", true, "Missing username");
        exit(1);
    }
    
    fs::path f_log = arg.options.count("log") ? fs::path(arg.options.at("log").value.value()) : fs::path("log.txt");

    if (fs::exists(f_log) && !arg.options.count("yes")) {
        std::cout << "'" << f_log.string() << "': File already exists. Overwrite? [y/N] ";

        i32 r = getch(true);
        std::cout << std::endl;

        if (r != 'y' && r != 'Y') {
            std::cout << "\nPatch canceled by user.\n";
            exit(1);
        }
    }

    tier_range rng;

    if (arg.options.count("filter")) {
        const std::string& st = arg.options.at("filter").value.value();
        rng = tier_range(st);

        if (!rng.valid) {
            help(arg, "update", true, "Invalid tier range '" + st + "'");
            exit(1);
        }
    }

    std::ofstream lgout(f_log);

    lgout << std::chrono::system_clock::now() << "\n\n";

    fs::path dir = arg.options.count("dir") ? fs::path(arg.options.at("dir").value.value()) : fs::path(".");
    std::vector<std::vector<i32>> ps(32);
    std::vector<std::pair<i32, tier_t>> odat;
    std::map<i32, tier_t> ndat;
    get_list(ps, dir);

    for (i32 i = 1; i <= 30; i++) {
        for (auto x : ps[i]) {
            odat.emplace_back(x, tier_t(i));
        }
    }

    sort(odat.begin(), odat.end(), [] (const auto& a, const auto& b) {
        bool r = a.second < b.second;
        if (a.second == b.second) r = a.first < b.first;
        return r;
    });

    const auto url = BASE_URL "search/problem?query=s@" + arg.args[0];
    CURL* curl = curl_easy_init();

    if (curl) {
        std::cout << "Fetching data from solved.ac... 0%" << std::flush;
    
        std::string buf;

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        CURLcode req = curl_easy_perform(curl);

        if (req != CURLE_OK) {
            std::cerr << COLORED_ERROR ": " << curl_easy_strerror(req);
            exit(1);
        }

        long sc; curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &sc);

        if (sc != 200) {
            std::cerr <<
                COLORED_ERROR ": Error while fetching data\n"
                "Check your network connection or try again later.\n\n"
                "Check log file for more information.\n";
                
            lgout <<
                "/* Debug Informations */" "\n"
                "HTTP Status Code : " << sc << "\n"
                "Response : \n" << buf << "\n"
                "/* End of Debug Informations */" "\n";
            exit(1);
        }

        auto _res = json::parse(buf);

        i32 size = _res["count"].get<i32>();

        for (i32 i = 0, len = size / 50 + !!(size % 50); i < len; i++) {
            std::string s = url + "&page=" + std::to_string(i + 1);
            curl_easy_setopt(curl, CURLOPT_URL, s.c_str());

            buf = "";

            CURLcode req = curl_easy_perform(curl);

            if (req != CURLE_OK) {
                std::cerr << COLORED_ERROR ": " << curl_easy_strerror(req);
                exit(1);
            }

            long sc; curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &sc);

            if (sc != 200) {
                std::cerr <<
                    COLORED_ERROR ": Error while fetching data\n"
                    "Check your network connection or try again later.\n\n"
                    "Check log file for more information.\n";
                
                lgout <<
                    "/* Debug Informations */" "\n"
                    "HTTP Status Code : " << sc << "\n"
                    "Response : \n" << buf << "\n"
                    "/* End of Debug Informations */" "\n";
                exit(1);
            }

            nlohmann::json res;

            try {
                res = json::parse(buf);
            } catch (...) {
                std::cerr << COLORED_ERROR "Error while parsing data\n";
                lgout <<
                    "/* Debug Informations */" "\n"
                    "Response : \n" << buf << "\n"
                    "/* End of Debug Informations */" "\n";
                exit(1);
            }


            for (auto& it : res["items"]) {
                auto pid = it["problemId"].get<i32>();
                auto lv = tier_t(it["level"].get<i32>());
                ndat[pid] = lv;
                auto [_r, _g, _b] = lv.color();
                lgout <<
                    "Data fetched : " << pid <<
                    " => " << rgb_color(_r, _g, _b) <<
                    lv.long_name() << RESET << "\n";
            }

            lgout.flush();

            buf.clear();

            std::cout << "\rFetching data from solved.ac... " << (i32)(i * 1.L / len * 100) << "%" << std::flush;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << COLORED_ERROR ": Error while initializing CURL\n";
        exit(1);
    }

    std::cout << "\rFetching data from solved.ac... Done.\n" << std::flush;

    i32 cnts = 0;
    std::unordered_set<i32> s;
    std::map<i32, tier_t> filt;

    for (auto [id, t] : odat)
        s.insert(id);
    
    for (auto [id, t] : ndat) {
        auto [_r, _g, _b] = t.color();

        lgout << "[" << rgb_color(_r, _g, _b) << t.long_name() << RESET "] " << id << " : ";

        if (s.count(id))
            lgout << COLOR(46) "✔" RESET "\n";
        else {
            cnts++;

            if (rng.contains(t))
                filt[id] = t;

            lgout << COLOR(160) "✘" RESET "\n";
        }
    }

    lgout.flush();

    std::cout
        << "\n[" COLORED_TEXT(219, "Result") "]\n"
        << COLORED_TEXT(46, "Solved") " : " << ndat.size() << "\n"
        << COLORED_TEXT(45, "Cached") " : " << odat.size() << "\n"
        << COLORED_TEXT(208, "Not in directory") " : " << cnts << "\n"
        << COLORED_TEXT(27, "Filtered") " : " << filt.size() << "\n\n";
    
    if (filt.size() == 0) {
        std::cout << "Nothing to update.\n";
        return;
    }

    if (!arg.options.count("yes")) {
        std::cout << "Do you want to view update list? [y/N] ";

        i32 r = getch(true);
        std::cout << std::endl;

        if (r == 'y' || r == 'Y') {
            fs::path tmp = fs::temp_directory_path() / "bjmgr_update_list.txt";
            std::ofstream out(tmp);
            
            for (auto& [id, t] : filt) {
                auto [_r, _g, _b] = t.color();
                out << id << " : " << rgb_color(_r, _g, _b) << t.long_name() << RESET << "\n";
            }
            out.close();

            [[maybe_unused]]
            int _r = system(("less " + tmp.string()).c_str());

            fs::remove(tmp);
        }
    }

    if (!arg.options.count("yes")) {
        std::cout << "Proceed to update? [y/N] ";

        i32 r = getch(true);
        std::cout << std::endl;

        if (r != 'y' && r != 'Y') {
            std::cout << "\nupdate canceled by user.\n";
            exit(1);
        }
    }

    std::cout << "\n";

    std::string fext = arg.options.count("extension") ? *arg.options.at("extension").value : "cpp";

    i32 i = 1;

    for (auto& [id, t] : filt) {
        std::cout << "\rupdating files... " << i << " / " << filt.size() << std::flush;
        fs::path p(dir / t.path() / (std::to_string(id) + "." + fext));

        std::ofstream(p).close();

        lgout << "File created : " << p.string() << "\n";

        if (arg.options.count("code"))
            [[maybe_unused]]
            int _r = system(("code -r \'" + p.string() + "'").c_str());

        std::cout << " [ next(n), skip(s), quit(q) ]" << std::flush;
        
        while (true) {
            i32 _inp = getch(false);
        
            switch (_inp) {
                case 'n': case 'N':
                    break;
                case 's': case 'S':
                    lgout << "File skipped : " << p.string() << "\n";
                    fs::remove(p);
                    break;
                case 'q': case 'Q':
                    lgout << "Update canceled by user.\n";
                    std::cout << "\n\nUpdate canceled by user.\n";
                    lgout.flush();
                    return;
                default:
                    continue;
            }

            break;
        }

        i++;
    }

    std::cout << "\r" << std::string(60, ' ') << std::flush;
    std::cout << "\rupdating files... Done.\n\n";
}

int main(int argc, char** argv) {
    std::cout << COLORED_APP_NAME " " APP_VERSION "\n";
    args c;

    if (argc == 1) {
        help(c, "help", true, "No specific command");
        exit(1);
    }

    std::string cmd = strlib::tolower(argv[1]);

    init_options(opt_table[cmd]);

    int res;
    
    if ((res = parse_command(std::vector<std::string>(argv + 2, argv + argc), c)) != 0) {
        help(c, cmd, true, err2str(parse_command, res));
        exit(1);
    }

    int t = tables[cmd];

    if (!t) help(c, cmd);
    else ((std::vector<void (*)(const args&)>) {
        nullptr, info, patch, get, new_file, update
    })[t](c);
    
    return 0;
}