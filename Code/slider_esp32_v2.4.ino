

// Runs on ESP32 Arduino. 
// To Do: Add WiFi controls


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AccelStepper.h" 

//Endstops
#define endstopMotor 26
#define endstopElectronics 16

//Stepper  
#define STEPPER_ENABLE 23
#define MS1 19
#define MS2 18
#define MS3 5
#define DIR 17
#define STEP 13

AccelStepper stepper(1, STEP, DIR); 

//Debug 
#define BAUD_RATE (9600)

//Display
#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 32 // OLED display height, use 64 or 32
#define OLED_RESET LED_BUILTIN 
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Encoder
#define encButton 25 
#define outA 33 
#define outB 32 

int outA_lastState,outA_currentState,counter=0;
int selection=1;
int numCount=1;
int encoderState;
int encoderLastState;  

volatile byte seqA = 0;
volatile byte seqB = 0;
volatile byte cnt1 = 0;
volatile byte cnt2 = 0;
volatile boolean right = false;
volatile boolean left = false;
volatile boolean encButtonStatus = false;


void IntCallback();
void EncButtonStatusInt();
void MainMenu(int selection);
void RealTime();
void FixedTime();
void Flowers();
void People();
void Review();
void Quick();
void TimeSlide(float tim);
void ManualTimeLapse();
void SlideRealTime(int def);
void TimeLapse();
void SetTimerTimeLapse();
void MainMenuFixedTime(int selection);
void MainMenu(int selection);


void setup() 
{
  Serial.begin(115200);
  Serial.println("Tinker Slider V1.0!!");

  //Status
  pinMode(LED_BUILTIN,OUTPUT);

  //Stepper Driver
  pinMode(STEP,OUTPUT); 
  pinMode(DIR,OUTPUT); 
  pinMode(STEPPER_ENABLE,OUTPUT);
  pinMode(MS1,OUTPUT);
  pinMode(MS2,OUTPUT);
  pinMode(MS3,OUTPUT);
  
  digitalWrite(STEPPER_ENABLE,LOW);
  
  // Set in 1/16th microstep mode
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH); 

  
  //Encoder
  pinMode(outA,INPUT_PULLUP);
  pinMode(outB,INPUT_PULLUP);
  pinMode(encButton,INPUT_PULLUP);
  digitalWrite(outA,HIGH);
  digitalWrite(outB,HIGH);
  digitalWrite(encButton,HIGH);

  //Interrupt Enable
  attachInterrupt(digitalPinToInterrupt(outA), IntCallback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(outB), IntCallback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encButton), EncButtonStatusInt, CHANGE);
  
  //Endstop
  pinMode(endstopElectronics,INPUT_PULLUP);
  pinMode(endstopMotor,INPUT_PULLUP);
  digitalWrite(endstopElectronics,HIGH);
  digitalWrite(endstopMotor,HIGH);    
  
  //Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  
  //Display the Tink splashscreen based on Adafruit GFX library
  display.display();  
  delay(1000);         
}

void loop() 
{ 
  MainMenu(ScrollSelection(3));  
}


void MainMenu(int selection)//main menu
{
  encButtonStatus=true;
  int numItems=3;

  Serial.println("Menu Select");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Tinker Slider v1.0");
  display.drawLine(0,15-1,127,15-1,WHITE);
  display.drawLine(0,16-1,127,16-1,WHITE);
  
  String MainMenu[] = {"1. RealTime","2. TimeLapse","3. FixedTime"};
  DisplayMenuSelection(numItems,MainMenu);
 
  
  if(encButtonStatus == false)
  {
    Serial.println("encButtonStatus Pressed");
    delay(500); // time delay to ensure encButtonStatus is released
    digitalWrite(encButton,HIGH);
    encButtonStatus=true;//set encButtonStatus state to false, otherwise it will remain true. it is toggle.
    switch(selection)
    {
      Serial.print("selection:");
      Serial.println(selection);
      case 1:
        RealTime();
        break;
      case 2:
        TimeLapse();  
        break;
      case 3:
        FixedTime();
        break;
      default:
        Serial.println("No such Menu");
        break;
    }      
  }
  
}


