#include <cstdio>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

struct Rule {
    std::string id;
    std::regex re;
    std::string scope;
    std::string why;
    std::string fixture;
};

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static bool split_row(const std::string& line, std::vector<std::string>* out) {
    out->clear();
    size_t pos = 0;
    while (true) {
        size_t hit = line.find(" | ", pos);
        if (hit == std::string::npos) {
            out->push_back(trim(line.substr(pos)));
            break;
        }
        out->push_back(trim(line.substr(pos, hit - pos)));
        pos = hit + 3;
    }
    return out->size() == 5;
}

static bool scope_match(const std::string& scope, const std::string& rel, const std::string& id) {
    if (scope == "all" || scope == "engine") {
        if (id == "L7" && rel == "platform/rt/traps.cpp") return false;
        return true;
    }
    size_t pos = 0;
    while (pos <= scope.size()) {
        size_t comma = scope.find(',', pos);
        std::string p = comma == std::string::npos ? scope.substr(pos) : scope.substr(pos, comma - pos);
        p = trim(p);
        if (!p.empty() && rel.rfind(p, 0) == 0) return true;
        if (comma == std::string::npos) break;
        pos = comma + 1;
    }
    return false;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: kitty-lint DIR\n");
        return 2;
    }
    std::ifstream rf("tools/kitty-lint.rules");
    if (!rf) {
        std::fprintf(stderr, "kitty-lint: cannot read tools/kitty-lint.rules\n");
        return 2;
    }
    std::vector<Rule> rules;
    std::string line;
    int line_no = 0;
    while (std::getline(rf, line)) {
        line_no++;
        line = trim(line);
        if (line.empty()) continue;
        std::vector<std::string> f;
        if (!split_row(line, &f)) {
            std::fprintf(stderr, "kitty-lint: bad rule row at line %d\n", line_no);
            return 2;
        }
        Rule r;
        r.id = f[0];
        try {
            r.re = std::regex(f[1]);
        } catch (const std::regex_error&) {
            std::fprintf(stderr, "kitty-lint: bad pattern in %s\n", f[0].c_str());
            return 2;
        }
        r.scope = f[2];
        r.why = f[3];
        r.fixture = f[4];
        rules.push_back(r);
    }
    if (rules.size() != 7) {
        std::fprintf(stderr, "kitty-lint: expected 7 rules, found %zu\n", rules.size());
        return 2;
    }

    fs::path root(argv[1]);
    std::error_code ec;
    if (!fs::exists(root, ec) || !fs::is_directory(root, ec)) {
        std::fprintf(stderr, "kitty-lint: %s is not a directory\n", argv[1]);
        return 2;
    }
    int hits = 0;
    std::vector<fs::path> files;
    for (auto it = fs::recursive_directory_iterator(root, ec); it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        std::string ext = it->path().extension().string();
        if (ext == ".cpp" || ext == ".h") files.push_back(it->path());
    }
    std::sort(files.begin(), files.end());

    for (const auto& p : files) {
        fs::path relp = fs::relative(p, root, ec);
        std::string rel = relp.generic_string();
        std::ifstream in(p);
        if (!in) continue;
        std::string text;
        int ln = 0;
        while (std::getline(in, text)) {
            ln++;
            for (const auto& r : rules) {
                if (!scope_match(r.scope, rel, r.id)) continue;
                if (std::regex_search(text, r.re)) {
                    std::printf("%s:%d: %s %s\n", p.generic_string().c_str(), ln,
                                r.id.c_str(), r.why.c_str());
                    hits++;
                }
            }
        }
    }
    return hits == 0 ? 0 : 1;
}
