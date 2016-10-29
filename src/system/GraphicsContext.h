#pragma once

#include "Color.h"
#include "Rectangle.h"

#include <memory>
#include <string>


class Font;
class Texture;

/// The graphics context interface used by the game.
///
/// The implementation's task is to provide a way to draw inside the graphical window,
/// by loading image files and fonts, and also drawing simple shapes directly on the screen.
///
/// For testing purposes, it should also provide a way to save screenshots of the game window.
class GraphicsContext {
public:
    virtual ~GraphicsContext() {};

    /// Called at the end of every frame, `render()` should update
    /// the displayed image of the game window.
    virtual void render() = 0;

    virtual unsigned short screenWidth() const = 0;
    virtual unsigned short screenHeight() const = 0;

    virtual float getDrawScale() const = 0;
    virtual void modifyDrawScale(float scale) = 0;

    /// Load a font file (in OTF or TTF format) in the given size.
    virtual std::shared_ptr<Font> loadFont(const std::string& path, unsigned pt) = 0;
    /// Load an image file as texture.
    virtual std::unique_ptr<Texture> loadTexture(const std::string& path) = 0;
    /// Load an image file as texture with additional tinting.
    virtual std::unique_ptr<Texture> loadTexture(const std::string& path, const RGBColor& tint) = 0;

    /// Draw a rectangle on the screen, defined by [x,y,w,h], filled with [r,g,b]
    virtual void drawFilledRect(const Rectangle& rectangle, const RGBColor& color) = 0;
    /// Draw a rectangle on the screen, defined by [x,y,w,h], filled with the optionally transparent color [r,g,b,a]
    virtual void drawFilledRect(const Rectangle& rectangle, const RGBAColor& color) = 0;
};
