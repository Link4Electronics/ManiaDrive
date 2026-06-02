#!/bin/sh

if [ -z "$1" ]; then
    echo "Midnight Commander text editor (mcedit) is using a tab width of 8,"
    echo "and fakes half-tabs with 4 space. This script will convert this"
    echo "style to a more regular one (tab width of 4, tab as spaces)."
    echo
    echo "Usage:"
    echo "	mcedit_to_tab4.sh file(s)"
    echo "	yes | mcedit_to_tab4.sh file(s)"
    exit 1
fi

if [ -z "$TMPDIR" ]; then
    TMPDIR="/tmp"
fi

until [ -z "$1" ]; do
    echo -n "overwrite '$1' ? (y/n): "
    read rep

    if [ "$rep" = "y" ]; then
#	expand "$1" > $TMPDIR/ex1 && unexpand -4 $TMPDIR/ex1 > "$1" && \
	expand "$1" > "$TMPDIR/ex1" && cp -f "$TMPDIR/ex1" "$1" && \
	echo "$1 converted"
    fi

    shift
done
