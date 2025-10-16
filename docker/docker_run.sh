SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

# Parse command line arguments
NO_GUI=false
NVIDIA=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --no-gui)
            NO_GUI=true
            shift
            ;;
        --nvidia)
            NVIDIA=true
            shift
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [--no-gui] [--nvidia]"
            exit 1
            ;;
    esac
done

# Build docker run command
DOCKER_CMD="docker run -it --rm"

# Add GUI support unless --no-gui is specified
if [ "$NO_GUI" = false ]; then
    DOCKER_CMD="$DOCKER_CMD -v /tmp/.X11-unix:/tmp/.X11-unix:ro"
    DOCKER_CMD="$DOCKER_CMD -e DISPLAY=$DISPLAY"

    # Add nvidia runtime if --nvidia is specified
    if [ "$NVIDIA" = true ]; then
        DOCKER_CMD="$DOCKER_CMD --runtime nvidia"
        DOCKER_CMD="$DOCKER_CMD -e NVIDIA_VISIBLE_DEVICES=all"
        DOCKER_CMD="$DOCKER_CMD -e NVIDIA_DRIVER_CAPABILITIES=all"
    else
        DOCKER_CMD="$DOCKER_CMD --device /dev/dri:/dev/dri"
    fi
else
    DOCKER_CMD="$DOCKER_CMD -p 18570:18570/udp"
fi

# Add common options
DOCKER_CMD="$DOCKER_CMD -p 8765:8765"
DOCKER_CMD="$DOCKER_CMD -v ${SCRIPTPATH}/..:/home/ubuntu/roscon-25-workshop_ws/src/roscon-25-workshop"
DOCKER_CMD="$DOCKER_CMD -v ${SCRIPTPATH}/../px4_roscon_25/px4_roscon_25/gz_models/x500_lidar_2d_mono_cam_down:/home/ubuntu/PX4-gazebo-models/models/x500_lidar_2d_mono_cam_down"
DOCKER_CMD="$DOCKER_CMD --name=px4-roscon-25"
DOCKER_CMD="$DOCKER_CMD dronecode/roscon-25-workshop bash"

# Execute the command
eval $DOCKER_CMD