void MainMenuFixedTime(int selection)//main menu
{
  encButtonStatus=true;
  int numItems=4;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Fixed Time Mode");
  display.drawLine(0,15-1,127,15-1,WHITE);
  display.drawLine(0,16-1,127,16-1,WHITE);
  
  String MainMenu[] = {"1. Flowers ","2. People","3. Review","4. Quick!"};
  DisplayMenuSelection(numItems,MainMenu);

  //if the encoder encButtonStatus is pressed, then selection is made (through interrupts)
  if(encButtonStatus == false)
  {
    Serial.println("Button Pressed");
    delay(500); // time delay to ensure encButtonStatus is released
    digitalWrite(encButton,HIGH);
    encButtonStatus=true;  
    switch(selection)
    {
      Serial.print("selection:");
      Serial.println(selection);
      case 1:
        Flowers();    // Contains the tried and tested sliding time to capture flowers
        break;
      case 2:
        People();     // Contains the tried and tested sliding time to capture people
        break;
      case 3:
        Review();     // Contains the tried and tested sliding time to review products (Accelerated)
        break;
      case 4:
        Quick();      // A 5 sec quick slide!
        break;
      default:
        Serial.println("No such Menu");
        break;
    }      
  }
  
}



void SetTimerTimeLapse()
{
  
    int hr=0,mins=0,sec=0;
    char hrs[3],minutes[3],seconds[3];
    int timePlaceHolder = 0; 
    float timeVal = 0;
    
    // Prepare the display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    
    display.println("Set the Timer!");
    display.setCursor(0,20);
    display.println("H : M : S");
    
    display.setTextColor(WHITE,BLACK);
    display.setCursor(0,36);

    //Display the time placeholders
    display.print(hr);
    display.print("   ");
    display.print(mins);
    display.print("   ");
    display.print(sec);

    display.setTextColor(BLACK,WHITE);
    display.setCursor(0,36);
    display.print("0");

    //Update the display
    display.display();
    
    encButtonStatus =true;
    counter = 0;
    encoderLastState = digitalRead(outA);

    while(1)
    {    
       encoderState = digitalRead(outA); // Reads current state of the outA
       if(encoderState != encoderLastState)
       { 
        if(timePlaceHolder==0)//hr
        {
        
         // clockwise if the outB state is different to the outA state
         if (digitalRead(outB) != encoderState)  hr++;
         else hr--;
         
         if(hr <=0) hr=0;  

         Serial.print("hr: ");
         Serial.println(hr);

         //Print the change on the display 
         sprintf((char*)hrs,"%-2d",hr);
         display.setTextColor(BLACK,WHITE);
         display.setCursor(0,36);
         display.print(hrs);
         
       }          

       if(timePlaceHolder==1)//Min
       {
         // clockwise if the outB state is different to the outA state
         if (digitalRead(outB) != encoderState) mins++;
         else mins--;
         
         if (mins <=0) mins =0;
                    
         Serial.print("mins: ");
         Serial.println(mins);
         
         //Print the change on the display 
         sprintf((char*)minutes,"%-2d",mins);
         display.setTextColor(BLACK,WHITE);
         display.setCursor(22,36);
         display.print(minutes);
       }

       
       if(timePlaceHolder==2)//Sec
       {
         // clockwise if the outB state is different to the outA state
         if (digitalRead(outB) != encoderState)  sec++;
         else  sec--;
         if (sec <= 0) sec=0;
         
         Serial.print("sec: ");
         Serial.println(sec);

         //Print the change on the display 
         sprintf((char*)seconds,"%-2d",sec);
         display.setTextColor(BLACK,WHITE);
         display.setCursor(44,36);
         display.print(seconds);
        }
        //Update the display
        display.display();    
       }    
       

       //Confirm the selection if the encoder encButtonStatus is pressed
       if(encButtonStatus== false)  
       {
        counter=0;
        delay(500); //Debounce

        //If hour Set, then highlight seconds placeholder 
        if(timePlaceHolder==0)
        {
          display.setTextColor(WHITE,BLACK);
          display.setCursor(0,36);
          display.print(hrs);

          display.setTextColor(BLACK,WHITE);
          display.setCursor(22,36);
          display.print("0");

        
        }
        //If mins Set, then highlight seconds placeholder
        else if(timePlaceHolder==1)
        {
         
          display.setTextColor(WHITE,BLACK);
          display.setCursor(22,36);
          display.print(minutes);

          display.setTextColor(BLACK,WHITE);
          display.setCursor(44,36);
          display.print("0");
        }

        // Time set
        else if(timePlaceHolder==2)
        {
          
          display.setTextColor(BLACK,WHITE);
          display.setCursor(44,36);
          display.print(seconds);
          display.display();    
          break;
        }
          
        timePlaceHolder++; 
       
        encoderState &= ~encoderState;    // Trigger the selection for the next option        
        encButtonStatus =true;
        digitalWrite(encButton,HIGH);
        display.display();                     
       }       
      }                       
      
      Serial.println("Time set");
      Serial.print("Hr:");
      Serial.print(hr);
      Serial.print("Mins");
      Serial.print(mins);
      Serial.print("Secs");
      Serial.print(sec);
      Serial.println(); 
        

      timeVal = sec + (mins * 60) + (hr*60*60); // calculate the time in secs
      Serial.println("Time in Seconds:");
      Serial.println(timeVal);

      // Bring the cam plate to home (Electronics Side endstop)
      homePosition();
      
      // Display the choosen Time in Seconds   
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Timer Set!");
      display.drawLine(0,15-1,127,15-1,WHITE);
      display.drawLine(0,16-1,127,16-1,WHITE);
      display.setCursor(0,30);
      display.println("You have choosen");
      display.setCursor(12,40);
      display.print(timeVal);
      display.setCursor(68,40);
      display.print("Seconds");
      display.display();
    
      // Start the sliding for the timeVal seconds
      TimeSlide(timeVal);
}


