#include <Dhcp.h>
#include <Dns.h>
#include <UIPClient.h>
#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPUdp.h>
#include <ethernet_comp.h>
#include <SPI.h>
#include <SD.h>
#include <UTFT.h>
#include <URTouch.h>
#include <EEPROM.h>

#define requestBuffer  80
#define dataBuffer   5
/////////////////////////////////

//konfiguracaja dotyku i polaczenia
extern uint8_t SmallFont[];
UTFT myGLCD(ITDB28,A5,A4,A3,A2);
URTouch  myTouch( A1, A3, A0, 8, 9);
/////////////////////////////////


//////////////////////////pozostale zmienne
int val = EEPROM.read(0);
int entryTime = val;
int val2 = EEPROM.read(1);
int exitTime = val2;

bool remoteDisarm=false;
bool remoteArming=false;
bool armed=false;
bool mainMenu=true;
bool configMenu = false;
bool infoMenu = false;
const float version = 1.0;
int sensorState = 0;
char alert[20]="";

//////////////////////////////////////////////
////////////////////////////////piny
const int sensor = A8;


//konfiguracja danych serwerowych
byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x31 }; 
EthernetClient clientA;
char server[] = "192.168.1.14";                                             

//statyczne ip na potrzeby testów OFFLINE, lokalnie
byte ip[]      = { 192, 168,   1,  25 };                
byte gateway[] = { 192, 168,   1,   1 };
byte subnet[]  = { 255, 255, 255,   0 };

//ustawienia serwerowe
EthernetServer serverA = EthernetServer(80);       // create a server at port 80
File webFile;
char HTTP_req[requestBuffer] = {0};
char req_index = 0;   

char webEntry[dataBuffer] = {0};
char webExit[dataBuffer] = {0};
///////////////////////////////////


void setup()
{   
   pinMode(53, OUTPUT); //pin cs ethernet
   digitalWrite(53, HIGH);
   pinMode(40, OUTPUT); //pin cs sd card
   digitalWrite(40, HIGH);

  myGLCD.InitLCD(LANDSCAPE); 
  myGLCD.setFont(SmallFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);
  draw_menu();

  
  //////konfiguracja karty sd///////////////////////
    Serial.begin(9600);
    Serial.println("Initializing SD card...");
    if (!SD.begin(40)) {
        Serial.println("ERROR - SD card initialization failed!");
    }
    else Serial.println("SUCCESS - SD card initialized.");

    if (!SD.exists("login2.htm")) {
        Serial.println("ERROR - Can't find login file!");
    }
    else Serial.println("SUCCESS - Found login file.");
    
    if (!SD.exists("config2.htm")) {
        Serial.println("ERROR - Can't find config file!");
    }
    else Serial.println("SUCCESS - Found config file.");

   Serial.end(); 
  ////////////////konfig karty ether////////////
  Ethernet.begin(mac, ip, gateway, subnet);
  String type="Uruchomienie";
  strcpy(alert, type.c_str()); 
  
  //////////////konfig serwera///////////////////
  serverA.begin();
  reportAlert(type);  
}


