#!/bin/bash

_sig() {
  kill -TERM $child 2>/dev/null
}


trap _sig SIGKILL SIGTERM SIGHUP SIGINT EXIT
  
if [ ! -f $DATA_PATH/$NAME_$PROFILE.osrm ]; then
  echo "osrm not found"
  exit 1
fi
  
./osrm-isoline -p $PORT $DATA_PATH/$NAME_$PROFILE.osrm
child=$!
wait "$child"