// TimeLapse menu
void TimeLapse()
{
  selection =1;
  while(1)
  {  
    int selection = ScrollSelection(2);
    encButtonStatus=true;
    int numItems=2;
    
    Serial.println("Menu Select");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("TimeLapse Modes");
    display.drawLine(0,15-1,127,15-1,WHITE);
    display.drawLine(0,16-1,127,16-1,WHITE);

    
    String MainMenu[] = {"1. Manual Adjust","2. Set Timer"};
    DisplayMenuSelection(numItems,MainMenu);
   
    
    if(encButtonStatus == false)
    {
      Serial.println("Button Pressed");
      delay(500); // debounce
      digitalWrite(encButton,HIGH);
      encButtonStatus=true;
      switch(selection)
      {
        Serial.print("selection:");
        Serial.println(selection);
        case 1:
          ManualTimeLapse();
          break;
        case 2:
          Serial.println("Set Timer");
          SetTimerTimeLapse();
          break;
      }      
    }
  }
}

// Manually adjust the time based on the encoder value
void ManualTimeLapse()
{
  Serial.println("In TimeLapse");
  
  homePosition();
  SlideRealTime(1);
  
}

// Used by Realtime slider 
void SlideRealTime(int def)
{  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Rotate the Knob Now!");
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,40);
  display.println("   Push to go Back!");
  display.display();
  delay(500);
  
  encoderLastState = digitalRead(outA); 
  stepper.enableOutputs(); 
  stepper.setMaxSpeed(4000);
  
  while(1)
  {
     encoderState = digitalRead(outA); 
     if (encoderState != encoderLastState)
     {     
       if (digitalRead(outB) != encoderState) 
         counter --;
       else 
         counter ++;
       Serial.print("Position: ");
       Serial.println(counter);
     } 
    
     
     stepper.setSpeed(def*counter);
     stepper.runSpeed(); 

     delayMicroseconds(1000);
           
     if(checkButtonStates() != 0)
      break;

      encoderLastState = encoderState; 
 } 
}


void Flowers()
{
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH); 
   
  homePosition();
  
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,LOW);
  
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
  stepper.moveTo(11200);  // Lets move upto 280mm which is required to complete the full slide
  
  while (stepper.currentPosition() != 11200) 
  {
     stepper.run();
     delayMicroseconds(1000);
     if(checkButtonStates() != 0)
      break;
  }
}

void People()
{
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);
  homePosition();

  // Display Time in Seconds   
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("People Mode!");
  display.drawLine(0,15-1,127,15-1,WHITE);
  display.drawLine(0,16-1,127,16-1,WHITE);

  display.setCursor(0,30);
  display.println("Sliding Time");
  display.setCursor(28,40);
  display.print("10 Seconds");
  display.display();

  TimeSlide(10);
}

void Review()
{
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);
  
  homePosition();
  
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,LOW);
  
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(500);
  stepper.moveTo(11200);
  while (stepper.currentPosition() != 11200) 
  {
    stepper.run();
     
    if(checkButtonStates() != 0)
      break;
   
  }
  
}

void Quick()
{
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);
  homePosition();
   
  TimeSlide(5);
}

void FixedTime()
{  
    selection=1;
    display.clearDisplay();
    Serial.println("In FixedTime");
    while(1)
      MainMenuFixedTime(ScrollSelection(4));     
}

void RealTime()
{
  Serial.println("In RealTime");
  homePosition();
   
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);
  SlideRealTime(100);
}


