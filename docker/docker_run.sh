SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

docker run -it \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    --rm \
    -e NVIDIA_VISIBLE_DEVICES=all \
    -e NVIDIA_DRIVER_CAPABILITIES=all \
    --device /dev/dri:/dev/dri \
    -e DISPLAY=$DISPLAY \
    -v ${SCRIPTPATH}/../px4_roscon_25:/home/ubuntu/roscon-25-workshop_ws/src/ \
    --name=px4-roscon-25 \
    px4/roscon-25-workshop bash