#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <dht.h>
LiquidCrystal lcd(13, 12, 11, 10, 8, 7);
dht DHT;
#define DHT11_PIN A4
#define HEAT_PIN 9
#define BUTTON_PREV A0
#define BUTTON_NEXT A1
#define BUTTON_OK A2
#define BUTTON_CANCEL A3
#define BUTTON_IND 2
#define RGB_RED 6
#define RGB_GREEN 5
#define RGB_BLUE 3
#define LED 4

#define unreadMessagesAddress 0
#define floodMessagesAddress 200
#define messageMaxLength 16
#define messageMaxNumber 10

char unreadMessages[messageMaxNumber][messageMaxLength];
char readMessages[messageMaxNumber][messageMaxLength];
char floodMessages[messageMaxNumber][messageMaxLength];
unsigned int unreadMessagesIndex = 0, unreadShowMessagesIndex = 0;
unsigned int readMessagesIndex = 0, unreadMessagesMaxIndex = 0, readShowMessagesIndex = 0;
unsigned int floodMessagesIndex = 0, floodMessagesMaxIndex = 0, floodShowMessagesIndex = 0;
unsigned int temp_r, temp_g, temp_b;
bool floodDetector = true;
unsigned int floodCounter = 0, lastMinute = 0;
unsigned int temperature = 0, lastTemperature = 0; bool temperatureRead = false;
unsigned int eepromWriteAddress = 0, eepromReadAddress = 0;
unsigned int counter = 0, menu = 1, date[]={21,5,2021}, timeNow[]={21, 5, 55};
unsigned int curentState[] = {0,0,0,0}, lastState[] = {0,0,0,0};
unsigned int password[] = {0, 1, 0, 1, 0, 1}, passwordIndex = 0;
float Err = 0.0, sumErr = 0.0, dErr = 0.0, lastErr = 0.0, temperatureCommand = 0.0, dt = 1.0;
float kp = 100.0, ki = 0.5, kd = 3.0;
unsigned int temperatureSetpoint;

void setup() 
{
  Serial.begin(9600);
  Serial.setTimeout(10);
  
  lcd.begin(16, 2);
  
  setTimer1();
  setExternalInterrupt();

  pinMode(BUTTON_PREV, INPUT);
  pinMode(BUTTON_NEXT, INPUT);
  pinMode(BUTTON_OK, INPUT);
  pinMode(BUTTON_CANCEL, INPUT);
  DDRD |= (1 << LED);
  
  readEepromMessages();
}

void loop() 
{
  if(temperatureRead)
  {
    temperatureRead = false;
    int chk = DHT.read11(DHT11_PIN);
    temperature = DHT.temperature;
    if(temperature < 15 || temperature > 50) temperature = lastTemperature;
    else lastTemperature = temperature;
  }
  serialGetMessages();
  checkFlood();
  buttonSettingPREV();
  buttonSettingNEXT();
  buttonSettingOK();
  buttonSettingCANCEL();
  delay(10);
}

