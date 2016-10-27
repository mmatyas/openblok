#include "LuaRotationFn.h"

#include "system/Log.h"

#include <exception>


const std::string LOG_TAG = "lua_rotation_plugin";

LuaRotationFn::LuaRotationFn(const std::string& scriptfile)
    : RotationFn(scriptfile)
{
    lua.open_libraries(sol::lib::base, sol::lib::table);
    try {
        lua.script_file(scriptfile);
    }
    catch (const sol::error& err) {
        throw std::runtime_error(err.what());
    }

    sol::optional<std::string> name = lua["plugin_name"];
    if (name != sol::nullopt)
        plugin_name = lua["plugin_name"];

    fn = lua["possibleRotations"];
    if (!fn.valid())
        throw std::runtime_error("A rotation plugin must have a possibleRotations function");
}

std::vector<std::pair<int, int>> LuaRotationFn::call(PieceType piece, PieceDirection from, bool clockwise)
{
    std::vector<std::pair<int, int>> output;

    auto fn_call = fn(toAscii(piece), toAscii(from), clockwise);
    if (!fn_call.valid()) {
        sol::error err = fn_call;
        Log::error(LOG_TAG) << err.what() << '\n';
        return output;
    }

    sol::table arr = fn_call;
    for (unsigned i = 1; i <= arr.size(); i++) {
        sol::table pair = arr[i];
        if (pair.valid()) {
            if (pair.size() != 2) {
                Log::error(LOG_TAG) << "offsets must have 2 elements\n";
                continue;
            }
            sol::optional<int> x = pair[1];
            sol::optional<int> y = pair[2];
            if (x == sol::nullopt || y == sol::nullopt) {
                Log::error(LOG_TAG) << "offsets must contain integers\n";
                continue;
            }
            output.emplace_back(pair[1], pair[2]);
        }
    }

    return output;
}
