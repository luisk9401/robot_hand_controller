#!/bin/sh
rm -rf ./motor_control_test.o
echo 'Compiling *.c *cpp files'
export SYSTEMC_HOME=/usr/local/systemc-2.3.2/
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib-linux64   ../src/motor_controller.cpp  ../tb/motor_control_tb.cpp -lsystemc -lm -o motor_control_test.o
echo 'Simulation Started'
./motor_control_test.o
echo 'Simulation Ended'