void printMenu()
{
  lcd.clear();
  switch(menu)
  {
    //time & number of messages
    case 1:
      lcd.setCursor(0, 0);
      timePrint();
      lcd.setCursor(1, 1);
      lcd.print(temperature);
      lcd.print("*C "); 
      lcd.print(countUnreadMessages());
      lcd.print(" mesaje");
    break; 
    //password
    case 2: 
      lcd.setCursor(1, 0);
      lcd.print("Type Password:");
      lcd.setCursor(5, 1);
      for(int i=0;i<passwordIndex;i++) lcd.print("*");
    break;
    
    //main menu
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Main Menu");
      lcd.setCursor(0, 1);
      lcd.print("1. Messages");
    break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("Main Menu");
      lcd.setCursor(0, 1);
      lcd.print("2. Control"); 
    break;
    case 5:
      lcd.setCursor(0, 0);
      lcd.print("Main Menu");
      lcd.setCursor(0, 1);
      lcd.print("3. Temperature"); 
    break;
    case 6:
      lcd.setCursor(0, 0);
      lcd.print("Main Menu");
      lcd.setCursor(0, 1);
      lcd.print("4. Floods"); 
    break;

    //messages menu
    case 7: 
    lcd.setCursor(0, 0);
    lcd.print("Messages Menu");
    lcd.setCursor(0, 1);
    lcd.print("1.1 Unread"); 
    break;
    case 8: 
      lcd.setCursor(0, 0);
      lcd.print("Messages Menu");
      lcd.setCursor(0, 1);
      lcd.print("1.2 Read"); 
    break;
    case 9: 
      lcd.setCursor(0, 0);
      lcd.print("Messages Menu");
      lcd.setCursor(0, 1);
      lcd.print("1.3 Delete"); 
    break;

    //control menu
    case 10: 
      lcd.setCursor(0, 0);
      lcd.print("Control Menu");
      lcd.setCursor(0, 1);
      lcd.print("2.1 Manual [x]"); 
    break;
    case 11: 
      lcd.setCursor(0, 0);
      lcd.print("Control Menu");
      lcd.setCursor(0, 1);
      lcd.print("2.2 Automatic []"); 
    break;

    //temperature menu
    case 12: 
      lcd.setCursor(0, 0);
      lcd.print("Temperature Menu");
      lcd.setCursor(0, 1);
      lcd.print("3.1 Temp. ");
      lcd.print(temperature);
      lcd.print("*C"); 
    break;

    //floods messages menu
    case 13: 
      lcd.setCursor(0, 0);
      lcd.print("Floods Messages");
      lcd.setCursor(0, 1);
      lcd.print(floodMessages[floodShowMessagesIndex]);
    break;
    
    //unread messages menu
    case 14: 
      lcd.setCursor(0, 0);
      lcd.print("Unread Messages");
      lcd.setCursor(0, 1);
      lcd.print(unreadMessages[unreadShowMessagesIndex]);
    break;
    //read messages menu
    case 15: 
      lcd.setCursor(0, 0);
      lcd.print("Read Messages");
      lcd.setCursor(0, 1);
      lcd.print(readMessages[readShowMessagesIndex]);
    break;
    //delete messages menu
    case 16: 
      lcd.setCursor(0, 0);
      lcd.print("Delete Messages");
      lcd.setCursor(0, 1);
      lcd.print(readMessages[readShowMessagesIndex]);
    break;

    //command temperature
    case 17: 
      lcd.setCursor(2, 0);
      lcd.print("Setpoint "); lcd.print(temperatureSetpoint); lcd.print(" C");
      lcd.setCursor(0, 1);
      lcd.print("Temperature ");
      lcd.print(temperature);
      lcd.print(" C"); 
    break;

    //control menu manual
    case 18: 
      lcd.setCursor(0, 0);
      lcd.print("Manual Control");
      lcd.setCursor(0, 1);
      lcd.print("Command in SM!"); 
    break;
    //control menu automatic
    case 19: 
      lcd.setCursor(0, 0);
      lcd.print("Auto Control");
      lcd.setCursor(0, 1);
      lcd.print("Check the LEDs"); 
    break;
  }
}

