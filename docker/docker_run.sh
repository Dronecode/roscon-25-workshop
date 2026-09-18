#!/bin/bash

SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

HEADLESS="${HEADLESS:-0}"
IMAGE="${IMAGE:-px4io/px4-dev-ros2-gazebo:main-jazzy}"


CONTAINER_NAME=px4-dev-ros2-gazebo
running_container=$(docker ps -q --filter "name=^/${CONTAINER_NAME}$" --filter status=running)
if [[ -n "$running_container" ]]; then
	exec docker exec -it "$CONTAINER_NAME" bash
fi

if [[ $# -lt 1 ]]; then
	echo "Usage: $0 PX4_DIR" >&2
	exit 1
fi

PX4_DIR=$1

WS_SRC_DIR=$SCRIPTPATH/../../..

set -euo pipefail

if [[ "$HEADLESS" -eq 1 ]]; then
	docker run --rm -it --name "$CONTAINER_NAME" --network host \
		--mount "type=bind,src=$WS_SRC_DIR,dst=/workspace" \
		--mount "type=bind,src=$PX4_DIR,dst=/PX4-Autopilot" \
		-e ROS_DOMAIN_ID=83 \
		-e GIT_CONFIG_COUNT=1 -e GIT_CONFIG_KEY_0=safe.directory -e "GIT_CONFIG_VALUE_0=*" \
		-e PX4_PATH=/PX4-Autopilot \
		-e HEADLESS \
		"$IMAGE"
else
	umask 077
	: "${DISPLAY:?Open a terminal in your local graphical session}"
	auth=$(mktemp)
	trap 'rm -f "$auth"' EXIT

	xauth nlist "$DISPLAY" |
	sed 's/^..../ffff/' |
	xauth -f "$auth" nmerge -
	if [ ! -s "$auth" ]; then
		echo "No Xauthority cookie for $DISPLAY; check the host XAUTHORITY setting." >&2
		exit 1
	fi

	docker run --rm -it --name "$CONTAINER_NAME" --network host \
		--mount type=bind,src=/tmp/.X11-unix,dst=/tmp/.X11-unix,readonly \
		--mount "type=bind,src=$auth,dst=/tmp/px4.xauth,readonly" \
		--mount "type=bind,src=$WS_SRC_DIR,dst=/workspace/src" \
		--mount "type=bind,src=$PX4_DIR,dst=/PX4-Autopilot" \
		-e "DISPLAY=$DISPLAY" -e XAUTHORITY=/tmp/px4.xauth \
		-e QT_QPA_PLATFORM=xcb -e LIBGL_ALWAYS_SOFTWARE=1 \
		-e ROS_DOMAIN_ID=83 \
		-e GIT_CONFIG_COUNT=1 -e GIT_CONFIG_KEY_0=safe.directory -e "GIT_CONFIG_VALUE_0=*" \
		-e PX4_PATH=/PX4-Autopilot \
		"$IMAGE"
fi