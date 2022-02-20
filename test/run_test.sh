#!/bin/bash

CROSSBAR_PATH=${1}/crossbar
TEST_BINARY=${2}
cd "${CROSSBAR_PATH}" || exit 1
docker run -d -v $PWD:/node -u 0 --rm --name=crossbar -it -p 8080:8080 crossbario/crossbar
sleep 15
${TEST_BINARY}
RET=$?
docker container stop crossbar
exit ${RET}
