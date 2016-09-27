#pragma once

#include "Color.h"
#include "ResourceTypes.h"

#include <memory>
#include <stdint.h>


/// Helper objects

struct Rectangle {
    int x, y, w, h;
};

/// The graphics context interface used by the game.
///
/// The implementation's task is to provide a graphical window, and create drawable textures,
/// either by loading image files, or by text rendering.
///
/// For testing purposes, it should also provide a way to save screenshots of the game window.
class GraphicsContext {
public:
    virtual ~GraphicsContext() {};

    /// Called at the end of every frame, `render()` should update
    /// the displayed image of the game window.
    virtual void render() = 0;

    /// Toggle fullscreen mode, if supported.
    virtual void toggleFullscreen() = 0;
    virtual uint16_t screenWidth() const = 0;
    virtual uint16_t screenHeight() const = 0;

    /// Load a font file (in OTF or TTF format) in the given size.
    /// Returns a unique ID that the client can use to refer to this font.
    virtual FontID loadFont(const std::string& path, unsigned pt) = 0;
    /// Force unloading a font.
    virtual void unloadFont(FontID) = 0;
    /// Render text to a texture, using a previously loaded font,
    /// using the provided RGBA color.
    /// Returns a unique ID that the client can use to refer to the rendered texture.
    virtual TextureID renderText(const std::string& text,
                                 FontID font, const RGBColor& color) = 0;
    /// Render text to a texture, using a previously loaded font,
    /// using the provided RGBA color,
    /// to a texture slot, replacing its contents.
    virtual void renderText(TextureID target_slot, const std::string& text,
                            FontID font, const RGBColor&) = 0;

    /// Load an image file as texture.
    /// Returns a unique ID that the client can use to refer to this texture.
    virtual TextureID loadTexture(const std::string& path) = 0;
    /// Load an image file as texture with additional tinting.
    /// Returns a unique ID that the client can use to refer to this texture.
    virtual TextureID loadTexture(const std::string& path, const RGBColor& tint) = 0;
    /// Force unloading a texture.
    virtual void unloadTexture(TextureID) = 0;

    /// Draw a previously created texture at coords (x,y)
    virtual void drawTexture(TextureID, unsigned x, unsigned y) = 0;
    /// Draw a previously created texture,
    /// moved and scaled to the position and size of a rectangle defined by [x,y,w,h]
    virtual void drawTexture(TextureID, const Rectangle& rectangle) = 0;
    /// Draw a rectangle on the screen, defined by [x,y,w,h], filled with [r,g,b]
    virtual void drawFilledRect(const Rectangle& rectangle, const RGBColor& color) = 0;
    /// Draw a rectangle on the screen, defined by [x,y,w,h], filled with the optionally transparent color [r,g,b,a]
    virtual void drawFilledRect(const Rectangle& rectangle, const RGBAColor& color) = 0;

    virtual unsigned textureWidth(TextureID) const = 0;
    virtual unsigned textureHeight(TextureID) const = 0;

    /// Save a screenshot to the provided path at the end of the current render cycle
    virtual void requestScreenshot(const std::string& path) = 0;

private:
    static std::unique_ptr<GraphicsContext> create();

friend class AppContext;
};
