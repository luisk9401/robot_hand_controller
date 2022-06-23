#include <systemc.h>
#include "../src/memory_ddr.cpp"



void init_ram() {

}
int sc_main (int argc, char* argv[]) {
  
  sc_signal<sc_uint<32> > data; 
  sc_signal<sc_uint<32> > address;
  sc_signal<sc_uint<32> > benable;
  
  ram mem("MEM");
  mem.address(address);
  mem.data(data);
  mem.benable(benable);      
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("memory");
  wf->set_time_unit(1, SC_NS);
  
  // Dump the desired signals
  sc_trace(wf, data, "data");
  sc_trace(wf, address, "address");
 
   
  sc_start(0,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;
  printf("WR: addr = 0x10, data = 0xaced\n");
  mem.update_command(NOP);
  mem.update_command(ZQCL);
  sc_start(20,SC_NS);

  address=0x10;
  data=0xaced;
  benable = 0xFF;
  sc_start(20,SC_NS);
 
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation

 }
