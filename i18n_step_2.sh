#!/bin/sh
GAME=mania_drive
echo -e "\n--- Compiling spanish version...\n"
msgfmt -c -v -o locale/es/LC_MESSAGES/$GAME.mo locale/es/LC_MESSAGES/$GAME.po && echo "--- Spanish version OK"
echo -e "\n--- Compiling french version...\n"
msgfmt -c -v -o locale/fr/LC_MESSAGES/$GAME.mo locale/fr/LC_MESSAGES/$GAME.po && echo "--- French version OK"
echo -e "\n--- Compiling italian version...\n"
msgfmt -c -v -o locale/it/LC_MESSAGES/$GAME.mo locale/it/LC_MESSAGES/$GAME.po && echo "--- Italian version OK"
