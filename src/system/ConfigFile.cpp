#include "ConfigFile.h"

#include "Log.h"

#include <assert.h>
#include <fstream>
#include <regex>


const std::string LOG_TAG("config");

ConfigFile::Blocks ConfigFile::load(const std::string& path)
{
    std::ifstream infile(path);
    if (!infile.is_open())
        return Blocks();

    const std::regex valid_head(R"(^\[[a-zA-Z0-9\.-_,: \(\)]+\]$)");
    const std::regex valid_data(R"(^[a-z_]+\s*=\s*([a-zA-Z0-9_, ]+|".*?"|#[a-fA-F0-9]{6,8})$)");
    const std::regex whitespace_left(R"(^\s+)");
    const std::regex whitespace_right(R"(\s+$)");

    ConfigFile::Blocks output;
    std::string current_head;

    unsigned linenum = 0;
    std::string line;
    while (std::getline(infile, line)) {
        linenum++;
        if (line.empty() || line.front() == '#')
            continue;

        if (regex_match(line, valid_head)) {
            current_head = line.substr(1, line.length() - 2);
            assert(!current_head.empty()); // the regex requires at least one char
        }
        else if (regex_match(line, valid_data)) {
            if (current_head.empty()) {
                Log::warning(LOG_TAG) << path << ":" << linenum << ": No block header defined before this line\n";
                Log::warning(LOG_TAG) << "Using default settings\n";
                return Blocks();
            }
            const auto split_pos = line.find("=");
            auto key = line.substr(0, split_pos);
            key = regex_replace(key, whitespace_left, "");
            key = regex_replace(key, whitespace_right, "");
            auto val = line.substr(split_pos + 1);
            val = regex_replace(val, whitespace_left, "");
            val = regex_replace(val, whitespace_right, "");
            output[current_head].emplace(std::move(key), std::move(val));
        }
        else {
            Log::warning(LOG_TAG) << path << ":" << linenum << ": Syntax error\n";
            Log::warning(LOG_TAG) << "Using default settings\n";
            return Blocks();
        }
    }
    return output;
}

void ConfigFile::save(const ConfigFile::Blocks& blocks, const std::string& path)
{
    std::ofstream outfile(path);
    if (!outfile.is_open()) {
        Log::warning(LOG_TAG) << "Could not write config file '" << path << "'\n";
        return;
    }

    for (const auto& block : blocks) {
        outfile << "[" << block.first << "]\n";
        for (const auto& keyval : block.second)
            outfile << keyval.first << " = " << keyval.second << "\n";
        outfile << "\n";
    }
}

bool ConfigFile::parseBool(const std::pair<std::string, std::string>& keyval)
{
    if (keyval.second == "on" || keyval.second == "yes" || keyval.second == "true")
        return true;
    else if (keyval.second == "off" || keyval.second == "no" || keyval.second == "false")
        return false;
    else {
        throw std::runtime_error(
            "Invalid value for '" + keyval.first + "', " +
            "expected one of {on,off,yes,no,true,false}, got '" + keyval.second + "'");
    }
}
