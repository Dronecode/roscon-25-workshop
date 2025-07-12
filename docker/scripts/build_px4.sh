#!/usr/bin/env bash
set -eo pipefail

PX4_VERSION=$1

git clone  --recurse-submodules -b ${PX4_VERSION} https://github.com/PX4/PX4-Autopilot.git /home/${USER}/PX4-Autopilot

cd /home/${USER}/PX4-Autopilot
./Tools/setup/ubuntu.sh --no-nuttx --no-sim-tools
make px4_sitl_default
cd ..
mkdir -p px4_sitl/bin
mkdir -p px4_sitl/romfs/etc
cp PX4-Autopilot/build/px4_sitl_default/bin/px4* px4_sitl/bin/
tar xf PX4-Autopilot/build/px4_sitl_default/romfs_files.tar -C px4_sitl/romfs/etc