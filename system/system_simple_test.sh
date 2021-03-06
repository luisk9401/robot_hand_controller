#!/bin/sh
rm -rf ./usb20_test.o
echo 'Compiling *.c *cpp files'
export SYSTEMC_HOME=/usr/local/systemc-2.3.2/
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib-linux64  system_tb.cpp reader_fifo.cpp ../blocks/utils/router.cpp ../blocks/usb2.0/src/usb20.cpp -lsystemc -lm -o system_test.o
echo 'Simulation Started'
./system_test.o
echo 'Simulation Ended'
