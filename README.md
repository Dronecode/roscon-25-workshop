<h1 align="center">🚀 PX4 + ROS 2 ROSCon 2025 Workshop</h1>

<p align="center">
    <strong>Fly with ROS 2. Powered by PX4</strong>
</p>
<p align="center">
    <a href="https://creativecommons.org/licenses/by-sa/4.0/">
        <img src="https://img.shields.io/badge/License-CC_BY--SA_4.0-lightgrey.svg" alt="License: CC BY-SA 4.0">
    </a>
</p>

## About
This full-day workshop is designed for ROS 2 developers ready to dive into aerial robotics with PX4. You’ll connect ROS 2 nodes to PX4’s real-time flight stack, stream telemetry into RViz, build offboard control logic, and integrate perception using Gazebo, ArUco markers, and LiDAR. By the end, you’ll have a complete simulation environment, reusable code, and a path to real hardware—no prior PX4 knowledge required.

This repository contains all the materials for the ROSCon 2025 PX4 + ROS 2 Workshop.

## Outline

### Introduction & Drone Architecture
- Why aerial robotics is hard (real-time constraints, safety, sensor fusion)  
- Common drone components overview: flight controller, IMU, GPS, LiDAR, camera, companion computer, motors/ESCs, radio link  
- PX4’s role in the Dronecode ecosystem 
### Environment Setup
- Download container (via internet or USB)  
- Launch PX4 SITL + Gazebo  
- First simulation run with QGroundControl  
- Walk through PX4 docs and setup guides  
- Explain uXRCE bridge and message list  
- Adding custom messages  
### Control Pipelines
- Sensor fusion example with PX4 + ROS 2  
- Two integration methodologies:  
  - ROS 2 Offboard mode  
  - PX4 ROS 2 Interface Library  
- Writing and swapping in ROS 2 control logic  
- Live walkthroughs: single and multi-drone control  
- Common pitfalls (timing, buffering) and solutions  
### Perception & Application
- Build perception pipeline: ArUco marker landing + LiDAR mapping  
- Vision-guided precision landing demo  
- Swarm coordination examples  
- Architecture diagrams + code snippets
### Q&A, Resources & Hardware Show-and-Tell
- Open discussion and troubleshooting tips  
- Key resources + links  
- Demo table: flight controllers, companion computers, sensors 
### Open Lab & Individual Consultations
- One-on-one help  
- Guided next-steps planning  
### External Resources