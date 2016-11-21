#! /bin/bash

set -o nounset
set -o errexit

command -v flatpak >/dev/null 2>&1 || {
    echo >&2 'Error: Flatpak is not installed'
    echo >&2 'Please visit `http://flatpak.org/` for installation instructions'
    exit 1
}

set -o xtrace

flatpak info org.freedesktop.Platform || {
    wget https://sdk.gnome.org/keys/gnome-sdk.gpg -P /tmp
    flatpak remote-add --gpg-import=/tmp/gnome-sdk.gpg gnome https://sdk.gnome.org/repo/
    flatpak install gnome org.freedesktop.Platform 1.4
    flatpak install gnome org.freedesktop.Sdk 1.4
}

rm -rf app
flatpak-builder --repo=repo app manifest.json
flatpak build-update-repo --prune repo

flatpak --user remote-add --no-gpg-verify openblok ./repo || true
flatpak --user -v install openblok hu.matyasmustoha.OpenBlok || true

flatpak update --user hu.matyasmustoha.OpenBlok
