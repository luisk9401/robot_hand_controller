//-----------------------------------------------------
#include "systemc.h"
#include <iostream>
#include <stdio.h>

typedef enum COMMAND { 
    ACT,PRE,PREA,
    RD,WR,REF,RST,
    PDE,PDX,ZQCL,
    SRE,SRX,NOP
}command_t;

typedef enum DDR3_STATES  { 
    POWERON, RESET, INIT,
    ZQCAL, IDLE,SREFRESH,
    REFRESH,ACTIVE,PREPWRDOWN,
    BANKACTIVE,WRITE,READ,
    PRECHARGE
}ddr3_state_t;


SC_MODULE (ram) {
 
  //-----------Inputs/Outputs-------------------
  sc_inout<sc_uint<32> > data; 
  sc_in<sc_uint<32> > address;
  sc_in<sc_uint<32> > benable; 
  //-----------Internal variables-------------------
  sc_uint <32> mem[1024];
  ddr3_state_t CurrentState;
  ddr3_state_t NextState;
  command_t command;
  sc_event update_t,wr_t,rd_t;
  const char* state_name[int(PRECHARGE)+1] = {"POWERON", "RESET", "INIT",
        "ZQCAL", "IDLE","SREFRESH",
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
  void update_command(command_t cmd) {
    command = cmd;
    update_t.notify(2,SC_NS);

  }
  
  void process_command() {
    printf("CurrentState value is: %d\n",CurrentState);
    while (true) {
        wait(update_t);
        switch(CurrentState) {
           case POWERON: 
               NextState = RESET;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case RESET:
               NextState = INIT;
               break;
           case INIT:
               if (command == ZQCL) {
                   NextState = ZQCAL;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break;
           case ZQCAL:
               NextState = IDLE;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case IDLE:
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
                   NextState = IDLE;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break; 
           case SREFRESH:
               if (command == SRX) {
                   NextState = IDLE;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break;
           case REFRESH:
               NextState = IDLE;
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
                if (command == PRE || command == PREA ) {
                   NextState = PRECHARGE;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                   wr_t.notify(2,SC_NS);
                }
                break;
           case READ:
                if (command == PRE || command == PREA ) {
                   NextState = PRECHARGE;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
                   rd_t.notify(2,SC_NS);
                }
                break;
           case PRECHARGE:
               NextState = IDLE;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;

        }
        if (command == RST) {
            CurrentState = RESET;
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
