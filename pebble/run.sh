#!/bin/sh
if [[ "$1" == "" ]]; then
    echo "usage: $0 <pebble sdk directory>"
    exit 1
fi
PEBBLE_SDK=$1
cd $(realpath $(dirname $0))
. $PEBBLE_SDK/.env/bin/activate
$PEBBLE_SDK/bin/pebble clean && \
    $PEBBLE_SDK/bin/pebble build && \
    $PEBBLE_SDK/bin/pebble install --cloudpebble && \
    $PEBBLE_SDK/bin/pebble logs --cloudpebble
