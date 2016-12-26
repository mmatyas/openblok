#pragma once

#include <memory>
#include <string>


class Music;
class SoundEffect;

/// The audio context of the game.
///
/// Its task is to load individual music and sound effects,
/// and to provide some functions to control all loaded audio (eg. pause).
class AudioContext {
public:
    virtual ~AudioContext() {}

    /// Loads a music file.
    virtual std::shared_ptr<Music> loadMusic(const std::string& path) = 0;
    /// Loads a sound effect file.
    virtual std::shared_ptr<SoundEffect> loadSound(const std::string& path) = 0;

    /// Pauses all playing audio.
    virtual void pauseAll() = 0;
    /// Resumes all paused audio.
    virtual void resumeAll() = 0;
    /// Stops all playing audio. You'll have to call `play()` on them again.
    virtual void stopAll() = 0;

    /// Mutes or unmutes the sound effects.
    virtual void toggleSFXMute() = 0;
    /// Mutes or unmutes the music.
    virtual void toggleMusicMute() = 0;
};
