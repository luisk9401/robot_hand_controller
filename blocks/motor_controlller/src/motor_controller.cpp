//-----------------------------------------------------
#include "systemc.h"
#include "../../utils/Initiator.cpp"
typedef enum FINGERS_N { 
    LITTLE,
    RING,
    MIDDLE,
    INDEX,
    THUMB
}finger_name_t;  
    
typedef enum MOTORLOC 
{
    UP,
    DOWN,
    LEFT,
} motorloc_t;
typedef enum MOTOR_STATE {
  INIT,
  IDLE,
  UPDATE_RQ,
  PWM_CFG,
  ACK_M,
  RESET,
} motor_state_t;

typedef enum COMMAND_M{ 
   UPD,
   NOP, 
   RST,
} command_t; 
SC_MODULE (MotorController) {
 
  //-----------Inputs/Outputs-------------------
  uint32_t data; 
  uint32_t address;
  sc_bit ack;
  sc_bit update;

  //-----------Internal variables-------------------
  motor_state_t CurrentState;
  motor_state_t NextState;
  command_t command;
  Initiator *motor_initiator_socket;
  sc_event update_t, update_pwm_t;
  motorloc_t finger_status[5] = {UP,UP,UP,UP,UP};
  const char* state_name[int(RESET)+1] = {
        "INIT",
        "IDLE",
        "UPDATE_RC",
        "PWM_CFG",
        "ACK_M", 
        "RESET", 
  };
 
  // Constructor for memory
  SC_CTOR(MotorController) {
    CurrentState = INIT;
    NextState = INIT;
    motor_initiator_socket = new Initiator("motor_initiator_socket");
    SC_THREAD(process_command);
    SC_THREAD(update_pwm_val);

  } // End of Constructor

   //------------Code Starts Here-------------------------
  void update_command(command_t cmd) {
    command = cmd;
    update_t.notify(2,SC_NS);

  }
  void update_pwm_val() { 
    while (true) {
        wait(update_pwm_t);
        switch((int)address) {
            case LITTLE:
                if (data == 0xFFFFFFFF) {
                    finger_status[LITTLE]= DOWN;
                }
                else if (data == 0xDDDDDDDD){ 
                    finger_status[LITTLE]= LEFT;
                }
                else {
                    finger_status[LITTLE]= UP;
                }
                break;
            case RING:
                if (data == 0xFFFFFFFF) {
                    finger_status[RING]= DOWN;
                }
                else if (data == 0xDDDDDDDD){ 
                    finger_status[RING]= LEFT;
                }
                else {
                    finger_status[RING]= UP;
                }
                break;
            case MIDDLE:
                if (data == 0xFFFFFFFF) {
                    finger_status[MIDDLE]= DOWN;
                }
                else if (data == 0xDDDDDDDD){ 
                    finger_status[MIDDLE]= LEFT;
                }
                else {
                    finger_status[MIDDLE]= UP;
                }
                break;
            case INDEX:
                if (data == 0xFFFFFFFF) {
                    finger_status[INDEX]= DOWN;
                }
                else if (data == 0xDDDDDDDD){ 
                    finger_status[INDEX]= LEFT;
                }
                else {
                    finger_status[INDEX]= UP;
                }
                break;
            case THUMB:
                if (data == 0xFFFFFFFF) {
                    finger_status[THUMB]= DOWN;
                }
                else if (data == 0xDDDDDDDD){ 
                    
                    finger_status[THUMB]= LEFT;
                    printf("Finger Status THUMB: %d\n",(int)finger_status[THUMB]);

                }
                else {
                    finger_status[THUMB]= UP;
                }
                break;
        }
    }
  }

  void process_command() {
    printf("CurrentState value is: %d\n",CurrentState);
    while (true) {
        wait(update_t);
        switch(CurrentState) {
           case INIT: 
               NextState = RESET;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case RESET:
               NextState = IDLE;
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case IDLE:
               if (command == UPD) {
                   NextState = PWM_CFG;
                   cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               }
               break;
           case PWM_CFG: 
               NextState = ACK_M;
               update_pwm_t.notify(2,SC_NS);
               cout <<"Transitioning to NextState:"<<state_name[(int)NextState]<<endl;
               break;
           case ACK_M:
               NextState = IDLE;
               ack = 0b1;
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
