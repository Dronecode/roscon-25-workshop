docker run -it --privileged \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    --rm \
    -e DISPLAY=$DISPLAY \
    --runtime=nvidia \
    -v $(pwd):/roscon-25-workshop \
    --name=gz \
    px4/roscon-25-workshop bash