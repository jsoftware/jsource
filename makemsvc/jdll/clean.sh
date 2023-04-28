#!/bin/sh
cd "$(dirname "$0")"
find ../.. -name '*.c' | sed 's}c$}o}' | grep -v ' ' | xargs rm -f
find ../.. -name '*.S' | sed 's}S$}o}' | grep -v ' ' | xargs rm -f
