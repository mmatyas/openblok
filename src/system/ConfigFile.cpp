#include "ConfigFile.h"

#include "Log.h"

#include <assert.h>
#include <fstream>
#include <regex>


const std::string LOG_TAG("config");

ConfigFile::Data ConfigFile::load(const std::string& path)
{
    std::ifstream infile(path);
    if (!infile.is_open())
        return Data();

    const std::regex valid_head(R"(\[[a-zA-Z0-9\.-_,: \(\)]+\])");
    const std::regex valid_data(R"([a-z_]+\s*=\s*[a-z0-9_, ]+)");
    const std::regex whitespace(R"(\s+)");

    ConfigFile::Data output;
    std::string current_head;

    unsigned linenum = 0;
    std::string line;
    while (std::getline(infile, line)) {
        linenum++;
        if (line.empty() || line.front() == '#')
            continue;

        if (std::regex_match(line, valid_head)) {
            current_head = line.substr(1, line.length() - 2);
            assert(!current_head.empty()); // the regex requires at least one char
        }
        else if (std::regex_match(line, valid_data)) {
            if (current_head.empty()) {
                Log::warning(LOG_TAG) << path << ":" << linenum << ": No block header defined before this line\n";
                Log::warning(LOG_TAG) << "Using default settings\n";
                return Data();
            }
            line = std::regex_replace(line, whitespace, "");
            const auto split_pos = line.find("=");
            const auto key = line.substr(0, split_pos);
            const auto val = line.substr(split_pos + 1);
            output[current_head].emplace(std::move(key), std::move(val));
        }
        else {
            Log::warning(LOG_TAG) << path << ":" << linenum << ": Syntax error\n";
            Log::warning(LOG_TAG) << "Using default settings\n";
            return Data();
        }
    }
    return output;
}

void ConfigFile::save(const ConfigFile::Data& data, const std::string& path)
{
    std::ofstream outfile(path);
    if (!outfile.is_open()) {
        Log::warning(LOG_TAG) << "Could not write config file '" << path << "'\n";
        return;
    }

    for (const auto& block : data) {
        outfile << "[" << block.first << "]\n";
        for (const auto& keyval : block.second)
            outfile << keyval.first << " = " << keyval.second << "\n";
        outfile << "\n";
    }
}
