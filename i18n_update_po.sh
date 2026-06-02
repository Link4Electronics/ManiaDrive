#!/bin/sh
GAME=mania_drive
#GAMEFILE=mania_drive_i18n.c
GAMEFILE=mania_drive.c
echo -e "\n---Generating new POT file of the file: $GAMEFILE\n"
xgettext -d $GAME -o tmp.pot $GAMEFILE
echo -e "\n--- Updating spanish version...\n"
msgmerge locale/es/LC_MESSAGES/$GAME.po tmp.pot -o locale/es/LC_MESSAGES/$GAME.po && echo -e "-- updated spanish po file"
echo -e "\n--- Updating french version...\n" 
msgmerge locale/fr/LC_MESSAGES/$GAME.po tmp.pot -o locale/fr/LC_MESSAGES/$GAME.po && echo -e "-- updated french po file"
echo -e "\n--- Updating italian version...\n"
msgmerge locale/it/LC_MESSAGES/$GAME.po tmp.pot -o locale/it/LC_MESSAGES/$GAME.po && echo -e "-- updatet italian po file"
rm tmp.pot
