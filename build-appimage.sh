#!/bin/bash

# $1 - src dir, $2 - build dir

die()
{
    local rc="$1"
    shift
    echo
    echo "$*"
    echo
    popd > /dev/null
    exit $rc
}


locate_or_download_linuxdeployqt()
{
    which linuxdeployqt > /dev/null && return 3
    # trying to find it in current dir.
    export PATH="$PATH:./"
    which linuxdeployqt > /dev/null && return 3
    echo "linuxdeployqt not found, will try to download it"
    wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage &&
        echo "wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage OK" || echo "wget error"
    chmod +x ./linuxdeployqt-continuous-x86_64.AppImage
    ln -s ./linuxdeployqt-continuous-x86_64.AppImage ./linuxdeployqt
    return 0
}


pushd "." > /dev/null
[ -d "$1" ] || die 1 "No such dir: $1"
[ -d "$2" ] || die 1 "No such dir: $2"
src_dir=$(realpath "$1")
popd > /dev/null
pushd "$2" > /dev/null
locate_or_download_linuxdeployqt


os_name=$(sed -n "/^ID=/ s/^ID=// p" /etc/os-release)
os_ver=$(sed -n "/^VERSION_ID=/ s/^VERSION_ID=\"\([^\"]*\)\"/\1/ p" /etc/os-release)

qmake "$src_dir" || die 2 "qmake error"

if [ $os_name=="ubuntu" -a $os_ver=="16.04" ]; then
    cxxflags="-std=c++11 -fPIC "
    echo "Ubuntu 16.04 xenial detected, using custom CXX flags: $cxxflags"
fi

make CXXFLAGS="$cxxflags" || die 2 "make error"
mkdir -p AppDir || die 2 "mkdir error"
make install INSTALL_ROOT=AppDir || die 2 "make install error"

./linuxdeployqt AppDir/share/applications/noteshrink-qt.desktop -no-translations -appimage -no-strip || die 2 "linuxdeployqt error"

popd > /dev/null

exit 0