void buttonSettingPREV()
{
  curentState[0] = digitalRead(BUTTON_PREV);//PINC & (1 << BUTTON_PREV);
  if((curentState[0] && !lastState[0]))
  {
    switch(menu)
    {
      case 2: 
        if(!password[passwordIndex] && passwordIndex < 6)
        {
          passwordIndex++;
        }
        else if(password[passwordIndex])
        {
          passwordIndex = 0;
          menu = 1;
        }
      break;
      case 3: menu = 6; break;
      case 4: menu = 3; break;
      case 5: menu = 4; break;
      case 6: menu = 5; break;
      
      case 7: menu = 9; break;
      case 8: menu = 7; break;
      case 9: menu = 8; break;
      
      case 10: menu = 11; break;
      case 11: menu = 10; break;
      
      case 13: 
      if(floodShowMessagesIndex == 0) floodShowMessagesIndex = (floodMessagesIndex - 1);
      else floodShowMessagesIndex--;
      break;
      
      case 14:
      if(unreadShowMessagesIndex == 0) unreadShowMessagesIndex = (unreadMessagesIndex - 1);
      else unreadShowMessagesIndex--;
      break;
      case 15: 
      if(readShowMessagesIndex == 0) readShowMessagesIndex = (readMessagesIndex - 1);
      else readShowMessagesIndex--;
      break;
      case 16: 
      if(readShowMessagesIndex == 0) readShowMessagesIndex = (readMessagesIndex - 1);
      else readShowMessagesIndex--;
      break;
    }
    printMenu();
  }
  lastState[0] = curentState[0];
}
void buttonSettingNEXT()
{
  curentState[1] = digitalRead(BUTTON_NEXT);//PINC & (1 << BUTTON_NEXT);
  if((curentState[1] && !lastState[1]))
  {
    switch(menu)
    {
      case 2: 
        if(password[passwordIndex] && passwordIndex < 6)
        {
          passwordIndex++;
          if(passwordIndex == 6) menu = 3;
        }
        else if(!password[passwordIndex])
        {
          passwordIndex = 0;
          menu = 1;
        }
      break;
      case 3: menu = 4; break;
      case 4: menu = 5; break;
      case 5: menu = 6; break;
      case 6: menu = 3; break;
      
      case 7: menu = 8; break;
      case 8: menu = 9; break;
      case 9: menu = 7; break;
      
      case 10: menu = 11; break;
      case 11: menu = 10; break;
      
      case 13: 
      if(floodShowMessagesIndex == (floodMessagesIndex - 1)) floodShowMessagesIndex = 0;
      else floodShowMessagesIndex++;
      break;
      
      case 14: 
      if(unreadShowMessagesIndex == (unreadMessagesIndex - 1)) unreadShowMessagesIndex = 0;
      else unreadShowMessagesIndex++;
      break;
      case 15: 
      if(readShowMessagesIndex == (readMessagesIndex - 1)) readShowMessagesIndex = 0;
      else readShowMessagesIndex++;
      break;
      case 16: 
      if(readShowMessagesIndex == (readMessagesIndex - 1)) readShowMessagesIndex = 0;
      else readShowMessagesIndex++;
      break;
    }
    printMenu();
  }
  lastState[1] = curentState[1];
}
void buttonSettingOK()
{
  curentState[2] = digitalRead(BUTTON_OK);//PINC & (1 << BUTTON_OK);
  if(curentState[2] && !lastState[2])
  {
    switch(menu)
    {
      case 1: menu = 2; break;
      case 3: menu = 7; break;
      case 4: menu = 10; break;
      case 5: menu = 12; break;
      case 6: menu = 13; break;
      case 7: menu = 14; break;
      case 8: menu = 15; break;
      case 9: menu = 16; break;
      case 10: menu = 18; break;
      case 11: menu = 19; break;
      case 14: unreadToReadMessage(); break;
      case 16: deleteReadMessage(); break;
    }
    printMenu();
  }
  lastState[2] = curentState[2];
}
void buttonSettingCANCEL()
{
  curentState[3] = digitalRead(BUTTON_CANCEL);//PINC & (1 << BUTTON_CANCEL);
  if(curentState[3] && !lastState[3])
  {
    switch(menu)
    {
      case 2: menu = 1; break;
      
      case 7: menu = 3; break;
      case 8: menu = 3; break;
      case 9: menu = 3; break;
      
      case 10: menu = 4; break;
      case 11: menu = 4; break;
      
      case 12: menu = 5; break;
      
      case 13: menu = 6; break;
      
      case 14: menu = 7; break;
      case 15: menu = 8; break;
      case 16: menu = 9; break;
      case 17: menu = 3; resetPID(); break;
      case 18: menu = 10; break;
      case 19: 
      analogWrite(RGB_RED, 255);
      analogWrite(RGB_GREEN, 255);
      analogWrite(RGB_BLUE, 255);
      PORTD &= ~(1 << LED);
      menu = 11; 
      break;
    }
    printMenu();
  }
  lastState[3] = curentState[3];
}

void timeUpdate()
{
  if(timeNow[2] < 59) timeNow[2]++;
  else if(timeNow[2] == 59)
  {
    timeNow[2] = 0;
    if(timeNow[1] < 59) timeNow[1]++;
    else if(timeNow[1]== 59)
    {
      timeNow[1] = 0;
      if(timeNow[0] < 23) timeNow[0]++;
      else if(timeNow[0] == 23) 
      {
        timeNow[0] = 0;
        if(date[0] < 32) date[0]++;
        else if(date[0] == 32)
        {
          date[0] = 1;
          if(date[1] < 13) date[1]++;
          else if(date[1] == 13)
          {
            date[1] = 1;
            date[2]++;
          }
        }
      }
    }
  }
}

