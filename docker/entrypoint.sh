#!/bin/bash
set -eo pipefail

# setup ros2 environment
echo "source /home/$USER/roscon-25-workshop_ws/install/setup.bash" >> /home/$USER/.bashrc
echo "source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash" >> /home/$USER/.bashrc

source /home/$USER/.bashrc
exec "$@"
