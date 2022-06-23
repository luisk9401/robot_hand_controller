#include <systemc.h>
#include "../src/memory_ddr.cpp"

sc_signal<sc_uint<32> > data; 
sc_signal<sc_uint<32> > address;
sc_signal<sc_uint<32> > benable;
sc_signal<sc_uint<32> > state; 
ram mem("MEM");
   

void init_ram() {
  mem.update_command(NOP);
  sc_start(10,SC_NS);
  mem.update_command(NOP);
  sc_start(20,SC_NS);
  mem.update_command(ZQCL);
  sc_start(20,SC_NS);
  mem.update_command(NOP);
  sc_start(20,SC_NS);

}
void write()
{
  mem.update_command(ACT);
  sc_start(20,SC_NS);
  mem.update_command(NOP);
  sc_start(20,SC_NS);
  mem.update_command(WR);
  sc_start(20,SC_NS);
  mem.update_command(PRE);
  sc_start(20,SC_NS);
  mem.update_command(NOP);
  sc_start(20,SC_NS);
}
void read()
{
  mem.update_command(ACT);
  sc_start(20,SC_NS);
  mem.update_command(NOP);
  sc_start(20,SC_NS);
  mem.update_command(RD);
  sc_start(20,SC_NS);
  mem.update_command(PRE);
  sc_start(20,SC_NS);
  mem.update_command(NOP);
  sc_start(20,SC_NS);
}
int sc_main (int argc, char* argv[]) {
  mem.address(address);
  mem.data(data);
  mem.benable(benable); 
   
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("memory");
  wf->set_time_unit(1, SC_NS);
  
  // Dump the desired signals
  sc_trace(wf, data, "data");
  sc_trace(wf, address, "address");
  sc_trace(wf, mem.mem[0x0], "data0");
  sc_trace(wf, mem.mem[0x1], "data1"); 
  init_ram(); 
  sc_start(0,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;
  printf("WR: addr = 0x10, data = 0xaced\n");
  address=0x0;
  data=0xaced;
  benable = 0xFFFFFFFF;
  write();
  address=0x1;
  read();
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation

 }
