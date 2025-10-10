#!/usr/bin/env bash
set -eo pipefail

QGC_VERSION=$1

if [ "${TARGETARCH}" = "amd64" ]; then
    cd /home/${USER}
    wget https://github.com/mavlink/qgroundcontrol/releases/download/${QGC_VERSION}/QGroundControl-x86_64.AppImage
    chmod +x ./QGroundControl-x86_64.AppImage
    ./QGroundControl-x86_64.AppImage --appimage-extract
    mv /home/${USER}/squashfs-root /home/${USER}/QGroundControl
    chmod +x /home/${USER}/QGroundControl/AppRun
    ln -s /home/${USER}/QGroundControl/AppRun /home/${USER}/QGroundControl/qgroundcontrol
else
    mkdir -p /home/${USER}/QGroundControl
    echo "QGroundControl is only available for amd64 architecture." >> /home/${USER}/QGroundControl/install.log
fi