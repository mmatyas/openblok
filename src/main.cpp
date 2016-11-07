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


#include "game/AppContext.h"
#include "game/GameState.h"
#include "game/Timing.h"
#include "game/states/InitState.h"
#include "system/Log.h"

#include <chrono>
#include <memory>
#include <thread>
#include <assert.h>


const std::string LOG_MAIN = "main";
const std::string LOG_HELP = "help";

int main(int argc, const char** argv)
{
    Log::info(LOG_MAIN) << "OpenBlok v0.1.0 by Mátyás Mustoha\n";

    for (int arg_i = 1; arg_i < argc; arg_i++) {
        std::string arg = argv[arg_i];
        if (arg == "-v" || arg == "--version")
            return 0;
        else if (arg == "--help") {
            Log::info(LOG_HELP) << "Usage:\n";
            Log::info(LOG_HELP) << "  --help              Display this help then quit\n";
            Log::info(LOG_HELP) << "  -v, --version       Display the version number then quit\n";
            return 0;
        }
        else {
            Log::info(LOG_MAIN) << "Unknown parameter '" << arg << "' skipped.\n";
        }
    }


    AppContext app;
    if (!app.init())
        return 1;


    try { app.states().emplace(std::make_unique<InitState>(app)); }
    catch (const std::exception& err) {
        app.window().showErrorMessage(err.what());
        return 1;
    }


    auto frame_starttime = std::chrono::steady_clock::now();
    auto frame_endtime = frame_starttime + Timing::frame_duration;

    while (!app.window().quitRequested()) {
        try {
            auto events = app.window().collectEvents();

            assert(app.states().size());
            app.states().top()->update(events, app);
            if (app.states().empty())
                break;

            app.states().top()->draw(app.gcx());
            app.gcx().render();
        }
        catch (const std::exception& err) {
            app.window().showErrorMessage(err.what());
            return 1;
        }

        // frame rate limiting
        std::this_thread::sleep_until(frame_endtime);
        frame_starttime = std::chrono::steady_clock::now();
        frame_endtime = frame_starttime + Timing::frame_duration;
    }

    return 0;
}
