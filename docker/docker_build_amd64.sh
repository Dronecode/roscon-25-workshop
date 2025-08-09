SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

docker build -t px4/roscon-25-workshop -f ${SCRIPTPATH}/Dockerfile  ${SCRIPTPATH}/..