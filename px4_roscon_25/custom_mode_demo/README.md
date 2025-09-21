# Custom mode demo

This packages offers a demo of how to use PX4 custom modes from ROS 2.

## Usage

1. Start the simulation, PX4 and QGC as described in the [docker guide](../../docker/README.md).
2. Run `custom_mode_demo.launch.py` from inside the docker container

   ```sh
   docker exec -it px4-roscon-25 bash -ic "ros2 launch custom_mode_demo custom_mode_demo.launch.py run_uxrcedds_agent:=true"
   ```

The `custom_mode_demo.launch.py` can also start the _MicroXrceAgent_. Set the launch argument `run_uxrcedds_agent` to `true` to run it.
