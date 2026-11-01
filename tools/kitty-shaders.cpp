#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

static std::string quote(const std::string& s) {
    std::string out = "'";
    for (char c : s) {
        if (c == '\'') out += "'\\''";
        else out += c;
    }
    out += "'";
    return out;
}

static int run(const std::string& cmd) {
    int rc = std::system(cmd.c_str());
    return rc == 0 ? 0 : 1;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: kitty-shaders SRC_DIR OUT_DIR\n");
        return 2;
    }
    std::string src = argv[1];
    std::string out = argv[2];
    std::error_code ec;
    fs::create_directories(out, ec);
    if (!fs::is_directory(src, ec)) {
        std::fprintf(stderr, "kitty-shaders: %s is not a directory\n", src.c_str());
        return 2;
    }
    std::vector<fs::path> files;
    for (auto it = fs::directory_iterator(src, ec); it != fs::directory_iterator(); it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        std::string ext = it->path().extension().string();
        if (ext == ".vert" || ext == ".frag") files.push_back(it->path());
    }
    std::sort(files.begin(), files.end());
    int bad = 0;
    for (const auto& p : files) {
        std::string stem = p.stem().string();
        std::string spv = (fs::path(out) / (stem + ".spv")).string();
        std::string cmd = "glslangValidator -V --target-env vulkan1.3 " + quote(p.string()) + " -o " + quote(spv);
        int rc = run(cmd);
        std::printf("%s rc=%d\n", stem.c_str(), rc);
        if (rc) { bad = 1; continue; }
        int vrc = run("spirv-val --target-env vulkan1.3 " + quote(spv));
        std::printf("%s spirv-val rc=%d\n", stem.c_str(), vrc);
        if (vrc) bad = 1;
    }
    return bad;
}
