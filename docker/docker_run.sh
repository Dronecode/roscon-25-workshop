docker run -it \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    --rm \
    -e DISPLAY=$DISPLAY \
    -v $(pwd)/px4_roscon_25:/home/ubuntu/roscon-25-workshop_ws/src/ \
    -v $(pwd)/PX4_Autopilot_SITL-x86_64.AppImage:/home/ubuntu/PX4_Autopilot_SITL-x86_64.AppImage:ro \
    --name=px4-roscon-25 \
    px4/roscon-25-workshop bash