void timePrint()
{
  lcd.clear();
  lcd.setCursor(4,0);
  if(timeNow[0]<10) lcd.print("0");
  lcd.print(timeNow[0]); lcd.print(":");
  if(timeNow[1]<10) lcd.print("0");
  lcd.print(timeNow[1]);lcd.print(":");
  if(timeNow[2]<10) lcd.print("0");
  lcd.print(timeNow[2]);
}
int countUnreadMessages()
{
  unsigned int k = 0;
  for(unsigned int i = 0; i < messageMaxNumber * messageMaxLength; i += messageMaxLength)
  {
    if(EEPROM.read(i) != 0) k++;
  }
  return k;
}

void serialGetMessages()
{
  if(Serial.available() > 0)
  {
    String stringMessage = Serial.readString();
    if(stringMessage[0] == 'A')
    {
      PORTD |= (1 << LED);
    }
    else if(stringMessage[0] == 'S')
    {
      PORTD &= ~(1 << LED);
    }
    else if(stringMessage[0] == '#')
    {
      serialToRGB(stringMessage);
    }
    else if(stringMessage[0] == '@' && stringMessage[stringMessage.length()-1] == '@')
    {
      //"@Write Temp 35 C@"
      commandTemperature(stringMessage);
    }
    else if(stringMessage[0] == '<' && stringMessage[stringMessage.length()-1] == '>')
    {
      String newMessage = stringMessage.substring(1, stringMessage.length()-1);
      Serial.println(newMessage);
      serialMessageToArduino(newMessage);
    }
  }
}


void commandTemperature(String tempString)
{
  String str = tempString.substring(12,14);
  temperatureSetpoint = (int) strtol(str.c_str(), 0, 10);
  Serial.print("Temperature Setpoint: "); Serial.print(temperatureSetpoint); Serial.println(" C");
  menu = 17;
}

void PID()
{
  Err = temperatureSetpoint - temperature;
  sumErr = (sumErr + Err) * dt;
  dErr = (Err - lastErr) / dt;
  temperatureCommand = (kp * Err) + (ki * sumErr) + (kd * dErr);
  temperatureCommand = constrain(temperatureCommand, 0, 255);
  analogWrite(HEAT_PIN, int(temperatureCommand));
  lastErr = Err;
}

void resetPID()
{
  Err = 0.0; lastErr = 0.0; sumErr = 0.0; dErr = 0.0; 
  temperatureSetpoint = 0.0; temperatureCommand = 0.0; 
  analogWrite(HEAT_PIN, 0);
}
void serialToRGB(String hex)
{
  String rr = hex.substring(1,3);
  String gg = hex.substring(3,5);
  String bb = hex.substring(5,7);

  byte r = (int) strtol(rr.c_str(), 0, 16);
  byte g = (int) strtol(gg.c_str(), 0, 16);
  byte b = (int) strtol(bb.c_str(), 0, 16);
    
  Serial.print("red: "); Serial.print(r);
  Serial.print(" green: "); Serial.print(g);
  Serial.print(" blue: "); Serial.println(b);
  
  analogWrite(RGB_RED, 255-r);
  analogWrite(RGB_GREEN, 255-g);
  analogWrite(RGB_BLUE, 255-b);
}

