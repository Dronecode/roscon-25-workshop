docker run -it --privileged \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    --rm \
    -e DISPLAY=$DISPLAY \
    --runtime=nvidia \
    -v $(pwd):/home/ubuntu/roscon-25-workshop \
# TO-DO put AppImage in docker
    -v $(pwd)/PX4_Autopilot_SITL-x86_64.AppImage:/home/ubuntu/PX4_Autopilot_SITL-x86_64.AppImage:ro \
    --name=gz \
    px4/roscon-25-workshop bash