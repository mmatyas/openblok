#pragma once

#include "ResourceUtils.h"

#include <array>
#include <memory>
#include <stdint.h>

std::array<uint8_t, 3> operator "" _rgb(unsigned long long color);
std::array<uint8_t, 4> operator "" _rgba(unsigned long long color);

/// The graphics context interface used by the game.
///
/// The implementation's task is to provide a graphical window, and create drawable textures,
/// either by loading image files, or by text rendering.
///
/// For testing purposes, it should also provide a way to save screenshots of the game window.
class GraphicsContext {
public:
    /// Called at the end of every frame, `render()` should update
    /// the displayed image of the game window.
    virtual void render() = 0;

    /// Toggle fullscreen mode, if supported.
    virtual void toggleFullscreen() = 0;
    virtual uint16_t screenWidth() = 0;
    virtual uint16_t screenHeight() = 0;

    /// Load a font file (in OTF or TTF format) in the given size.
    /// The client should be able to use the same `ResourceID` later to refer to this font.
    virtual void loadFont(ResourceID, const std::string& path, unsigned pt) = 0;
    /// Render text to a texture, using a previously loaded font,
    /// using the provided RGBA color.
    /// Later, the client should be able to refer and draw this texture, using its `ResourceID`.
    /// The implementation should cache the created texture, if possible.
    virtual void cacheText(ResourceID, const std::string& text,
                           ResourceID font_id, const std::array<uint8_t, 4>& color) = 0;

    /// Load an image file as texture.
    /// The client should be able to use the same `ResourceID` later to refer to this texture.
    virtual void loadTexture(ResourceID, const std::string& path) = 0;
    /// Load an image file as texture with additional tinting.
    /// The client should be able to use the same `ResourceID` later to refer to this texture.
    virtual void loadTexture(ResourceID, const std::string& path, const std::array<uint8_t, 3>& color) = 0;
    /// Draw a previously created texture, referred with a `ResourceID`, at coords (x,y)
    virtual void drawTexture(ResourceID, unsigned x, unsigned y) = 0;
    /// Draw a rectangle on the screen, defined by [x,y,w,h], filled with [r,g,b]
    virtual void drawFilledRect(const std::array<unsigned, 4>& rectangle, const std::array<uint8_t, 3>& color) = 0;

    /// Save a screenshot to the provided path at the end of the current render cycle
    virtual void requestScreenshot(const std::string& path) = 0;

private:
    static std::unique_ptr<GraphicsContext> create();

friend class AppContext;
};
