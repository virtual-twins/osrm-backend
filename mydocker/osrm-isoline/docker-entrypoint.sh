#!/bin/bash
DATA_PATH=${DATA_PATH:="/osrm-data"}

_sig() {
  kill -TERM $child 2>/dev/null
}

if [ "$1" = 'osrm' ]; then
  trap _sig SIGKILL SIGTERM SIGHUP SIGINT EXIT
  
  if [ ! -f $DATA_PATH/$3_$2.osrm ]; then
    echo "osrm not found"
    exit 1
  fi
  
  ./osrm-isoline $DATA_PATH/$3_$2.osrm
  child=$!
  wait "$child"
else
  exec "$@"
fi
