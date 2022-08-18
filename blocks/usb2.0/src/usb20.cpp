//-----------------------------------------------------
#include "systemc.h"
#include "../../utils/Initiator.cpp"
//Constants 
#define  BUFFER_SIZE    1026
#define  FRAME_SIZE 1024

//Enumtype
typedef enum PID_TYPE {
    Token     = 0b0001,
    Data      = 0b0010,
    Handshake = 0b0011,
    Special   = 0b0100,
}pid_type_t ;
typedef enum TokenName {
    OUT   = 0b0001,
    IN    = 0b1001,
    SOF   = 0b0101,
    EoF   = 0b1101,
}token_name_t;
typedef enum DataName {
    DATA0 = 0b0011,
    DATA1 = 0b1011,
    DATA2 = 0b0111,
    MDATA = 0b1111,
}data_name_t;

typedef enum HandshakeName { 
    ACK   = 0b0010,
    NAK   = 0b1010,
    STALL = 0b1110,
    NYET  = 0b0110,
}handshake_name_t;
typedef enum SpecialName {
    PRE      = 0b0010,
    ERR      = 0b1100,
    SPLIT    = 0b1000,
    PING     = 0b0100,
    Reserved = 0b0000,
} special_name_t;
typedef enum USBSates {
    HC_HS_IDLE,
    HC_HS_PACKET_START,
    HC_HS_PACKET_COLLECT, 
    HC_HS_PACKET_END,
}usb_states_t;

SC_MODULE (usb20) {
    
  //-----------Internal variables-------------------
  usb_states_t CurrentSate;
  usb_states_t NextState;

  uint8_t *data;
  sc_event update_data_t;
  Initiator *usb_initiator_socket;
  uint8_t *data_out;
  // Constructor  
  SC_CTOR(usb20) {
    //Data that will go to the ram tlm eventually on project 2 
    data_out     = new uint8_t [BUFFER_SIZE-1];
    usb_initiator_socket = new Initiator("usb_initiator_socket");
    CurrentSate  = HC_HS_IDLE;
    NextState    = HC_HS_IDLE; 
    SC_THREAD(read);
  } // End of Constructor

  void update_data_s(uint8_t data_r[BUFFER_SIZE]) {
    data = &data_r[0];
    update_data_t.notify();

  }
   //------------Code Starts Here-------------------------
  void read() {
    while(true) {
        printf("CurrentSate value is: %d\n",CurrentSate);
        wait(update_data_t);
        switch(CurrentSate) { 
            case HC_HS_IDLE: 
                if (*data == (uint8_t)Token) {
                    NextState = HC_HS_PACKET_START;
                    printf("Transitioning to NextState: %d\n",NextState);
                }
                break;
            case HC_HS_PACKET_START:
                if (*(data+1) == (uint8_t)SOF) {
                    NextState = HC_HS_PACKET_COLLECT;
                    printf("Transitioning to NextState: %d\n",NextState);
                }
                break;
            case HC_HS_PACKET_COLLECT: 
                if (*(data+1) == (uint8_t)EoF) {
                    NextState = HC_HS_PACKET_END;
                    printf("Transitioning to NextState: %d\n",NextState);
                }
                for (int i=2; i <= 1023; i++) {
                    //data_ram_out.write(*(data+i));
                    usb_initiator_socket->write(*data,1);
                    wait(1, sc_core::SC_NS);  
                }
                break;
            case  HC_HS_PACKET_END:
                NextState = HC_HS_IDLE;
                printf("Transitioning to NextState: %d\n",NextState);
                data_out[0] = (uint8_t)Handshake;
                data_out[1] = (uint8_t)ACK;
                break;
        }
        CurrentSate = NextState;
    }
  }  
  

}; // End of Module memory
