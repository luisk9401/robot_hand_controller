#include <systemc.h>
#include "../blocks/utils/routerv2.cpp"
#include "../blocks/usb2.0/src/usb20.cpp"
#include "../blocks/riscv/src/riscv.cpp"
#include "../blocks/motor_controlller/src/motor_controller.cpp"
#include "../blocks/ddram/src/ddram.cpp" 
#include <iostream>
#include <string>
#define IMAGE_HEADER_SIZE 14
#define IMAGE_INFORMATION_HEADER_SIZE 40
#define MAX_PACKET_COUNT_SUPPORT 1024

//Global Variables
usb20 usb("usb2.0");
MotorController motor("motor");
//cpu riscv("riscv");
Memory    *memory;
uint8_t  * data;

Router r0("r0");
Router r1("r1");
Router r2("r2");
//Router r3("r3");



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
  memory    = new Memory("memory");
  usb.usb_initiator_socket->socket.bind(r0.socket);
  r0.initiator_socket[0]->socket.bind(r1.socket);
  r0.initiator_socket[1]->socket.bind(r2.socket);
  motor.motor_initiator_socket->socket.bind(r1.socket);
  r1.initiator_socket[0]->socket.bind(r2.socket);
  r1.initiator_socket[1]->socket.bind(r2.socket);
  r2.initiator_socket[0]->socket.bind(memory->socket);
  r2.initiator_socket[1]->socket.bind(r0.socket);

  //ddr signal connection

  
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
