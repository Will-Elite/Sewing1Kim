#include <Arduino.h>
#include <Wire.h>
#include <ModbusMaster.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

// The I2C address of the OLED display
#define OLED_REST -1  // Set -1 if not using the reset pin

// Initialize the OLED display
Adafruit_SH1106 display(OLED_REST);

ModbusMaster node;

int input_status = 0;
int status_machine = 0;
int input_Stop = 0;

// Button 
int button_Yellow = 27;
int buttonStatus_Yellow = 0;

int button_Green = 29;
int buttonStatus_Green = 0;
int lastbuttonState_Green =0;

int button_Red = 31;
int buttonStatus_Red = 0;
int Status_button_Red = 0;
int lastbuttonState_red = 0;
int buttonPushCounter_red = 0;

// Biến Number of meters of fabric
int Meters_fabric = 0;
int SetupMeters_fabric = 0;
int buttonPushCounter_Yellow_Green_fabric = 0;
int buttonPushCounter_Yellow_Green_LC = 0;

int Meters_fabric_A = 0;
int SetupMeters_D200 = 0;

int buttonGreenPressingTime = 0; //Thời gian đọc nút nhấn Green
unsigned long buttonGreenPressingTimeLast = 0;

long debounceDelayGreen2 = 20000; //Thời gian cho giản 2 đơn vị
long debounceDelayGreen3 = 40000; //Thời gian cho giản 3 đơn vị

int result;
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  node.begin (1, Serial1);

  // Start up the OLED display
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  delay(100); // Wait a moment for the display to boot up

  //Display startup information
  display.display(); // call this mothod for the changes to make effect
  delay(2000);
  // Clear the screen
  display.clearDisplay(); // all pixels are off

  //button 
  pinMode (button_Red, INPUT_PULLUP);
  pinMode (button_Green, INPUT_PULLUP);
  pinMode (button_Yellow, INPUT_PULLUP);
}

void Read_value_M5()
{  
  int address_M5 = 5;
  result = node.readCoils(address_M5, 1);
  if(result == node.ku8MBSuccess)
  {
    //Serial.println("M" + String(address_M5) + "value:"+ String(node.getResponseBuffer(0)));
    String input_status_str = String (node.getResponseBuffer(0));
    input_status = input_status_str.toInt();   
  }
  else
  {
    //Serial.println("M"+ String(address_M5) + "Read Error");
  }
}

//Read Button Stop
void Read_value_M30()
{
  int address_M30 = 30;
  result = node.readCoils(address_M30, 1);
  //Serial.println("M" + String(address_M30) + "value:"+ String(node.getResponseBuffer(0)));
  String input_status_str_M30 = String (node.getResponseBuffer(0));
  input_Stop = input_status_str_M30.toInt();

  if (input_Stop == 1)
  {
  Meters_fabric_A = Meters_fabric;
  }
}

 //Read met value
void Read_Vadlue_D54 ()
{
  int address_Met = 54; // Địa chỉ vủng nhớ D plc
  result = node.readHoldingRegisters(address_Met, 1);
  if (result == node.ku8MBSuccess)
  {
    Meters_fabric = node.getResponseBuffer(0);
    //Serial.println("Met:" + String(Meters_fabric));
  }
  else
  {
    //Serial.println ("Meters Read Error");
  }
}

//Read setup meters fabric
void Read_value_metters_setup()
{
  int address_metters_setup = 58;
  result = node.readHoldingRegisters(address_metters_setup,1);
  if (result == node.ku8MBSuccess)
  {
    SetupMeters_fabric = node.getResponseBuffer(0);
    //Serial.println("Setup meters:" + String(SetupMeters_fabric));
  }
  else 
  {
    //Serial.println ("Meters setup Read Error");
  }
}

//Read D200 set fabric
void Read_value_metters_setup_D200()
{
  int address_D200 = 200;
  result = node.readHoldingRegisters(address_D200,1);
  if (result == node.ku8MBSuccess)
  {
    SetupMeters_D200 = node.getResponseBuffer(0);
    //Serial.println("Setup meters:" + String(SetupMeters_D200));
  }
  else 
  {
    //Serial.println ("Meters setup Read Error");
  }
}

//Read D210 set Collar


