#!/bin/sh

# This script will build a release archive of Raydium

if [ ! -d "build" ]; then
    echo "creating build directory"
    mkdir build
fi


# create names
file="raydium-`date +%Y-%m-%d`"
directory="build/$file"

if [ -d "$directory" ]; then
    echo "$directory already exists !"
    exit 1
fi

echo "creating $directory ..."
mkdir "$directory"

if [ "$?" != "0" ]; then
    echo "error creating $directory"
    exit 1
fi

echo "downloading lastest SVN version ..."
svn checkout svn://raydium.org/raydium/trunk $directory

# build archive
cd build
tar czf "$file.tar.gz" "$file"

if [ "$?" != "0" ]; then
    echo "error creating tar.gz archive"
    exit 1
fi

echo "archive is ready in build directory"
