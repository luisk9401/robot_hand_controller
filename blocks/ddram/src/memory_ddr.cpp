//-----------------------------------------------------
#include "systemc.h"
#include <iostream>
#include <stdio.h>

typedef enum COMMAND { 
    ACT,PRE_M,PREA,
    RD,WR,REF,RST_M,
    PDE,PDX,ZQCL,
    SRE,SRX,NOP_M
}command_m_t;

typedef enum DDR3_STATES  { 
    POWERON, RESET_M, INIT_M,
    ZQCAL, IDLE_M,SREFRESH,
    REFRESH,ACTIVE,PREPWRDOWN,
    BANKACTIVE,WRITE,READ,
    PRECHARGE
}ddr3_state_t;


SC_MODULE (ram) {
 
  //-----------Inputs/Outputs-------------------
  sc_inout<sc_uint<8> > data; 
  sc_in<sc_uint<64> > address;
  sc_in<sc_uint<32> > benable; 
  //-----------Internal variables-------------------
  sc_uint <32> mem[1024];
  ddr3_state_t CurrentState;
  ddr3_state_t NextState;
  command_m_t command;
  sc_event update_t,wr_t,rd_t;

  sc_fifo_out<uint8_t> dram_out;
  sc_fifo_in<uint8_t>  dram_in;
  const char* state_name[int(PRECHARGE)+1] = {"POWERON", "RESET_M", "INIT_M",
        "ZQCAL", "IDLE_M","SREFRESH",
        "REFRESH","ACTIVE","PREPWRDOWN",
        "BANKACTIVE","WRITE","READ",
        "PRECHARGE"
    };
 
  // Constructor for memory
  //SC_CTOR(ram) {
  SC_HAS_PROCESS(ram);
    ram(sc_module_name ram) {
    CurrentState = POWERON;
    NextState = POWERON;
    SC_THREAD(process_command);
    SC_THREAD(wr);
    SC_THREAD(rd);
  } // End of Constructor

   //------------Code Starts Here-------------------------
  void update_command(command_m_t cmd) {
    command = cmd;
    update_t.notify(2,SC_NS);

  }
  
  void process_command() {
    printf("CurrentState value is: %d\n",CurrentState);
    while (true) {
        wait(update_t);
        switch(CurrentState) {
           case POWERON: 
               NextState = RESET_M;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case RESET_M:
               NextState = INIT_M;
               break;
           case INIT_M:
               if (command == ZQCL) {
                   NextState = ZQCAL;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break;
           case ZQCAL:
               NextState = IDLE_M;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case IDLE_M:
               switch(command) {
                   case ACT:
                       NextState = ACTIVE;
                       cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                       break;
                   case PDE:
                       NextState = PREPWRDOWN;
                       cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                       break;
                   case SRE:
                       NextState = SREFRESH;
                       cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                       break;
                   case REF:
                       NextState = REFRESH;
                       cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                       break;

               }
               break;
           case ACTIVE: 
               NextState = BANKACTIVE;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case PREPWRDOWN:
               if (command == PDX) {
                   NextState = IDLE_M;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break; 
           case SREFRESH:
               if (command == SRX) {
                   NextState = IDLE_M;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break;
           case REFRESH:
               NextState = IDLE_M;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case BANKACTIVE:
               if (command == WR) {
                   NextState = WRITE;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               else if(command == RD) {
                   NextState = READ;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break;
           case WRITE:
                if (command == PRE_M || command == PREA ) {
                   NextState = PRECHARGE;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                   wr_t.notify(2,SC_NS);
                }
                break;
           case READ:
                if (command == PRE_M || command == PREA ) {
                   NextState = PRECHARGE;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                   rd_t.notify(2,SC_NS);
                }
                break;
           case PRECHARGE:
               NextState = IDLE_M;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;

        }
        if (command == RST_M) {
            CurrentState = RESET_M;
        }
        else {
            CurrentState = NextState;
        }
    }
  }
 
  

 void wr() {
    while(true) {
      wait(wr_t);
      printf("Writing Data:%d to adress:%d\n",(int)data.read(),(int)address.read());
      mem [address.read()] = data.read();
    }  
  }

 void rd() {
    while(true) {
      wait(rd_t);
      data = mem [address.read()] && benable.read();
      printf("Reading Data:%d to adress:%d\n",(int)data.read(),(int)address.read());

    }  
  }
  
  
}; // End of Module memory
