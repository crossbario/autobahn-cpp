#!/bin/bash

CROSSBAR_PATH=${1}/crossbar
TEST_BINARY=${2}
cd "${CROSSBAR_PATH}" || exit 1
/opt/homebrew/bin/crossbar start --cbdir .crossbar > /dev/null 2>&1 &
PID=$!
sleep 3
${TEST_BINARY}
RET=$?
kill ${PID}
exit ${RET}
