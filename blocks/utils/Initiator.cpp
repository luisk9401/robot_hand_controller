#ifndef INITIATOR_H_
#define INITIATOR_H_ 

#include <systemc.h>   
using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "ExtendId.cpp"

struct Initiator: sc_module   
{   
  // TLM2 socket, defaults to 32-bits wide, generic payload, generic DMI mode   
  tlm_utils::simple_initiator_socket<Initiator> socket; 
  sc_event send_request_t, read_t,finish_t; 
  uint8_t data;
  uint64 address;
  tlm::tlm_generic_payload trans;
  ID_extension* id_extension = new ID_extension;
  tlm::tlm_command cmd;
  SC_CTOR(Initiator)   
  : socket("socket")  // Construct and name socket   
  {   
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_bw(this, &Initiator::nb_transport_bw);
    
    SC_THREAD(thread_process); 
  }   
  void write(uint8_t data_to_send, uint64 addr)  {
      data    = data_to_send;
      address = addr;
      cmd     = tlm::TLM_WRITE_COMMAND;
      send_request_t.notify();
  }
  uint8_t read(uint64 addr) {
      address = addr;
      cmd     = tlm::TLM_READ_COMMAND;
      send_request_t.notify(); 
      wait(read_t);
      return data;
  }
  void send_request(uint8_t data_to_send, uint64 addr,tlm::tlm_command cmd_m) {
      data    = data_to_send;
      address = addr;
      cmd     = cmd_m;
      send_request_t.notify();
  }
  void thread_process() {
      while(true) { 
         wait(send_request_t);
         trans.set_extension(id_extension);
         tlm::tlm_phase phase = tlm::BEGIN_REQ;   
         sc_time delay = sc_time(10, SC_NS);   
         trans.set_command(cmd);   
         trans.set_address(address);   
         trans.set_data_ptr(reinterpret_cast<unsigned char*>(&data));   
         trans.set_data_length(4);
         tlm::tlm_sync_enum status;  
         //cout << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
         status = socket->nb_transport_fw( trans, phase, delay );
         switch (status)   {
            case tlm::TLM_ACCEPTED:   
                //Delay for END_REQ
                wait( sc_time(10, SC_NS) );
                //cout << name() << " END_REQ SENT" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
                // Expect response on the backward path  
                phase = tlm::END_REQ; 
                status = socket->nb_transport_fw(trans, phase, delay);  // Non-blocking transport call
                break;   
            case tlm::TLM_UPDATED:   
            case tlm::TLM_COMPLETED:   
                // Initiator obliged to check response status   
                if (trans.is_response_error() )   
                    SC_REPORT_ERROR("TLM2", "Response error from nb_transport_fw");   
                //cout << "trans/fw = { " << (cmd ? 'W' : 'R') << ", " << hex << " } , data = "   
                //<< hex << data << " at time " << sc_time_stamp() << ", delay = " << delay << endl;   
                break;   
         }
         id_extension->transaction_id++;
        }
    }
   
  // *********************************************   
  // TLM2 backward path non-blocking transport method   
  // *********************************************   
   
  virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,   
                                           tlm::tlm_phase& phase, sc_time& delay )   
  {   
    tlm::tlm_command cmd = trans.get_command();   
    sc_dt::uint64    adr = trans.get_address();   
    
    ID_extension* id_extension = new ID_extension;
    trans.get_extension( id_extension ); 
    
    if (phase == tlm::END_RESP) {  
           
      //Delay for TLM_COMPLETE
      wait(delay);
      
      cout << name() << " END_RESP RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
            
      return tlm::TLM_COMPLETED;
        
    }
    
    if (phase == tlm::BEGIN_RESP) {
                              
      // Initiator obliged to check response status   
      if (trans.is_response_error() )   
        SC_REPORT_ERROR("TLM2", "Response error from nb_transport");   
            
      cout <<name() << "trans/bw = { " << (cmd ? 'W' : 'R') << ", " << hex << adr   
           << " } , data = " << hex << data << " at time " << sc_time_stamp()   
           << ", delay = " << delay << endl;
      
      //Delay
      wait(delay);
      read_t.notify();
      cout << name () << " BEGIN_RESP RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
      finish_t.notify();
      return tlm::TLM_ACCEPTED;   
    }   
  }   
};   

#endif