void loop()
{
int x,y;
    EthernetClient client = serverA.available();
    serverListener(client);
    ////////////////////////////////////    
    myTouch.read();
    //////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////panel config
    x = myTouch.getX();
    y = myTouch.getY();
    if(x>=95 && x<=220 && y>=110 && y<=150 && mainMenu)
    {
      myGLCD.clrScr();
      draw_configMenu();
      draw_exitTime(exitTime);
      draw_entryTime(entryTime);
      configMenu = true;
      mainMenu = false;
    }
    ///////////////////////////////////////////////////////////
    //////////////////////////////////////////////konfigurowanie czasow
      if(configMenu)
      {
        //////////konfigurowanie czasu wyjscia///////
        x = myTouch.getX();
        y = myTouch.getY();
        if(x>=135 && x<=175 && y>=80 && y<=119 && exitTime>10)
        {
          exitTime = exitTime - 1;
          draw_exitTime(exitTime);
          delay(500);
        }
        //zwiekszenie czasu uzbrojenia
        x = myTouch.getX();
        y = myTouch.getY();
        if(x>=265 && x<=305 && y>=80 && y<=119 && exitTime<30)
        {
          exitTime = exitTime + 1;
          draw_exitTime(exitTime);
          delay(500);
        }
        ///////////////////////////////////////////////
        ///////////konfigurowanie czasu wejscia////////
        x = myTouch.getX();
        y = myTouch.getY();
        if(x>=135 && x<=175 && y>=150 && y<=189 && entryTime>10)
        {
         entryTime = entryTime - 1;
          draw_entryTime(entryTime);
          delay(500);
        }
        x = myTouch.getX();
        y = myTouch.getY();
        if(x>=265 && x<=305 && y>=150 && y<=189 && entryTime<30)
        {
          entryTime = entryTime + 1;
          draw_entryTime(entryTime);
          delay(500);
        }
      }
   //////////////////////////////////////////////////////////////info panel
    x = myTouch.getX();
    y = myTouch.getY();
    if(x>=95 && x<=220, y>=160 && y<=200 && mainMenu)
    {
      draw_infoBox(exitTime, entryTime, version);
      mainMenu=false;
      infoMenu=true;
    }
    //////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////powrot do menu
    x = myTouch.getX();
    y = myTouch.getY();
    if(x>=0 && x<=124 && y>=200 && y<=230 && !mainMenu)
    {
      myGLCD.clrScr();
      draw_menu();
      configMenu = false;
      infoMenu = false;
      mainMenu = true;
    }
    //////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////uzbrajanie    
    x = myTouch.getX();
    y = myTouch.getY();
    if(x>=95 && x<=220 && y>=60 && y<=100 && mainMenu || remoteArming == true || remoteArming)
    {
      draw_arming(exitTime);
      armed=true; 
      mainMenu=false;
      configMenu = false;
      infoMenu = false;

      String type = "Uzbrojenie";
      strcpy(alert, type.c_str());
      reportAlert(type);
    }
    /////////////////////////////////////////////////////////////////////////  
    /////////////////////////////////////////////////////////////dzialania uzbrojonego alarmu
    while(armed)
    {
      ///////////////nasłuchiwanie//////////
      EthernetClient client = serverA.available();
      serverListener(client);
      ////////////////////////////////////
      digitalWrite(sensor, LOW);
      sensorState = digitalRead(sensor);
      
      if(sensorState == HIGH) 
      {
        unsigned long previousMillis = millis();
        //unsigned long *previous = previousMillis;
        bool alarmTrigger = false;
        draw_disarmButton();
        while(armed)
        {              
          alarmTrigger=timeElapsed(previousMillis, entryTime*1000);
          if(alarmTrigger && armed)
          {
            remoteArming = false;
            armed = false;
            draw_menu();
            mainMenu=true;
          }
        }
      }
      else if(remoteDisarm == true)
      {
            String type = "Rozbrojenie";
            strcpy(alert, type.c_str());
            reportAlert(type);
            remoteArming = false;
            remoteDisarm = false;
            armed = false;
            draw_menu();
            mainMenu=true;
      }
    }   
}

//funkcje
void reportAlert(String type)
{      
      //for(int i=0; i<2; i++)
      //{
        clientA.connect(server, 80);
      //} 
      clientA.print( "GET /log/add_data.php?");
      clientA.print("alert=");
      clientA.print(type);
      clientA.println( " HTTP/1.0");
      clientA.println( "Connection: close" );
      clientA.println();
      clientA.println();
      clientA.stop();     
}

