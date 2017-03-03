#!/bin/bash

########################################################################
# Package the binaries built on Travis-CI as an AppImage
# By Simon Peter 2016
# For more information, see http://appimage.org/
########################################################################

export ARCH=$(arch)

APP=openblok
LOWERAPP=${APP,,}

GIT_REV=$(git rev-parse --short HEAD)
echo $GIT_REV

make install/strip DESTDIR=$HOME/$APP/$APP.AppDir
mv $HOME/$APP/$APP.AppDir/usr/{games,bin}

cd $HOME/$APP/

wget -q https://github.com/probonopd/AppImages/raw/master/functions.sh -O ./functions.sh
. ./functions.sh

cd $APP.AppDir

########################################################################
# Copy desktop and icon file to AppDir for AppRun to pick them up
########################################################################

get_apprun
get_desktop
get_icon

########################################################################
# Copy in the dependencies that cannot be assumed to be available
# on all target systems
########################################################################

copy_deps
move_lib

ln -s pulseaudio/libpulsecommon-4.0.so ./usr/lib/x86_64-linux-gnu/

########################################################################
# Delete stuff that should not go into the AppImage
########################################################################

# Delete dangerous libraries; see
# https://github.com/probonopd/AppImages/blob/master/excludelist
delete_blacklisted

rm -rf ./usr/share/applications
rm -rf ./usr/share/pixmaps

########################################################################
# desktopintegration asks the user on first run to install a menu item
########################################################################

get_desktopintegration $APP

########################################################################
# Determine the version of the app; also include needed glibc version
########################################################################

GLIBC_NEEDED=$(glibc_needed)
VERSION=git$GIT_REV-glibc$GLIBC_NEEDED

########################################################################
# Patch away absolute paths; it would be nice if they were relative
########################################################################

patch_usr
mv ./usr/{share,bin/}

########################################################################
# AppDir complete
# Now packaging it as an AppImage
########################################################################

cd .. # Go out of AppImage

mkdir -p ../out/

BINTRAY_USER=mmatyas
BINTRAY_REPO=appimages
generate_type2_appimage

########################################################################
# Upload the AppDir
########################################################################

transfer ../out/*
