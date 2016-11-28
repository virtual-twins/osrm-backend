#!/usr/bin/env bash

set -e
set -o pipefail

docker build -t naviki/osrm-isoline-file-builder:latest .
