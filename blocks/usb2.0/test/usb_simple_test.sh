#!/bin/sh
rm -rf ./usb20_test.o
echo 'Compiling *.c *cpp files'
export SYSTEMC_HOME=/usr/local/systemc-2.3.2/
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib-linux64  ../tb/usb20_tb.cpp ../src/usb20.cpp -lsystemc -lm -o usb20_test.o
echo 'Simulation Started'
./usb20_test.o
echo 'Simulation Ended'
