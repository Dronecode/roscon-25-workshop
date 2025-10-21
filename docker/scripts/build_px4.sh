#!/usr/bin/env bash
set -eo pipefail

PX4_VERSION=$1

git clone  --recurse-submodules -b ${PX4_VERSION} https://github.com/PX4/PX4-Autopilot.git /home/${USER}/PX4-Autopilot

cd /home/${USER}/PX4-Autopilot

# Patch to allow setting parameters via env variables
git apply - <<'EOF'
diff --git a/ROMFS/px4fmu_common/init.d-posix/rcS b/ROMFS/px4fmu_common/init.d-posix/rcS
index fed292b9d1..1d34854077 100644
--- a/ROMFS/px4fmu_common/init.d-posix/rcS
+++ b/ROMFS/px4fmu_common/init.d-posix/rcS
@@ -126,15 +126,6 @@ then
 	set AUTOCNF yes
 fi
 
-# Allow overriding parameters via env variables: export PX4_PARAM_{name}={value}
-env | while IFS='=' read -r line; do
-  value=${line#*=}
-  name=${line%%=*}
-  case $name in
-    "PX4_PARAM_"*) param set "${name#PX4_PARAM_}" "$value" ;;
-  esac
-done
-
 # multi-instance setup
 # shellcheck disable=SC2154
 param set MAV_SYS_ID $((px4_instance+1))
@@ -239,6 +230,15 @@ then
 	exit 1
 fi
 
+# Allow overriding parameters via env variables: export PX4_PARAM_{name}={value}
+env | while IFS='=' read -r line; do
+  value=${line#*=}
+  name=${line%%=*}
+  case $name in
+    "PX4_PARAM_"*) param set "${name#PX4_PARAM_}" "$value" ;;
+  esac
+done
+
 dataman start
 
 # only start the simulator if not in replay mode, as both control the lockstep time
EOF

./Tools/setup/ubuntu.sh --no-nuttx --no-sim-tools
make px4_sitl_default
cd ..
mkdir -p px4_sitl/bin
mkdir -p px4_sitl/romfs/etc
cp -a PX4-Autopilot/build/px4_sitl_default/bin px4_sitl/
tar xf PX4-Autopilot/build/px4_sitl_default/romfs_files.tar -C px4_sitl/romfs/etc