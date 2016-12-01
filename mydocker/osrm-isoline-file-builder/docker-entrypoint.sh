#!/bin/bash

_sig() {
  echo "Shutting down.."
  exit 1
}

trap _sig SIGKILL SIGTERM SIGHUP SIGINT EXIT

#Deletes files older than 7 days
if [ -f $DATA_PATH/$Name_$PROFILE.osrm ]; then
  find $DATA_PATH/$Name_$PROFILE.osrm -mtime +7 -type f -delete
fi
if [ ! -f $DATA_PATH/$Name_$PROFILE.osrm ]; then
  if [ ! -f $DATA_PATH/$Name_$PROFILE.osm.pbf ]; then
      curl $URL > $DATA_PATH/$Name_$PROFILE.osm.pbf
    fi
    ./osrm-extract -p $PROFILE.lua $DATA_PATH/$Name_$PROFILE.osm.pbf
  rm $DATA_PATH/$Name_$PROFILE.osm.pbf
fi
echo "Contraction finished"

