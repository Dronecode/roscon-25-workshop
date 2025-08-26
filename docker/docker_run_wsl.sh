SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

docker run -it \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    --rm \
    -e DISPLAY=$DISPLAY \
    -e NVIDIA_VISIBLE_DEVICES=all \
    -e NVIDIA_DRIVER_CAPABILITIES=all \
    -v ${SCRIPTPATH}/../px4_roscon_25:/home/ubuntu/roscon-25-workshop_ws/src/ \
    --name=px4-roscon-25 \
    --runtime nvidia \
    px4/roscon-25-workshop bash