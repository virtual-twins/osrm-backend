#!/bin/bash
DATA_PATH=${DATA_PATH:="/osrm-data"}


_sig() {
  echo "Shutting down.."
  exit 1
}

if [ "$1" = 'osrm' ]; then
  trap _sig SIGKILL SIGTERM SIGHUP SIGINT EXIT

#Deletes files older than 7 days
  if [ -f $DATA_PATH/$3_$2.osrm ]; then
    find $DATA_PATH/$3_$2.osrm -mtime +7 -type f -delete
  fi
  if [ ! -f $DATA_PATH/$3_$2.osrm ]; then
    if [ ! -f $DATA_PATH/$3_$2.osm.pbf ]; then
      curl $4 > $DATA_PATH/$3_$2.osm.pbf
    fi
    ./osrm-extract -p $2.lua $DATA_PATH/$3_$2.osm.pbf
    rm $DATA_PATH/$3_$2.osm.pbf
  fi
  echo "Contraction finished"
else
  exec "$@"
fi