void serverListener(EthernetClient client)
{
  if (client) {  // got client?
        Serial.begin(9600);
        pinMode(10, OUTPUT); //pin cs LCD_sd card
        digitalWrite(10, HIGH);
    
        boolean currentLineIsBlank = true;
        while (client.connected()) {        
            if (client.available()) {   
                char c = client.read(); 
                Serial.print (c);
                    if (req_index < (requestBuffer - 1)) {
                    HTTP_req[req_index] = c;          
                    req_index++;
                }          
                if (c == '\n' && currentLineIsBlank) 
                {                
                    if (StrContains(HTTP_req, "ajax_inputs")){
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        if (GetDataFromWebsite(webEntry, webExit, dataBuffer)){
                          long a = atol(webEntry);
                          if(a>=10 && a<=30){
                            entryTime = a;
                            EEPROM.write(0, entryTime);
                          }                          
                          Serial.print("Entry value: ");
                          Serial.println(webEntry);
                          //Serial.println("EEprom value: ");
                          //Serial.println(EEPROM.read(0));
                          
                          long b = atol(webExit); 
                          if(b >= 10 && b<=30){    
                            exitTime = b;
                            EEPROM.write(1, exitTime);
                          }
                          Serial.print("Exit value: ");                          
                          Serial.println(webExit);
                          //Serial.println("EEprom value: ");
                          //Serial.println(EEPROM.read(1));
                        }
                    } 
                    else if (StrContains(HTTP_req, "remoteArm")){
                        Serial.println("zlapalem remote arming");
                        client.println("Content-Type: text/xml");
                        client.println("Connection: close");
                        client.println();
                        remoteArming = true;
                        Serial.println(remoteArming);
                    } 
                    else if (StrContains(HTTP_req, "remoteDisarm")){
                        Serial.println("zlapalem remote arming");
                        client.println("Content-Type: text/xml");
                        client.println("Connection: close");
                        client.println();
                        if(armed)
                          remoteDisarm = true;
                        Serial.println(remoteDisarm);
                    }                                 
                    else if (StrContains(HTTP_req, "GET / ") || StrContains(HTTP_req, "GET /login.htm")){
                        Serial.println("login"); 
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("login2.htm");       
                    }
                    else if (StrContains(HTTP_req, "POST /config2.htm")){                                             
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("config2.htm");       
                    }                                                       
                    else if (StrContains(HTTP_req, "GET /main.jpg")) {
                        webFile = SD.open("main.jpg");
                          if (webFile) {
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: image/jpeg");
                            client.println("Connection: keep-alive");
                            client.println();
                          }
                    }
                    if (webFile) {
                      //Serial.println("wypluwam strone"); 
                        while(webFile.available()) {     
                            char fill=webFile.read();
                            if(fill == '~' && StrContains(HTTP_req, "POST /config2.htm"))
                            {
                              client.println(exitTime);                  
                            }
                            else if(fill == '|' && StrContains(HTTP_req, "POST /config2.htm"))
                            {
                              client.println(entryTime);                   
                            }
                            else if(fill == '`' && StrContains(HTTP_req, "POST /config2.htm"))
                            {
                              client.println(alert);
                            }
                            else
                              client.write(fill); 
                        }
                        webFile.close();
                    }

                    req_index = 0;
                    StrClear(HTTP_req, requestBuffer);
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    currentLineIsBlank = false;
                }                
            } 
        } // end while (client.connected()), dorzcenie client.stop(); na koniec zapobiega zapetleniu się kodu po nieudanym podlaczeniu sie przegladarki. przegladarka nie zamyka sesji
        delay(1);      
        client.stop(); 
    }
    Serial.end();
}





boolean GetDataFromWebsite(char *line1, char *line2, int len)
{
  boolean got_text = false;    // text received flag
  char *str_begin;             // pointer to start of text
  char *str_end;               // pointer to end of text
  int str_len = 0;
  int txt_index = 0;
  char *current_line;

  current_line = line1;

  str_begin = strstr(HTTP_req, "&L1=");

  for (int j = 0; j < 3; j++) { // do for 3 lines of text
    if (str_begin != NULL) {
      str_begin = strstr(str_begin, "=");  // skip to the =
      str_begin += 1;                      // skip over the =
      str_end = strstr(str_begin, "&");

      if (str_end != NULL) {
        str_end[0] = 0;  // terminate the string
        str_len = strlen(str_begin);

        // copy the string to the buffer and replace %20 with space ' '
        for (int i = 0; i < str_len; i++) {
          if (str_begin[i] != '%') {
            if (str_begin[i] == 0) {
              // end of string
              break;
            }
            else {
              current_line[txt_index++] = str_begin[i];
              if (txt_index >= (len - 1)) {
                // keep the output string within bounds
                break;
              }
            }
          }
          else {
            // replace %20 with a space
            if ((str_begin[i + 1] == '2') && (str_begin[i + 2] == '0')) {
              current_line[txt_index++] = ' ';
              i += 2;
              if (txt_index >= (len - 1)) {
                // keep the output string within bounds
                break;
              }
            }
          }
        } 
        current_line[txt_index] = 0;
        if (j == 0) {
          // got first line of text, now get second line
          str_begin = strstr(&str_end[1], "L2=");
          current_line = line2;
          txt_index = 0;
        }
        got_text = true;
      }
    }
  } 
  return got_text;
}

void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }
    return 0;
}




void draw_menu()
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 50);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("Witaj w systemie alarmowym Arduino", CENTER, 20);
  
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(95, 60, 220, 100); //guzik 1
  myGLCD.fillRect(95, 110, 220, 150); //guzik 2
  myGLCD.fillRect(95, 160, 220, 200); //guzik 3
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.print("Uzbroj", CENTER, 75); //tekst guzika 1
  myGLCD.print("Konfiguruj", CENTER, 125); //tekst guzika 2
  myGLCD.print("Informacje", CENTER, 175); //tekst guzika 3 cz1 
}