// write set fabric 
void Write_value_metters_setup_D200()
{
  int address_Write = 200; // Địa chỉ vùng nhớ D plc
    int value_to_write = buttonPushCounter_Yellow_Green_fabric;
    result = node.writeSingleRegister(address_Write, value_to_write);
    if (result == node.ku8MBSuccess)
    {
      //Serial.println("Write to D200 successful");
    }
    else
    {
      //Serial.println("Write to D200 failed");
    }
    delay(500);
}

// Button
void Button_Red ()
{
  buttonStatus_Red = digitalRead(button_Red);
  //Serial.println(buttonStatus_Red);
  if (buttonStatus_Red != lastbuttonState_red)
  {
    delay(10);
    if (buttonStatus_Red == 0)
    {
      buttonPushCounter_red++;
      //Serial.println(buttonStatus_Red);
      //Serial.println("Da nhan Red");
      //while (digitalRead(button_Red) == 0);
    }    
  }
  lastbuttonState_red = buttonStatus_Red;
  //Serial.println(buttonPushCounter_red);
}

void Button_Yellow ()
{
  buttonStatus_Yellow = digitalRead(button_Yellow);
  //Serial.println(buttonStatus_Yellow);
  if (buttonStatus_Yellow == 0)
  {
    delay(10);
    if(buttonStatus_Yellow == 0)
    {
      
      buttonPushCounter_Yellow_Green_fabric++;
      //Serial.println(buttonStatus_Yellow);
      //Serial.println("da nhan Yellow");
    }
  }
}
void Button_Green ()
{
  buttonStatus_Green = digitalRead(button_Green);
  //Serial.println(buttonStatus_Green);
  
  if (buttonStatus_Green == 0)
  {
    delay(10);
    buttonGreenPressingTime = millis();
    if(buttonStatus_Green == 0)
    {
      unsigned long a = buttonGreenPressingTime - buttonGreenPressingTimeLast;
      Serial.println(a);
      if(debounceDelayGreen1 > a);
      {
        buttonPushCounter_Yellow_Green_fabric--;
      }

      //Serial.println(buttonStatus_Green);
      //Serial.println("da nhan Green");
      
    }
  }
  if (buttonStatus_Green != lastbuttonState_Green)
  {
    buttonGreenPressingTimeLast = buttonGreenPressingTime;
    lastbuttonState_Green = buttonStatus_Green;
  }
}

void loop() {
  // Read coil value
  Read_value_M5();

  Read_value_M30();

  //Read met value
  Read_Vadlue_D54 ();
  
  //Read setup meters fabric
  Read_value_metters_setup();

  //Read D200
  Read_value_metters_setup_D200();
 
  // write set fabric 
  if (buttonPushCounter_red == 2)
  {
    Write_value_metters_setup_D200();
  }

  //Serial.println("test:" + String(input_status));
    if (input_status == 0)
    {
      status_machine = 0; // Machine stop
    }
    else
    {
      status_machine = 1; // machine Run
    }

// LCD
  if (buttonPushCounter_red == 0) 
  {   
    display.clearDisplay();
    display.setTextColor(WHITE);  
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.println("Meters:");
    display.setCursor(50, 50);
    display.println(Meters_fabric_A);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.println("Machine: ");
    if (status_machine == 1)
    {
      display.setCursor(50,20); // x,y
      display.println("RUN ");
      if (Meters_fabric > 0)
      {
        Meters_fabric_A = Meters_fabric;
      }   
    }
    else 
    {
      display.setCursor(50,20); // x,y
      display.println("STOP");
    }
    buttonPushCounter_Yellow_Green_fabric = SetupMeters_D200;
    delay(300);
  }

  if (buttonPushCounter_red == 1)
  {
    display.clearDisplay();
    display.setTextColor(WHITE);  
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Fabric:");
    display.setCursor(50,0);
    display.println(SetupMeters_fabric);
    display.setCursor(0, 10);
    display.println("->");
    display.setCursor(15, 10);
    display.println("Set Fabric1:");
    display.setCursor(87,10);
    display.println(buttonPushCounter_Yellow_Green_fabric); 
    delay(100);
  }


  //success
  if (buttonPushCounter_red == 2)
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0); 
    display.println("success");
  }

  display.display();
  

// Button
Button_Red (); 
Button_Yellow ();
Button_Green ();  
  
  
  if (buttonPushCounter_red > 2)
  {
    buttonPushCounter_red = 0;
  }
}


  
