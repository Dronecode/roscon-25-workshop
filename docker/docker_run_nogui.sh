SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

docker run -it \
    --rm \
    -v ${SCRIPTPATH}/../px4_roscon_25:/home/ubuntu/roscon-25-workshop_ws/src/ \
    --name=px4-roscon-25 \
    -p 18570:18570/udp \
    -p 8765:8765 \
    dronecode/roscon-25-workshop bash