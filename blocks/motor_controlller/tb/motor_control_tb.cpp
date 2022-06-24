#include <systemc.h>
#include "../src/motor_controller.cpp"



int sc_main (int argc, char* argv[]) {
  MotorController motor("MOTORCONT");
   
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("motor");
  wf->set_time_unit(1, SC_NS);
  
  // Dump the desired signals
  sc_trace(wf, motor.data, "data");
  sc_trace(wf, motor.address, "address");
  sc_trace(wf, (int)motor.finger_status[5], "THUMB");
  sc_trace(wf, motor.ack, "ACK");

  sc_start(0,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;
  
  motor.update_command(NOP);
  sc_start(5,SC_NS); 
  motor.update_command(NOP);
  sc_start(5,SC_NS); 
  motor.update_command(UPD);
  sc_start(5,SC_NS);
  motor.data = 0xDDDDDDDD; 
  motor.address = 0x4;
  motor.update_command(NOP);
  sc_start(5,SC_NS); 
  motor.update_command(NOP);
  sc_start(5,SC_NS);
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation

 }
