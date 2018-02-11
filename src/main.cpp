// OpenBlok
// Copyright (C) 2016  Mátyás Mustoha
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "version.h"
#include "game/AppContext.h"
#include "game/GameState.h"
#include "game/Timing.h"
#include "game/states/InitState.h"
#include "system/Log.h"
#include "system/Paths.h"
#include "system/util/MakeUnique.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <assert.h>
#include <emscripten.h>


const std::string LOG_MAIN = "main";
const std::string LOG_HELP = "help";

AppContext app;
void runNextFrame();

int main(int argc, const char** argv)
{
    Log::info(LOG_MAIN) << "OpenBlok, created by Mátyás Mustoha, " << game_version << "\n";

    for (int arg_i = 1; arg_i < argc; arg_i++) {
        std::string arg = argv[arg_i];
        if (arg == "-v" || arg == "--version")
            return 0;
        else if (arg == "--help") {
            Log::info(LOG_HELP) << "Usage:\n";
            Log::info(LOG_HELP) << "  -v, --version            Display the version number then quit\n";
            Log::info(LOG_HELP) << "  --help                   Display this help then quit\n";
            Log::info(LOG_HELP) << "  --data <dir>             Load game resources from the <dir> directory\n";
            return 0;
        }
        else if (arg == "--data") {
            if (++arg_i >= argc) {
                Log::error(LOG_MAIN) << "'--data' requires a directory as parameter!\n";
                return 1;
            }
            Paths::changeDataDir(argv[arg_i]);
        }
        else {
            Log::error(LOG_MAIN) << "Unknown parameter '" << arg << "'.\n";
            return 1;
        }
    }


    if (!app.init())
        return 1;


    try { app.states().emplace(std::make_unique<InitState>(app)); }
    catch (const std::exception& err) {
        app.window().showErrorMessage(err.what());
        return 1;
    }

    emscripten_set_main_loop(runNextFrame, 0, 1);

    // save input config on exit
    const auto mappings = app.window().createInputConfig();
    app.inputconfig().save(mappings, Paths::config() + "input.cfg");

    return 0;
}

void runNextFrame()
{
    static auto gametime_delay = Timing::frame_duration; // start with an update

    auto frame_starttime = std::chrono::steady_clock::now();
    auto frame_planned_endtime = frame_starttime + Timing::frame_duration;

    try {
        while (gametime_delay >= Timing::frame_duration && !app.states().empty()) {
            auto events = app.window().collectEvents();
            app.states().top()->update(events, app);
            gametime_delay -= Timing::frame_duration;
        }
        if (app.states().empty())
            return;

        app.states().top()->draw(app.gcx());
        app.gcx().render();
    }
    catch (const std::exception& err) {
        app.window().showErrorMessage(err.what());

    }

    auto lag = std::max(std::chrono::steady_clock::now() - frame_planned_endtime, Duration::zero());
    gametime_delay += Timing::frame_duration + lag;
}
