#!/bin/sh
rm -rf ./memory_ddr_test.o
echo 'Compiling *.c *cpp files'
export SYSTEMC_HOME=/usr/local/systemc-2.3.2/
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib-linux64 ../tb/memory_ddr_tb.cpp  ../src/memory_ddr.cpp -lsystemc -lm -o memory_ddr_test.o
echo 'Simulation Started'
./memory_ddr_test.o
echo 'Simulation Ended'