void serialMessageToArduino(String message)
{
  if(unreadMessagesIndex < messageMaxNumber) 
  {
    strcpy(unreadMessages[unreadMessagesIndex], message.c_str());
    EEPROM.put(unreadMessagesAddress + unreadMessagesIndex*messageMaxLength, unreadMessages[unreadMessagesIndex]);
    
    unreadMessagesIndex++;
    unreadMessagesMaxIndex++; 
    EEPROM.put(400, unreadMessagesMaxIndex);
  }
  else
  {
    for(unsigned int i = 0; i < messageMaxNumber - 1; i++)
    {
      String temp;
      temp = (String)unreadMessages[i];
      strcpy(unreadMessages[i], unreadMessages[i + 1]);
      strcpy(unreadMessages[i + 1], temp.c_str());
    }
    strcpy(unreadMessages[messageMaxNumber - 1], message.c_str());
    for(unsigned int i = 0 + unreadMessagesAddress; i < (messageMaxNumber - 1)*messageMaxLength; i+=messageMaxLength)
    {
      char temp1[16], temp2[16];
      EEPROM.get(i, temp1);
      EEPROM.get((i + messageMaxLength), temp2);
      EEPROM.put(i, 0);
      EEPROM.put((i + messageMaxLength), 0);
      EEPROM.put(i, temp2);
      EEPROM.put((i + messageMaxLength), temp1);
    }
    EEPROM.put(unreadMessagesAddress + (messageMaxNumber - 1)*messageMaxLength, 0);
    EEPROM.put(unreadMessagesAddress + (messageMaxNumber - 1)*messageMaxLength, unreadMessages[messageMaxNumber - 1]);
  }
}
void unreadToReadMessage()
{
  addReadMessage((String)unreadMessages[unreadShowMessagesIndex]);
  for(unsigned int i = unreadShowMessagesIndex; i < unreadMessagesIndex - 1; i++)
  {
    String temp;
    temp = (String)unreadMessages[i];
    strcpy(unreadMessages[i], unreadMessages[i + 1]);
    strcpy(unreadMessages[i + 1], temp.c_str());
  }
  unreadMessages[unreadMessagesIndex - 1][0] = '\0';
  for(unsigned int i = unreadShowMessagesIndex*messageMaxLength + unreadMessagesAddress; i < (unreadMessagesIndex - 1)*messageMaxLength; i+=messageMaxLength)
  {
    char temp1[16], temp2[16];
    EEPROM.get(i, temp1);
    EEPROM.get((i + messageMaxLength), temp2);
    EEPROM.put(i, 0);
    EEPROM.put((i + messageMaxLength), 0);
    EEPROM.put(i, temp2);
    EEPROM.put((i + messageMaxLength), temp1);
  }
  EEPROM.put(unreadMessagesAddress + (unreadMessagesIndex - 1)*messageMaxLength, 0);
  
  unreadMessagesIndex--;
  unreadShowMessagesIndex = 0;
  unreadMessagesMaxIndex--;
  EEPROM.put(400, unreadMessagesIndex);
}
void addReadMessage(String message)
{
  if(readMessagesIndex < messageMaxNumber) strcpy(readMessages[readMessagesIndex], message.c_str());
  else
  {
    for(unsigned int i = 0; i < messageMaxNumber - 1; i++)
    {
      String temp;
      temp = (String)readMessages[i];
      strcpy(readMessages[i], readMessages[i + 1]);
      strcpy(readMessages[i + 1], temp.c_str());
    }
    strcpy(readMessages[messageMaxNumber - 1], message.c_str());
  }
  readMessagesIndex++;
}
void deleteReadMessage()
{
  for(unsigned int i = readShowMessagesIndex; i < readMessagesIndex - 1; i++)
  {
    String temp;
    temp = (String)unreadMessages[i];
    strcpy(readMessages[i], readMessages[i + 1]);
    strcpy(readMessages[i + 1], temp.c_str());
  }
  readMessages[messageMaxNumber - 1][0] = '\0';
  readMessagesIndex--;
  readShowMessagesIndex = 0;
}

void readEepromMessages()
{
  EEPROM.get(400, unreadMessagesMaxIndex);
  EEPROM.get(404, floodMessagesMaxIndex);
  if(unreadMessagesMaxIndex != 0)
  {
    for(unsigned int i = 0 + unreadMessagesAddress; i < unreadMessagesMaxIndex*messageMaxLength + unreadMessagesAddress; i+=messageMaxLength)
    {
      EEPROM.get(i, unreadMessages[unreadMessagesIndex]);
      //Serial.print(unreadMessages[unreadMessagesIndex]);
      //Serial.print(" ");
      //Serial.println(i);
      
      unreadMessagesIndex++;
    }
  }
  if(floodMessagesMaxIndex != 0)
  {
    for(unsigned int i = 0 + floodMessagesAddress; i < floodMessagesMaxIndex*messageMaxLength + floodMessagesAddress; i+=messageMaxLength)
    {
      EEPROM.get(i, floodMessages[floodMessagesIndex]);
      //Serial.print("mesaj: ");
      //Serial.print(floodMessages[floodMessagesIndex]);
      //Serial.print(" adresa: ");
      //Serial.println(i);
      floodMessagesIndex++;
    }
  }
}


