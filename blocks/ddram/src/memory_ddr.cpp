//-----------------------------------------------------
#include "systemc.h"

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
    update_t.notify(15,SC_NS);

  }
  
  void process_command() {
    printf("CurrentState value is: %d\n",CurrentState);
    while (true) {
        wait(update_t);
        switch(CurrentState) {
           case POWERON: 
               NextState = RESET;
               printf("Transitioning to NextState: %d\n",NextState);
               //sc_start(5,SC_NS);
               break;
           case RESET:
               NextState = INIT;
               printf("Transitioning to NextState: %d\n",NextState);
               //sc_start(10,SC_NS);
               break;
           case INIT:
               if (command == ZQCL) {
                   NextState = ZQCAL;
                   printf("Transitioning to NextState: %d\n",NextState);
                   //sc_start(3,SC_NS);
               }
               break;
           case ZQCAL:
               NextState = IDLE;
               printf("Transitioning to NextState: %d\n",NextState);
               //sc_start(5,SC_NS);
               break;
           case IDLE:
               switch(command) {
                   case ACT:
                       NextState = ACTIVE;
                       printf("Transitioning to NextState: %d\n",NextState);
                       break;
                   case PDE:
                       NextState = PREPWRDOWN;
                       printf("Transitioning to NextState: %d\n",NextState);
                       break;
                   case SRE:
                       NextState = SREFRESH;
                       printf("Transitioning to NextState: %d\n",NextState);
                       break;
                   case REF:
                       NextState = REFRESH;
                       printf("Transitioning to NextState: %d\n",NextState);
                       break;

               }
               break;
           case ACTIVE: 
               NextState = BANKACTIVE;
               printf("Transitioning to NextState: %d\n",NextState);
               //sc_start(1,SC_NS);
               break;
           case PREPWRDOWN:
               if (command == PDX) {
                   NextState = IDLE;
                   printf("Transitioning to NextState: %d\n",NextState);
                   //sc_start(1,SC_NS);

               }
               break; 
           case SREFRESH:
               if (command == SRX) {
                   NextState = IDLE;
                   printf("Transitioning to NextState: %d\n",NextState);
                   //sc_start(1,SC_NS);
               }
               break;
           case REFRESH:
               NextState = IDLE;
               printf("Transitioning to NextState: %d\n",NextState);
               //sc_start(1,SC_NS);
               break;
           case BANKACTIVE:
               if (command == WR) {
                   NextState = WRITE;
                   printf("Transitioning to NextState: %d\n",NextState);

               }
               else if(command == RD) {
                   NextState = READ;
                   printf("Transitioning to NextState: %d\n",NextState);
                   //sc_start(1,SC_NS);
               }
               break;
           case WRITE:
                if (command == PRE || command == PREA ) {
                   NextState = PRECHARGE;
                   printf("Transitioning to NextState: %d\n",NextState);
                   wr_t.notify(2,SC_NS);
                }
                break;
           case READ:
                if (command == PRE || command == PREA ) {
                   NextState = PRECHARGE;
                   printf("Transitioning to NextState: %d\n",NextState);
                   rd_t.notify(2,SC_NS);
                }
                break;
           case PRECHARGE:
               NextState = IDLE;
               printf("Transitioning to NextState: %d\n",NextState);
               //sc_start(2,SC_NS);
               break;

        }
    }
    if (command == RST) {
        CurrentState = RESET;
    }
    else {
        CurrentState = NextState;
    }
  }
 
  

 void wr() {
    while(true) {
      wait(wr_t);
      mem [address.read()] = data.read() && benable.read();
    }  
  }

 void rd() {
    while(true) {
      wait(rd_t);
      data = mem [address.read()] && benable.read();
    }  
  }
  
  
}; // End of Module memory
