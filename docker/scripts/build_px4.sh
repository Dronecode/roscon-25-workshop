#!/usr/bin/env bash
set -eo pipefail

PX4_VERSION=$1

git clone --depth 1 --shallow-submodules --recurse-submodules -b ${PX4_VERSION} https://github.com/PX4/PX4-Autopilot.git /home/${USER}/PX4-Autopilot
chown -R ${USER}:${USER} /home/${USER}/PX4-Autopilot

cd /home/${USER}/PX4-Autopilot

./Tools/setup/ubuntu.sh --no-nuttx
