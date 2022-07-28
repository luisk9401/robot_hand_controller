#include <systemc.h>
#include "../blocks/usb2.0/src/usb20.cpp"
#include "../blocks/riscv/src/riscv.cpp"
#include "../blocks/utils/router.cpp"
#include "../blocks/motor_controlller/src/motor_controller.cpp"
#include "../blocks/ddram/src/memory_ddr.cpp" 
#include <iostream>
#include <string>
#define IMAGE_HEADER_SIZE 14
#define IMAGE_INFORMATION_HEADER_SIZE 40
#define MAX_PACKET_COUNT_SUPPORT 1024

//Global Variables
usb20 usb("usb2.0");
MotorController motor("motor");
cpu riscv("riscv");
ram ddr3("ddr3");
uint8_t  * data;
sc_fifo<uint8_t> usb_fifo_in;
sc_fifo<uint8_t> usb_fifo_out;
sc_fifo<uint8_t> motor_fifo_in;
sc_fifo<uint8_t> motor_fifo_out;
sc_fifo<uint8_t> ddr3_fifo_in;
sc_fifo<uint8_t> ddr3_fifo_out;
sc_fifo<uint8_t> cpu_fifo_in;
sc_fifo<uint8_t> cpu_fifo_out;

Router r0("r0");
Router r1("r1");
Router r2("r2");
Router r3("r3");

sc_signal<sc_uint<8> > data_ram; 
sc_signal<sc_uint<64> > address_ram;
sc_signal<sc_uint<32> > benable;
sc_signal<sc_uint<32> > state;

void read_image(std::string path) 
{
  ifstream f;
  f.open(path,std::ios::in | std::ios::binary);
  unsigned char fileHeader [IMAGE_HEADER_SIZE];
  unsigned char information_header[IMAGE_INFORMATION_HEADER_SIZE];
  f.read((char*)fileHeader,IMAGE_HEADER_SIZE);
  f.read((char*)information_header,IMAGE_INFORMATION_HEADER_SIZE);
  int fileSize = fileHeader[2] + (fileHeader[3] << 8) + ( fileHeader[4] << 16) + (fileHeader[5] << 24);
  int m_width = information_header[4] + (information_header[5] << 8) + (information_header[6] << 16) + (information_header[7] << 24);
  int m_height = information_header[8] + (information_header[9] << 8) + (information_header[10] << 16) + (information_header[11] << 24);
  const int paddingAmount = ((4 - (m_width*3)%4)%4);
  int packet_count = 0;
  for (int x=0; x < m_width; x++) {
      for (int y=0; y < m_height; y++) {
        if(packet_count + 4 >=  MAX_PACKET_COUNT_SUPPORT) { 
            packet_count = 0;
            usb.update_data_s(data);
            sc_dt::uint64 address = 1;
            r0.initiator->trigger_message(address);
            sc_start(5,SC_NS);
        }
        else {
            unsigned char color[3];
            f.read((char*)color,3);
            data[packet_count+2] = (uint8_t)color[0];
            data[packet_count+3] = (uint8_t)color[1];
            data[packet_count+4] = (uint8_t)color[2];
            packet_count+=5;
        }
      }
  }
  f.close();
}

int sc_main (int argc, char* argv[]) {
  //usb connection  
  r0.initiator->socket.bind(r1.socketR->socket);
  usb.data_ram_out(usb_fifo_in);
  r0.initiator->fifo_in(usb_fifo_in);
  r0.socketR->fifo_out(usb_fifo_out);
  r0.socketR->next_initiator = r1.initiator;
  usb.data_read_in(usb_fifo_out);
  //cpu connection
  r1.initiator->socket.bind(r2.socketR->socket);
  riscv.cpu_out(cpu_fifo_in);
  r1.initiator->fifo_in(cpu_fifo_in);
  r1.socketR->fifo_out(cpu_fifo_out);
  r1.socketR->next_initiator = r2.initiator;
  riscv.cpu_in(cpu_fifo_out);
  //ram connection
  r2.initiator->socket.bind(r3.socketR->socket);
  ddr3.dram_out(ddr3_fifo_in);
  r2.initiator->fifo_in(ddr3_fifo_in);
  r2.socketR->fifo_out(ddr3_fifo_out);
  r2.socketR->next_initiator = r3.initiator;
  ddr3.dram_in(ddr3_fifo_out); 
  //motor connection
  r3.initiator->socket.bind(r0.socketR->socket);
  motor.motor_out(motor_fifo_in);
  r3.initiator->fifo_in(motor_fifo_in);
  r3.socketR->fifo_out(motor_fifo_out);
  r3.socketR->next_initiator = r0.initiator;
  motor.motor_in(motor_fifo_out); 
  //ddr signal connection
  ddr3.data(data_ram);
  ddr3.address(address_ram);
  ddr3.benable(benable);
  
  data = new uint8_t[1026];  

  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("usb.vcd");
  wf->set_time_unit(1, SC_NS);
  
  // Dump the desired signals
  //sc_trace(wf,r.in, "fifo_out");
  
  sc_start(5,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;
  
  printf("Sending Packet ID: %d\n",1);
  data[0] = 1;
  usb.update_data_s(data);
  sc_start(5,SC_NS);
  printf("Sending Start of Frame Command\n");
  data[1]= 5;
  usb.update_data_s(data);
  sc_start(5,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;  
  printf("Sending Image through usb bus\n");
  read_image("../blocks/usb2.0/test/FLAG_B24.BMP");
  printf("Sending End of Frame Command\n");
  data[1] = 13;
  usb.update_data_s(data);
  sc_start(5,SC_NS);
  printf("Reading back acknowledge signal\n");
  sc_start(20,SC_NS);
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation
 }
