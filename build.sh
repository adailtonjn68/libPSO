#!/bin/bash


CC="gcc"
WARNINGS="-Wall -Wextra -Wpedantic"
STANDARD="-std=c2x"
OPTIM="-O3"
DEBUG="-g3"
C_FLAGS="${WARNINGS} ${STANDARD} ${OPTIM} ${DEBUG}"

TEST_DIR=./Tests
BUILD_DIR=./Build

if [[ $1 == "clear" ]]; then
    rm -f ${BUILD_DIR}/*
elif [[ $1 == "compile" ]]; then
    ${CC} ${C_FLAGS} pso.c ${TEST_DIR}/test01.c -o ${BUILD_DIR}/test01
elif [[ $1 == "run" ]]; then
    {BUILD_DIR}/test01
else
    echo "Pass one of the following arguments"
    echo -e "\tclear\n\tcompile\n\ttests\n\texamples"

fi

exit
