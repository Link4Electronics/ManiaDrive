#!/bin/sh

#######################
# Geany Symbols Script. 
#######################

# Little script to generate a symbol list with all the functions of Raydium. 
# For use only with Geany ( a lightweight text editor for coders). 
# It also "installs" the list of symbols copying it to the local folder of 
# tags in geany. All C files opened with Geany will have Raydium 
# autocompletion and syntax help.

HEADERS=../../raydium/headers
GEANYLOCAL=~/.geany/tags/

# We may be called from any path, so let's
# may sure our relative path to Raydium headers will be correct.
cd "$(dirname "$0")"

geany -g -P raydium-tags-tmp.c.tags $HEADERS/*.h
mkdir -p $GEANYLOCAL
mv -f raydium-tags-tmp.c.tags $GEANYLOCAL/raydium.c.tags

