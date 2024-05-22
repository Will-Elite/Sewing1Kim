#include <Arduino.h>
#include <Wire.h>
#include <ModbusMaster.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

// The I2C address of the OLED display
#define OLED_REST -1  // Set -1 if not using the reset pin

// Initialize the OLED display
Adafruit_SH1106 display(OLED_REST);

ModbusMaster node; // Slave 2 (plc)
ModbusMaster node1; // Slave 1 (BFC)

int input_status = 0;
int status_machine = 0;
int input_Stop = 0;

// Button 
int button_Yellow = 27;
int buttonStatus_Yellow = 0;
int lastbuttonState_Yellow = 0;

int button_Green = 29;
int buttonStatus_Green = 0;
int lastbuttonState_Green = 0;

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

unsigned long buttonYellowPressingTime = 0; //Thời gian đọc nút nhấn Green
unsigned long buttonYellowPressingTimeLast = 0;
unsigned long debounceDelayYellow1 = 5000; //Thời gian cho giảm 1 đơn vị 5000
unsigned long debounceDelayYellow2 = 10000; //Thời gian cho giản 2 đơn vị 10000
unsigned long debounceDelayYellow3 = 20000; //Thời gian cho giản 3 đơn vị 20000
unsigned long debounceDelayYellow4 = 30000; //Thời gian cho giản 3 đơn vị 30000

unsigned long buttonGreenPressingTime = 0; //Thời gian đọc nút nhấn Green
unsigned long buttonGreenPressingTimeLast = 0;
unsigned long debounceDelayGreen1 = 5000; //Thời gian cho giảm 1 đơn vị 5000
unsigned long debounceDelayGreen2 = 10000; //Thời gian cho giản 2 đơn vị 10000
unsigned long debounceDelayGreen3 = 20000; //Thời gian cho giản 3 đơn vị 20000
unsigned long debounceDelayGreen4 = 30000; //Thời gian cho giản 3 đơn vị 30000

int result;
int SlavePLC = 2;
int SlaveBFC = 1;

int Read_value_BF5R = 0;
int Read_SettingValue_BF5R_CH1 = 0;


void setup() {
  Serial.begin(9600);
  Serial1.begin(38400);
  node.begin (SlavePLC, Serial1);
  node1.begin (SlaveBFC, Serial1);

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

void Read_value_BFC () 
{
 //Khai báo địa chỉ đọc giá trị 
  const int inputRegisterAddress_Read_1 = 0x00C8; //địa chỉ đọc giá trị
  result = node1.readInputRegisters(inputRegisterAddress_Read_1, 1);

  if(result == node1.ku8MBSuccess)
  {
    Read_value_BF5R = node1.getResponseBuffer(0);

    //Serial.print("Value BF5R:");
    //Serial.println(Read_value_BF5R);
  }
  else
  {
    Serial.println("Error reading Input Register");
  }
}

void Read_SettingValue_BF5R()
{
  //Khai báo địa chỉ đọc giá trị
  const int inputRegisterAddress_Read_SV = 0x012C; //địa chỉ đọc giá trị setting value

  result = node1.readInputRegisters(inputRegisterAddress_Read_SV, 1);
  if(result == node1.ku8MBSuccess)
  {
    Read_SettingValue_BF5R_CH1 = node1.getResponseBuffer(0);
    Serial.print("CH1 Setting Value:");
    Serial.println(Read_SettingValue_BF5R_CH1);
  }
  else
  {
    Serial.println("Error reading Input Register Setting Value");
  }
}

void Write_value_BFC()
{
  const int writeRegisterAddress_SettingValue = 0x0000; //địa chỉ thanh ghi vào
  const int valueToWrite = 1133;
  result = node1.writeSingleRegister(writeRegisterAddress_SettingValue, valueToWrite);
  //Kiểm tra xem việc ghi có thành công hay không
  if(result == node1.ku8MBSuccess)
  {
    Serial.print("ghi giá trị thành công");
  }
  else
  {
    Serial.print("Lỗi ghi:");
    Serial.println(result, HEX);
  }
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

 //Read met value BF5R
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
    buttonYellowPressingTime = millis();
    if(buttonStatus_Yellow == 0)
    {
      unsigned long b = buttonYellowPressingTime - buttonYellowPressingTimeLast;
      Serial.println(b);
      if(debounceDelayYellow1 > b)
      {
      buttonPushCounter_Yellow_Green_fabric++;
      }
      else if ((debounceDelayYellow2 > b) && (b > debounceDelayYellow1))
      {
        buttonPushCounter_Yellow_Green_fabric += 2;
      }
      else if((debounceDelayYellow3 > b) && (b > debounceDelayYellow2))
      {
        buttonPushCounter_Yellow_Green_fabric += 3;
      }
      else if((debounceDelayYellow4 > b) && (b > debounceDelayYellow3))
      {
        buttonPushCounter_Yellow_Green_fabric += 5;
      }
      else if(b > debounceDelayYellow4){
        buttonPushCounter_Yellow_Green_fabric += 100;
      }
      //Serial.println(buttonStatus_Yellow);
      //Serial.println("da nhan Yellow");
    }
  }
  if (buttonStatus_Yellow != lastbuttonState_Yellow)
  {
    buttonYellowPressingTimeLast = buttonYellowPressingTime;
    lastbuttonState_Yellow = buttonStatus_Yellow;
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
      if (buttonPushCounter_Yellow_Green_fabric > 0)
      {
        if(debounceDelayGreen1 > a)
        {
          buttonPushCounter_Yellow_Green_fabric--;
        }
        else if((debounceDelayGreen2 > a) && (a > debounceDelayGreen1))
        {
          buttonPushCounter_Yellow_Green_fabric -= 2;
        }
        else if((debounceDelayGreen3 > a) && (a > debounceDelayGreen2))
        {
          buttonPushCounter_Yellow_Green_fabric -= 3;
        }
        else if((debounceDelayGreen4 > a) && (a > debounceDelayGreen3))
        {
          buttonPushCounter_Yellow_Green_fabric -= 4;
        }
        else if (a > debounceDelayGreen4)
        {
          buttonPushCounter_Yellow_Green_fabric -=100;
        }
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
  //Read value sensor
  //Read_value_BFC ();
 
 
  //Read button Stop
  //Read_value_M30();
 
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
    // Read coil value
    Read_value_M5();

    //Read met value
    Read_Vadlue_D54 ();

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
        Meters_fabric_A = Meters_fabric; //D54
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
    //Read setup meters fabric
    Read_value_metters_setup();
    
    //Read met value
    Read_Vadlue_D54 ();
    
    //Read D200
    Read_value_metters_setup_D200();

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
    display.setCursor(15, 20);
    display.println("Set BFC    :");
    display.setCursor(87, 20);
    display.println("aaaaaa");
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
    delay(50);
    Read_SettingValue_BF5R();
    delay(50);
  }

  display.display();
  
  // Button
Button_Red (); 
Button_Yellow ();
Button_Green ();  
    
  if (buttonPushCounter_red > 2)
  {
    buttonPushCounter_red = 0;
    Write_value_BFC();
  }
}


  
