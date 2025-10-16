# Prerequisites installation

## System setup on WSL2

On W11 WSL2 you can either install Docker desktop on W11 or docker engine directly on WSL, these instructions follow the second approach.

1. **Install Ubuntu 24.04 on WSL2**. Follow the [official instructions](https://learn.microsoft.com/en-us/windows/wsl/install) to install Ubuntu 24.04 on W11 WSL2.

    ```sh
    wsl.exe --install Ubuntu-24.04
    ```

2. **Install docker engine on Ubuntu-24.04**. Docker engine installation instructions are the same for WSL 2 and native Ubuntu. Please got to the instructions for Ubuntu host.

## System setup on Ubuntu

1. **Install docker engine on Ubuntu-24.04**.

    1. Install using the `apt` repository

        ```sh
        # Add Docker's official GPG key:
        sudo apt-get update
        sudo apt-get install ca-certificates curl
        sudo install -m 0755 -d /etc/apt/keyrings
        sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
        sudo chmod a+r /etc/apt/keyrings/docker.asc

        # Add the repository to Apt sources:
        echo \
        "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
        $(. /etc/os-release && echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}") stable" | \
        sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
        sudo apt-get update
        sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
        ```

    2. Verify the installation

        ```sh
        sudo docker run hello-world
        ```

    3. Run docker as non-root user

        ```sh
        sudo usermod -aG docker $USER
        ```

    4. Verify that you can run docker commands without sudo

        ```sh
        docker run hello-world
        ```

    For further information regarding docker installation, please refer to the [official documentation](https://docs.docker.com/engine/install/ubuntu/). For the security risks involved in in running docker as non-root user please refer to [Manage Docker as a non-root user](https://docs.docker.com/engine/install/linux-postinstall/#manage-docker-as-a-non-root-user).

2. **Install QGroundControl**. QGC can either run on the Host and connect to PX4 running in the container or it can directly run in the container. If you plan to run your container in headless mode, then you will need to use the first approach.

    - **Install QGC on the host system**. Please follow the [official instructions](https://docs.qgroundcontrol.com/master/en/qgc-user-guide/getting_started/download_and_install.html#ubuntu) to install QGC on Ubuntu.

        1. Enable serial-port access Add your user to the dialout group so you can talk to USB devices without root. Please note that this is not strictly necessary as QGC will only use UDP connection for the workshop.

            ```sh
            sudo usermod -aG dialout "$(id -un)"
            ```

        2. (Optional) Disable ModemManager On some Ubuntu-based systems, ModemManager can claim serial ports that QGC needs. If you don't use it elsewhere, mask or remove it.

            ```sh
            # preferred: stop and mask the service
            sudo systemctl mask --now ModemManager.service

            # or, if you’d rather remove the package
            sudo apt remove --purge modemmanager
            ```

        3. On the command prompt, enter:

            ```sh
            sudo apt install gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-gl -y
            sudo apt install libfuse2 -y
            sudo apt install libxcb-xinerama0 libxkbcommon-x11-0 libxcb-cursor-dev -y
            ```

        4. Now you can download [QGroundControl-x86_64.AppImage](https://d176tv9ibo4jno.cloudfront.net/latest/QGroundControl-x86_64.AppImage).
        5. Make the AppImage executable

            ```sh
            chmod +x QGroundControl-x86_64.AppImage
            ```

    - **Configure the host to run GQC inside the container**. No extra steps are required.