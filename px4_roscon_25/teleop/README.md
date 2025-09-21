# teleop

This packages offers uses PX4 custom modes to teleop a drone.

## Usage

1. Start the simulation, PX4 and QGC as described in the [docker guide](../../docker/README.md).
2. Run `teleop.launch.py` from inside the docker container

   ```sh
   docker exec -it px4-roscon-25 bash -ic "ros2 launch teleop teleop.launch.py run_uxrcedds_agent:=true"
   ```

The `teleop.launch.py` can also start the _MicroXrceAgent_. Set the launch argument `run_uxrcedds_agent` to `true` to run it.
