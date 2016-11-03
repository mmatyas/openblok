#include "Window.h"

#include "Log.h"
#include "sdl/SDLWindow.h"


std::unique_ptr<Window> Window::create()
{
    return std::make_unique<SDLWindow>();
}

void Window::showErrorMessage(const std::string& content)
{
    Log::error("critical") << content << "\n";
    SDLWindow::showErrorMessage("Error", content);
}
