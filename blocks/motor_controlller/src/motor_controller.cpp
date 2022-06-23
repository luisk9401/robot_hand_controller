//-----------------------------------------------------
#include "systemc.h"

typedef enum FINGERS_N { 
    LITTLE,
    RING,
    MIDDLE,
    INDEX,
    Thumb
}finger_name_t;  
    

typedef enum MOTOR_STATE {
  INIT,
  IDLE,
  UPDATE_RC,
  PWM_CFG,
  ACK,
} motor_state_t;

typedef enum COMMAND { 
   POWERON,
   UPD,
   NOP, 
} command_t; 
SC_MODULE (MotorController) {
 
  //-----------Inputs/Outputs-------------------
  sc_inout<sc_uint<32> > data; 
  sc_out<sc_uint<32> > address;
  sc_out<sc_bit > ack;
  sc_out<sc_bit > update;
  //-----------Internal variables-------------------
  motor_state_t CurrentState;
  motor_state_t NextState;
  command_t command;
  sc_event update_t;
  const char* state_name[int(PRECHARGE)+1] = {
        "INIT",
        "IDLE",
        "UPDATE_RC",
        "PWM_CFG",
        "ACK",  
  };
 
  // Constructor for memory
  //SC_CTOR(ram) {
  SC_HAS_PROCESS(MotorController) {
    CurrentState = INIT;
    NextState = INIT;
    SC_THREAD(process_command);
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
  
  
}; // End of Module memory