void TimeSlide(float tim)
{
  float timeDelay=0;
  int microMode = 0;
  encButtonStatus=true;
  Serial.println("tim:");
  Serial.println(tim);
  
 
  // Anything greater than 23 seconds can have  1/16th microstepping 
  if(tim >=23)
  {
    Serial.println(">23");
    digitalWrite(MS1,HIGH);
    digitalWrite(MS2,HIGH);
    digitalWrite(MS3,HIGH);
    timeDelay = (tim*1000) /22521;
    microMode = 3200;
  }
  
  // 1/8th microstepping 
  if(tim < 23 && tim > 11)
  {    
    Serial.println("<23 >11");
    digitalWrite(MS1,HIGH);
    digitalWrite(MS2,HIGH);
    digitalWrite(MS3,LOW);
    timeDelay = (tim*1000) /11263;
    microMode = 1600;
  }

  //1/4th microstepping
  if(tim <=10)
  {
    Serial.println("<10");    
    digitalWrite(MS1,LOW);
    digitalWrite(MS2,HIGH);
    digitalWrite(MS3,LOW);
    timeDelay = (tim*1000) /5630;
    microMode = 800;
  }
  
  stepper.enableOutputs(); 
  stepper.setMaxSpeed(4000);
  stepper.setSpeed(4000);
  
  Serial.print("timeDelay");
  Serial.println(timeDelay);
  Serial.print("Micromode");
  Serial.println(microMode);
  int lastTime = millis();


  for(int i = 0; i < microMode *7; i++)
  {
     stepper.runSpeed(); 
     delayMicroseconds(1000*timeDelay);
    
     if(checkButtonStates() != 0)
      break;
     
  }
  Serial.println(millis() -lastTime);

}


// Interrupts
void IntCallback()
{
    // Read A and B signals
   
    boolean A_val = digitalRead(outA);
    boolean B_val = digitalRead(outB);

    // Record the A and B signals in seperate sequences
    seqA <<= 1;
    seqA |= A_val;
    
    seqB <<= 1;
    seqB |= B_val;
    
    // Mask the MSB four bits
    seqA &= 0b00001111;
    seqB &= 0b00001111;
    
    // Compare the recorded sequence with the expected sequence
    if (seqA == 0b00001001 && seqB == 0b00000011) {
      cnt1++;
      left = true;
      //Serial.print("L cnt1:");
      //Serial.println(cnt1);
      }
     
    if (seqA == 0b00000011 && seqB == 0b00001001) {
      cnt2++;
      right = true;
      //Serial.print("L cnt2:");
      //Serial.println(cnt2);
      }  
}

void EncButtonStatusInt()
{
  if(!digitalRead(encButton))  
  {
    encButtonStatus   = false;
    Serial.println("Button Status Changed");
  }
}    

int checkButtonStates()
{  
   if(digitalRead(endstopElectronics) == false)
   {  
    Serial.println("Electronics Endstop Triggered");
    stepper.setMaxSpeed(2000);
    stepper.setSpeed(500);      
    for(int k = 0 ;k <300;k++)
    {
      stepper.runSpeed();
      delay(2);
    }
    counter=0;
    return -1;
   }  
   
   if(digitalRead(endstopMotor) == false)
   {  
    Serial.println("Motor Endstop Triggered");
    stepper.setMaxSpeed(2000);
    stepper.setSpeed(-500);      
    for(int k = 0 ;k <300;k++)
    {
      stepper.runSpeed(); 
      delay(2);
    }
    counter=0;
    return -1;
   }  

   if(encButtonStatus== false)  
   {
    Serial.println("Button Pressed");
    encButtonStatus =true;
    digitalWrite(encButton,HIGH);
    return -1;     
   }
   return 0;
}


int ScrollSelection(int numRows) 
{
  if (left) 
  {
    left = false;
    selection--;
    if (selection==0) 
      selection=numRows;      
  }
  if (right) 
  {
    right = false;
    selection++;
    if (selection==numRows+1) 
      selection=1;      
  }  
  return selection;
}

//homes the system to the limit switch
void homePosition()
{
  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.println("Home position...");
  display.display(); 
  stepper.enableOutputs();
  stepper.setMaxSpeed(-4000);

  while(1)
  {
    if(digitalRead(endstopElectronics) == false)
      break; 
    stepper.setSpeed(-1000);      
    stepper.runSpeed();
    delayMicroseconds(1000);
  }
  stepper.setMaxSpeed(2000);
  stepper.setSpeed(500);      
  for(int k = 0 ;k <300;k++)
  {
    stepper.runSpeed();
    delay(2);
  }
}


//creates menu on the oled display
void DisplayMenuSelection(int numItems,String menu[])
{
  display.setTextSize(1);
  
  for(int i=1;i<numItems+1;i++)
  {
    display.setCursor(0, 10+i*10);    
    
    if (i==4 && selection==5 || selection == i)
      display.setTextColor(BLACK,WHITE);
    else 
      display.setTextColor(WHITE, BLACK);
    
    display.print(menu[i-1]);    
  }
  
  display.display(); 
}
