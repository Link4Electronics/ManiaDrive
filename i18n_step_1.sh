#!/bin/sh
GAME=mania_drive
#GAMEFILE=mania_drive_i18n.c
GAMEFILE=mania_drive.c
mkdir locale
mkdir locale/es
mkdir locale/es/LC_MESSAGES
mkdir locale/fr
mkdir locale/fr/LC_MESSAGES
mkdir locale/it
mkdir locale/it/LC_MESSAGES
echo -e "\n---Generating POT file of the file: $GAMEFILE\n"
xgettext -d $GAME -o $GAME.pot $GAMEFILE
echo -e "\n--- Initializing spanish version...\n"
msginit -l es -o locale/es/LC_MESSAGES/$GAME.po -i $GAME.pot
echo -e "\n--- Initializing french version...\n"
msginit -l fr -o locale/fr/LC_MESSAGES/$GAME.po -i $GAME.pot
echo -e "\n--- Initializing italian version...\n"
msginit -l it -o locale/it/LC_MESSAGES/$GAME.po -i $GAME.pot
echo -e "\n--- Finishing...\n"
echo -e "At this point you have to translate the application in the .po files. After that you will have to compile the po files executing i18n_step_2.sh."
