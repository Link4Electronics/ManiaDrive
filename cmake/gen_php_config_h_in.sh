#!/bin/sh
# Generate php_config.h.in from config.status's embedded define list,
# then re-run config.status to produce a complete php_config.h.
set -e
cd "$1"
sed -n 's/.*${ac_uA}\([A-Z_0-9]*\)${ac_uB}.*/\1/p' config.status \
  | sort -u \
  | awk '{print "#undef " $0}' > "$2/main/php_config.h.in"
./config.status
