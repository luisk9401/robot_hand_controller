#!/bin/sh
echo 'Compiling *.c *cpp files'
rm -rf Audio_receiver.o
export SYSTEMC_HOME=/usr/local/systemc-2.3.3_c++11/
export LD_LIBRARY_PATH=/usr/local/systemc-ams-2.1/lib-linux64/:$SYSTEMC_HOME/lib-linux64/
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib-linux64 -I/home/kenneth/systemcams/systemc-ams-2.3/include -L/home/kenneth/systemcams/systemc-ams-2.3/lib-linux64 sc_main.cpp -std=c++11 -lsystemc -lsystemc-ams -lm -o audio_receiver.o
echo 'Simulation Started'
./audio_receiver.o
echo 'Simulation Ended'