void draw_configMenu()
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 50);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("Panel konfiguracyjny", CENTER, 20);
  
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 80, 124, 120); //pole czasu uzbrojenia
  myGLCD.fillRect(0, 150, 124, 190); // pole czasu rozbrojenia
  myGLCD.fillRect(0, 200, 124, 230); //guzik powrot
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.print("Czas", 50, 85); //tekst guzika uzbrojenia cz1
  myGLCD.print("do uzbrojenia", 13, 100); //tekst guzika uzbrojenia cz2
  myGLCD.print("Czas", 50, 155); //tekst guzika rozbrojenia cz1
  myGLCD.print("do rozbrojenia", 10, 170); //tekst guzika rozbrojenia cz2
  myGLCD.print("Powrot", 40, 210); //tekst powrotu
  
  //guziki konfiguracyujne
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.drawRect(135, 80, 175, 119); //guzik - dla uzbrojenia
  myGLCD.print("-", 152, 95); // - dla uzbrojenia
  myGLCD.drawRect(190, 80, 250, 119); //ramka do pokazywania czasu uzbrojenia
  myGLCD.drawRect(265, 80, 305, 119); //guzik + dls uznrojenia
  myGLCD.print("+", 282, 95); //+ dla uzbrojenia
  
  
  myGLCD.drawRect(135, 150, 175, 189); //guzik - dla rozbrojenia
  myGLCD.print("-", 152, 165); // - dla rozbrojenia
  myGLCD.drawRect(190, 150, 250, 189); //ramka do pokazywania czasu rozbrojenia
  myGLCD.drawRect(265, 150, 305, 189); //guzik + dls roznrojenia
  myGLCD.print("+", 282, 165); //+ dla rozbrojenia  
}

void draw_exitTime(int entryTimer)
{
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(205, 90, 230, 110);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(entryTimer, 210, 95);
  myGLCD.print("s", 230,95);  
}

void draw_entryTime(int entryTimer)
{
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(205, 160, 230, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(entryTimer, 210, 165);
  myGLCD.print("s", 230, 165);  
}

void draw_infoBox(int timer, int timer2, float version)
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 50);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("Panel informacyjny", CENTER, 20);
  
  myGLCD.setBackColor(0,0,0);
  myGLCD.print("wersja oprogramowania:", 10, 70);
  myGLCD.print("v", 200, 70);
  myGLCD.printNumF(version, 2, 210, 70);
  myGLCD.print("Czas uzbrojenia:", 10, 110);
  myGLCD.printNumI(timer, 200, 110);
  myGLCD.print("Czas rozbrojenia:", 10, 130);
  myGLCD.printNumI(timer2, 200, 130);
  
  myGLCD.setColor(64,64,64);
  myGLCD.fillRect(0, 200, 124, 230); //guzik powrot
  myGLCD.setColor(255,255,255);
  myGLCD.setBackColor(64,64,64);
  myGLCD.print("Powrot", 40, 210); //tekst powrotu
}

void draw_arming(int timer)
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 50);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("Uzbrajanie alarmu", CENTER, 20);
  myGLCD.setBackColor(0,0,0);
  myGLCD.print("Alarm zostanie uzbrojony za:", CENTER, 80);
  for(int i=0; i<=timer; i++)
  {
    myGLCD.setColor(0,0,0);
    myGLCD.fillRect(150, 95, 165, 110);
    myGLCD.setColor(255, 255, 255);
    myGLCD.printNumI(timer-i, CENTER, 100);
    myGLCD.print("s", 170, 100);
    delay(1000);
  }
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 95, 319, 145);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("Alarm uzbrojony!", CENTER, 115);
}

void draw_disarmButton()
{
  myGLCD.setColor(64,64,64);
  myGLCD.fillRect(95, 170, 220, 210);
  myGLCD.setColor(255,255,255);
  myGLCD.setBackColor(64,64,64);
  myGLCD.print("Rozbroj", CENTER, 185);
}

boolean timeElapsed(unsigned long previousMillis, unsigned int interval) //interwał tu będzie czasem wejścia 
{
  unsigned long currentMillis;
  unsigned long diff;

  currentMillis = millis();
  diff = currentMillis - previousMillis;

    myTouch.read();
    int x,y;
    x = myTouch.getX();
    y = myTouch.getY();
    
    if(diff >= interval)
    {
      //digitalWrite(buzzer, HIGH);
      delay(5000);
      //digitalWrite(buzzer, LOW);
      String type = "Naruszenie";
      strcpy(alert, type.c_str());
      reportAlert(type);
      return true;
    }
    else if(x>=95 && x<=220 && y>=170 && y<=210)
    {
      String type = "Rozbrojenie";
      strcpy(alert, type.c_str());
      reportAlert(type);
      remoteArming = false;
      armed=false;
      draw_menu();
      mainMenu=true;
    }   
    else 
    {
      return false;
    } 
}
