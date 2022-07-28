#include<systemc>

SC_MODULE(cpu) {
  // sc_fifo_in_if is a abstract class from which you cannot create an object.
  // sc_fifo_in is derived from the above class which actually implements the read method.
  sc_core::sc_fifo_in<uint8_t> cpu_in;
  sc_core::sc_fifo_out<uint8_t> cpu_out;  

  void roperation()
  {

    uint8_t val;
    while (true)
    {
       // Time unit is defined in sc_core namespace.
        cpu_in.read(val);
        printf("reader: value is: %d\n",val);
        wait(1, sc_core::SC_NS);
        // Or you can use the statement after including all the header files:
        // using namespace std
    }
    //< when using the systemc header you need to specify the std namespace.
    // Or you can use the statement after including all the header files:
    // using namespace std

  }

  SC_CTOR(cpu) //< Your code mentions here writer the constructor name should be same as the class name.
  {
    SC_THREAD(roperation); //< Your code mentions woperation

  }

};