void writeFloodMessage()
{
  if(timeNow[1] != lastMinute)
  {
    String floodMessage;
    if(timeNow[0] < 10) floodMessage += "0";
    floodMessage += timeNow[0];
    floodMessage += ":";
    if(timeNow[1] < 10) floodMessage += "0";
    floodMessage += timeNow[1];
    floodMessage += " ";
    if(date[0] < 10) floodMessage += "0";
    floodMessage += date[0];
    floodMessage += "/";
    if(date[1] < 10) floodMessage += "0";
    floodMessage += date[1];
    floodMessage += "/";
    floodMessage += date[2];
    Serial.println(floodMessage);
    
    if(floodMessagesIndex < messageMaxNumber) 
    {
      Serial.print("index: "); Serial.print(floodMessagesIndex); Serial.print(" mesaj: "); Serial.println(floodMessage.c_str());
      strcpy(floodMessages[floodMessagesIndex], floodMessage.c_str());
      EEPROM.put(floodMessagesAddress + floodMessagesIndex*messageMaxLength, floodMessages[floodMessagesIndex]);
      floodMessagesIndex++;
      floodMessagesMaxIndex++;
      EEPROM.put(404, floodMessagesMaxIndex);
    }
    else
    {
      for(unsigned int i = 0; i < messageMaxNumber - 1; i++)
      {
        String temp;
        temp = (String)floodMessages[i];
        strcpy(floodMessages[i], floodMessages[i + 1]);
        strcpy(floodMessages[i + 1], temp.c_str());
      }
      strcpy(floodMessages[messageMaxNumber - 1], floodMessage.c_str());
      for(unsigned int i = 0 + floodMessagesAddress; i < (messageMaxNumber - 1)*messageMaxLength; i+=messageMaxLength)
      {
        char temp1[16], temp2[16];
        EEPROM.get(i, temp1);
        EEPROM.get((i + messageMaxLength), temp2);
        EEPROM.put(i, 0);
        EEPROM.put((i + messageMaxLength), 0);
        EEPROM.put(i, temp2);
        EEPROM.put((i + messageMaxLength), temp1);
      }
      EEPROM.put(floodMessagesAddress + (messageMaxNumber - 1)*messageMaxLength, 0);
      EEPROM.put(floodMessagesAddress + (messageMaxNumber - 1)*messageMaxLength, floodMessages[messageMaxNumber - 1]);
    }
  }
  lastMinute = timeNow[1];
}

void checkFlood()
{
  if(!(PIND &= 1 << BUTTON_IND))
  {
    writeFloodMessage();
    Serial.println("!FLOOD WARNING!");
    floodDetector = false;
  }
}

void setExternalInterrupt()
{
  PORTD |= 1 << BUTTON_IND;
  EICRA = 0;
  EIMSK |= 1 << INT0;
}
ISR(INT0_vect)
{
  floodCounter++;
  if(floodCounter >= 100)
  {
    floodDetector = true;
    floodCounter = 0;
  }
}
void setTimer1()
{
  cli();
  TCCR1A = 0; TCCR1B = 0; TCCR1C = 0;
  TCCR1B |= 1 << WGM12;
  TCCR1B |= ((1 << CS10) | (1 << CS12));
  OCR1A = 314;
  TIMSK1 |= 1 << OCIE1A;
  sei();
}
ISR(TIMER1_COMPA_vect)
{
  if(!(counter%50))
  {
    temperatureRead = true;
    if(menu == 1) printMenu();
    if(menu == 12) printMenu();
    if(menu == 17) 
    {
      printMenu(); 
      PID();
      if(temperature == temperatureSetpoint)
      {
        resetPID();
        menu = 1;
      }
    }
    if(menu == 19)
    {
      if((counter/10)%2==0) PORTD |= (1 << LED);
      else PORTD &= ~(1 << LED);
      Serial.println(counter);
      temp_r = random(0, 255);
      temp_g = random(0, 255);
      temp_b = random(0, 255);
      analogWrite(RGB_RED, 255-temp_r);
      analogWrite(RGB_GREEN, 255-temp_g);
      analogWrite(RGB_BLUE, 255-temp_b);
    }
    if(counter!=0 && (menu < 2 || menu == 4))
    {
      Serial.print("Current Temperature: "); 
      Serial.print(temperature);
      Serial.println(" C"); 
    }
    timeUpdate();
  }
  counter++;
}
