

#define Baud_rate 9600  
char Char;
bool Switch_present = true; // process initiation flag, no micro-switch = false
#define Micro_switch 4
// uln stepper mortor controller inputs from microcontroller 
/*
   Connect your motor controller pins IN1..IN4 to the following Arduino pins.
   The Arduino "talks" directly to controller pins IN1..IN4 via PORTB.
*/
#define TRIG_1_PIN 5
#define TRIG_2_PIN 6
#define ECO_2_PIN  7
#define ECO_1_PIN  12
#define IN4  8 
#define IN3  9
#define IN2  10
#define IN1  11
byte Motor[8] =                  //half-stepping
{ B00001000,
  B00001100,
  B00000100,
  B00000110,
  B00000010,
  B00000011,
  B00000001,
  B00001001
};
int Index = 0;                  //Motor[] array index
int Step_counter = 0;           //360 degrees requires 4096 steps
unsigned long Delay = 2;        //give motor shaft time to move
byte Pattern;                   // variable to change the pattern
unsigned long Speed_of_rotation = 30; // setting speed
int Direction = 0;              // initially seting the direction of rotation of the stepper mortor in anti clock wise direction
float az1 = 0;                  // making a float variable to tell the angle of the object
int az =0;                      // azimuth value initially set to 0
float az2=0;


class Trigger 
{
  private: 
    byte pin;
  public:
    Trigger(byte pin)
    { this-> pin=pin;
    initialize();
    }
    void initialize()
    { pinMode(pin,OUTPUT);
    off();
    delayMicroseconds(2);
    }
    void on()
    {digitalWrite(pin,HIGH);
    }
    void off()
    {digitalWrite(pin, LOW);
    }
 };


 class Ecco { 
  private:
     byte pin;
     long duration, cm;
  public: 
    Ecco( byte pin)
      {this-> pin=pin;
       determine();
       calculate_display();
      }
   void determine()
   {  pinMode(pin,INPUT);
      duration = pulseIn(pin, HIGH);
   }
   void calculate_display()
   {
    cm= duration/29/2;
   Serial.print(cm);
   Serial.print("cm");
   Serial.println();
   delay(100);
   }

 };


Trigger trigger1(TRIG_1_PIN);
Trigger trigger2(TRIG_2_PIN);
Ecco ecco1(ECO_1_PIN);
Ecco ecco2(ECO_2_PIN);


void setup()
{
  Serial.begin(Baud_rate);
  pinMode(Micro_switch, INPUT_PULLUP);  // rotating the stepper motor to a initial position
  
  Pattern = DDRB;                       // get PORTB data directions, ie IN 1, 2, 3, 4
  Pattern = Pattern | B00001111;        // preserve MSN data direction &
  DDRB = Pattern; 
  
 if (Switch_present)
  {
    home();                             //to rotate the motor once to initialize the position of the sensor 
  }
connect_to_display();   
}


    
void connect_to_display()                   // function initiation to display the plot
{
  while (Serial.available() <= 0)
  {
    // ----- keep sending synch ('S') until the display responds
    Serial.println("S");
    delay(250);
  }
}

void home()
{
  // ----- rotate clockwise until limit switch operates
  Step_counter = 2048 ;
  while (digitalRead(Micro_switch))
  {
    Index = Step_counter % 8;                 //calculate array index
    Pattern = PORTB;                          //get current motor pattern
    Pattern = Pattern & B11110000;            //preserve MSN
    Pattern = Pattern | Motor[Index];         //create new motor pattern
    PORTB = Pattern;                          //send new pattern to motor
    Step_counter--;
    delay(Delay);                             //controls motor speed (fastest=1)
  }

  // ----- back off slightly
  /*
     Keep clear of limit switch during normal scans
  */
  Step_counter = 0;
  for (int i = 0; i < 256; i++)
  {
    Index = Step_counter % 8;                 //calculate array index
    Pattern = PORTB;                          //get current motor pattern
    Pattern = Pattern & B11110000;            //preserve MSN
    Pattern = Pattern | Motor[Index];         //create new motor pattern
    PORTB = Pattern;                          //send new pattern to motor
    Step_counter++;
    delay(Delay);                             //controls motor speed (fastest=1)
  }

  Step_counter = 0;
}

void loop()
{
  if (Serial.available() > 0)
  {
    Char = Serial.read();               // read character

    // ----- send data to display whenever a send character ('S') is received
    if (Char == 'S')
    {
      trigger1.on();
      delayMicroseconds(10);
      trigger1.off();
      ecco1.determine();
      ecco1.calculate_display();
          Serial.print("angle 1");
          Serial.print(az1);
      trigger2.on();
      delayMicroseconds(10);
      trigger2.off();
      ecco2.determine();
      ecco2.calculate_display();
         Serial.print("angle 2");
         Serial.print(az2);
    rotate();
  
    delay(Speed_of_rotation); 
   }
  }
  
}   
void rotate()
     {
  // ----- counter-clockwise scan
  if (Direction == 0)
  {for (int az=0; az<256;az++)                  //increment Azimuth(az) every 8 steps
   {
    for (int i = 0; i < 8; i++)
    {
      // ----- rotate motor to next ping position
      Index = Step_counter % 8;                 //calculate array index
      Pattern = PORTB;                          //get current motor pattern
      Pattern = Pattern & B11110000;            //preserve MSN
      Pattern = Pattern | Motor[Index];         //create new motor pattern
      PORTB = Pattern;                          //send new pattern to motor
      Step_counter++;
      az1=az/1.422;                             // angle calculation
      az2=az1-180;
      delay(Delay);                             //controls motor speed (fastest=1)
    }
    }
    
      Direction = 1;                            // switching the swing 
      Step_counter = 2048;                      // 180 degree turn
  }
  else
  {
    // ----- clockwise scan
    for (int az=256; az>0;az--)                  //decrement Azimuth(az) every 8 steps
    {
     for (int i = 0; i < 8; i++)
     {
      // ----- rotate motor to next ping position
      Index = Step_counter % 8;                 //calculate array index
      Pattern = PORTB;                          //get current motor pattern
      Pattern = Pattern & B11110000;            //preserve MSN, clearing current value
      Pattern = Pattern | Motor[Index];         //create new motor pattern, putting the required pattern
      PORTB = Pattern;                          //send new pattern to motor
      Step_counter--;
      az1=az/1.422;
      az2=az1-180;
      delay(Delay);                             //controls motor speed (fastest=1)
     }
    }
  
      Direction = 0;                            // direction of swing reversed
      Step_counter = 0;                         // step reinitiated to zero
  }
  
  
}      
   
