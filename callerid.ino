static int state=0;
static int o=0;
static int i=0;
static char nam[128];
static char num[25];
static char dat[25];
static int plen=0;
static int nlen=0;
static int dlen=0;
static int mlen=0;
static int len=0;
int SER = 0;
String str = ""; 

#include <SerialCommands.h>


char serial_command_buffer_[32];
SerialCommands serial_commands_(&SerialUSB, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");


void cmd_led_on(SerialCommands* sender)
{
//  digitalWrite(kLedPin, HIGH);  
  sender->GetSerial()->println("{\"pong\":\"1\"}");
}

SerialCommand cmd_led_on_("{\"ping\":\"0\"}", cmd_led_on);

// {"ping":"0"}

void Ht9032_get()
{
  if (Serial1.available() > 0)  
  {

  SER = Serial1.read(); 
    
   /*
    * states
    * 0 - waiting for U's
    * 1 - waiting for 25 U's and then an 0x80
    * 2 - waiting for packet len
    * 3 - waiting for message id
    * 4 - get name len
    * 5 - receiving name
    * 6 - get number len
    * 7 - receiving number
    * 8 - get date len
    * 9 - receiving date
    * 10 - packet complete
    * 
    * message ids
    * 1 - date
    * 2 - number
    * 4 - number not found reason
    * 7 - name
    * 8 - name not found reason
    * 
    * reasons
    * O - Out of area (no caller ID available)
    * P - Private party (Blocked)
    * 
    */
  switch(state)
  {
    case 0:if (SER==0x55)  
               {
                 state=1;
                 i=1;
 //SerialUSB.println("State: 0 ");
               }break;
 
    case 1:if (SER==0x55) 
               {
                 i++;
               }                        
                  
               if(SER==0x80)
               {
                 if(i > 25)
                   {
                      state=2;
                   }
               }
   //         SerialUSB.println("State: 1 ");             
               break;
               
    case 2:plen = SER; 
               i=0;
               state=3;                                        
     /*        SerialUSB.println("State: 2 ");
              SerialUSB.print("Packet Length: ");
              SerialUSB.println(plen);*/
               break;
 
    case 3: o++;
           //SerialUSB.println("State: 3 ");
           //SerialUSB.print("MID: ");
           //SerialUSB.println(SER);
              if(SER==1) 
              {
                state=8;
              }
              else if(SER==2)
              {
                state=6;
              }
              else if(SER==4)
              {
                state=6;
              }
              else if(SER==7)
              {
                state=4;
              }
              else if(SER==8)
              {
                state=4;
              }
              break;
 
    case 4:len = SER;
               nlen = len ; 
               o++;
               i=0;
               state=5;                                        
          //     SerialUSB.println("State: 4 ");
          //     SerialUSB.print("Data Length: ");
          //     SerialUSB.println(nlen);
               break;
 
    case 6:len = SER; 
               mlen = len ; 
               o++;
               i=0;
               state=7;                                        
          //     SerialUSB.println("State: 6 ");
          //     SerialUSB.print("Data Length: ");
         //      SerialUSB.println(mlen);
               break;
 
    case 8:len = SER; 
               dlen = len ; 
               o++;
               i=0;
               state=9;                                        
           //    SerialUSB.println("State: 8 ");
           //    SerialUSB.print("Data Length: ");
           //    SerialUSB.println(dlen);
               break;
 
  
    case 5:nam[i]=SER;  
            i++;
            o++;
              if( i > nlen - 1 ) 
          {  
            state=3;
            i=0;                              
          }

          if( o > plen - 1 ) 
          {  
            state=10;
          }
       //   SerialUSB.println("State: 5 ");
          break;
          
    case 7:num[i]=SER;  
            i++;
            o++;
              if( i > mlen - 1 ) 
          {  
            state=3;
            i=0;                              
          }

          if( o > plen - 1 ) 
          {  
            state=10;
          }
       //   SerialUSB.println("State: 7 ");
          break;
          
    case 9:dat[i]=SER;  
            i++;
            o++;
              if( i > dlen - 1 ) 
          {  
            state=3;
            i=0;                              
          }

          if( o > plen - 1 ) 
          {  
            state=10;
          }
      //    SerialUSB.println("State: 9 ");
          break;
          
     default:break;
   }
  }
}
 


void setup() {
  // put your setup code here, to run once:
SerialUSB.begin(115200);
Serial1.begin(1200);
pinMode(4, INPUT);
pinMode(7, OUTPUT);
    delay(3000);
    SerialUSB.println("{\"FW\":\"1.0a\"}");
    
    serial_commands_.AddCommand(&cmd_led_on_);
}

void loop() {
  // put your main code here, to run repeatedly:
  Ht9032_get();
  serial_commands_.ReadSerial();
  //SerialUSB.print("-");
  digitalWrite(7, digitalRead(4));   // turn the LED on (HIGH is the voltage level)
//  if (digitalRead(4) == HIGH) {
//  SerialUSB.println("RING");
//  }

            if( state == 10 ) 
          {      
            o=0;     
       //     SerialUSB.println("CALL");
            SerialUSB.print("{\"DATE\":\"");
            SerialUSB.print(char(dat[0]));
            SerialUSB.print(char(dat[1]));
            SerialUSB.print('-');
            SerialUSB.print(char(dat[2]));
            SerialUSB.print(char(dat[3]));
            SerialUSB.print("\",\"TIME\": \"");
            SerialUSB.print(char(dat[4]));
            SerialUSB.print(char(dat[5]));
            SerialUSB.print(':');
            SerialUSB.print(char(dat[6]));
            SerialUSB.print(char(dat[7]));
            SerialUSB.print("\",\"NAME\": \"");
            for(int i=0;i<nlen;i++)
             {
               SerialUSB.print(char(nam[i]));
             }
            //SerialUSB.print("");
            SerialUSB.print(",\"NUMBER\": \"");
            for(int i=0;i<mlen;i++)
             {
               SerialUSB.print(char(num[i]));
             }
             SerialUSB.println("\"}");

            state=0; 
          }
}


