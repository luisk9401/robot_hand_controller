#include <systemc.h>   
using namespace sc_core;   
using namespace sc_dt;   
using namespace std;   
   
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "tlm_utils/multi_passthrough_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "Initiator.cpp"



struct Router: sc_module   
{   
  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::multi_passthrough_target_socket<Router> socket;
  
  enum { SIZE = 256 };   
  const sc_time LATENCY;   
  sc_event  e1;
  tlm::tlm_generic_payload* trans_pending;   
  tlm::tlm_phase phase_pending;   
  sc_time delay_pending; 
  Initiator *initiator_socket[2];
  
  SC_CTOR(Router)   
  : socket("socket"), LATENCY(10, SC_NS)   
  {   
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_fw(  this, &Router::nb_transport_fw);
    char name[20];

    for (unsigned int i = 0; i < 2; i++) {
        sprintf(name,"initiator_%d",i);
        initiator_socket[i] = new Initiator(name);
    }
    SC_THREAD(thread_process);   
  }   
   
  // TLM2 non-blocking transport method 
  
  virtual tlm::tlm_sync_enum nb_transport_fw( int id, tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
    sc_dt::uint64    adr = trans.get_address();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();

    ID_extension* id_extension = new ID_extension;
    trans.get_extension( id_extension ); 
    
    if(phase == tlm::END_REQ){
      
      wait(delay);
      
      //cout << name() << " END_REQ RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;
      
      return tlm::TLM_COMPLETED;
    }
    if(phase == tlm::BEGIN_REQ){
      // Obliged to check the transaction attributes for unsupported features
      // and to generate the appropriate error response
      if (byt != 0) {
        trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
        return tlm::TLM_COMPLETED;
      }
      //if (len > 4 || wid < len) {
      //  trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
      //  return tlm::TLM_COMPLETED;
      //}

      // Now queue the transaction until the annotated time has elapsed
      trans_pending=&trans;
      phase_pending=phase;
      delay_pending=delay;

      e1.notify();
      
      //Delay
      wait(delay);
      
      //cout << name() << " BEGIN_REQ RECEIVED" << " TRANS ID " << id_extension->transaction_id << " at time " << sc_time_stamp() << endl;      
      
      return tlm::TLM_ACCEPTED;
    }  
  }
  
  // *********************************************   
  // Thread to call nb_transport on backward path   
  // ********************************************* 
   
  void thread_process()  
  {   
    while (true) {
    
      // Wait for an event to pop out of the back end of the queue   
      wait(e1); 
      //printf("ACCESING MEMORY\n");
   
      //tlm::tlm_generic_payload* trans_ptr;   
      tlm::tlm_phase phase;   
      
      ID_extension* id_extension = new ID_extension;
      trans_pending->get_extension( id_extension ); 
      
      sc_dt::uint64 adr = trans_pending->get_address();
      tlm::tlm_command cmd = trans_pending->get_command();   
      unsigned char*   ptr = trans_pending->get_data_ptr();   
      unsigned int     len = trans_pending->get_data_length();   
      switch (adr) {
        case 1:
            initiator_socket[0]->send_request(*ptr,adr,cmd);
            wait(initiator_socket[0]->finish_t);
      	    break;
        case 2:
            initiator_socket[0]->send_request(*ptr,adr,cmd);
            wait(initiator_socket[1]->finish_t);
      	    break;
      } 
      // Obliged to set response status to indicate successful completion   
      trans_pending->set_response_status( tlm::TLM_OK_RESPONSE );  
      
      wait( sc_time(20, SC_NS) );
      
      delay_pending= sc_time(10, SC_NS);
      //cout << name() << " BEGIN_RESP SENT" << " TRANS ID " << id_extension->transaction_id <<  " at time " << sc_time_stamp() << endl;
      
      // Call on backward path to complete the transaction
      tlm::tlm_sync_enum status;
        phase = tlm::BEGIN_RESP;   
      status = socket->nb_transport_bw( *trans_pending, phase, delay_pending );   
   
        // The target gets a final chance to read or update the transaction object at this point.   
        // Once this process yields, the target must assume that the transaction object   
        // will be deleted by the initiator   
   
      // Check value returned from nb_transport   
   
      switch (status)   
        
      //case tlm::TLM_REJECTED:   
        case tlm::TLM_ACCEPTED:   
          
          wait( sc_time(10, SC_NS) );
          
          //cout << name() << " END_RESP SENT" << " TRANS ID " << id_extension->transaction_id <<  " at time " << sc_time_stamp() << endl;
          // Expect response on the backward path  
          phase = tlm::END_RESP; 
          socket->nb_transport_bw( *trans_pending, phase, delay_pending );  // Non-blocking transport call
        //break;   
        
    }   
  } 
}; 
