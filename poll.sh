#!/bin/bash
set -Eeu
set -o posix
set -o pipefail

declare -ir sleep_secs="$1"

cd "$(dirname "$0")"

while True; do
    ./main
    sleep $sleep_secs
done
