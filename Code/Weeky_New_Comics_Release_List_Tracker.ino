         /////////////////////////////////////////////  
        //    Marvel and DC Weekly New Comics      //
       //         Release List Tracker            //
      //           -----------------             //
     //          (Arduino Nano 33 IoT)          //           
    //             by Kutluhan Aktar           // 
   //                                         //
  /////////////////////////////////////////////

// Display comics release lists of this, next, and previous weeks via this amazing 🕷 PCB playing Spider-Man theme song and Nano 33 IoT.
// 
//
// For more information:
// https://www.theamplituhedron.com/projects/Marvel-and-DC-Weekly-New-Comics-Release-List-Tracker/
//
//
// Connections
// Arduino Nano 33 IoT :           
//                                Nokia 5110 Screen
// D2  --------------------------- SCK (Clk)
// D3  --------------------------- MOSI (Din) 
// D4  --------------------------- DC 
// D5  --------------------------- RST
// D6  --------------------------- CS (CE)
//                                Rotary Potentiometer
// A0  --------------------------- S
//                                Button (Right)
// A1  --------------------------- S
//                                Button (Left)
// A2  --------------------------- S
//                                Button (OK)
// A3  --------------------------- S
//                                Button (Up)
// A6  --------------------------- S
//                                Button (Down)
// A7  --------------------------- S
//                                RGB LEB (RCGB)
// D12 --------------------------- R
// D11 --------------------------- G
// D10 --------------------------- B
//                                Buzzer
// D8  --------------------------- +
//                                Vibration Motor
// D9  --------------------------- +


// Include required libraries:
#include <SPI.h>
#include <WiFiNINA.h>
#include <LCD5110_Graph.h>
#include "pitches.h"

char ssid[] = "<_SSID_>";        // your network SSID (name)
char pass[] = "<_PASSWORD_>";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;

// Enter the IPAddress of your Raspberry Pi.
IPAddress server(192, 168, 1, 20);

// Initialize the Ethernet client library
WiFiClient client; /* WiFiSSLClient client; */

// Define screen settings.
LCD5110 myGLCD(2,3,4,5,6);

extern uint8_t SmallFont[];
// Define the graphics:
extern uint8_t spiderman[];
extern uint8_t batman[];
extern uint8_t daredevil[];
extern uint8_t marvel[];
extern uint8_t superman[];

// Define controls:
#define pot A0
#define right A1
#define left A2
#define OK A3
#define up A6
#define down A7

// Define RGB LED pins.
#define redPin 12
#define greenPin 11
#define bluePin 10

// Define the buzzer pin.
#define buzzer 8

// Define the vibration motor pin.
#define vib_motor 9

// Define menu options:
volatile boolean this_week = false;
volatile boolean next_week = false;
volatile boolean previous_week = false;
volatile boolean graphics = false;
volatile boolean activated = false;

// Define the data holders:
int _pot, _right, _left, _OK, _up, _down, selected = 0, x = 0;
String comics_list, _date;

void setup() {
  Serial.begin(9600);
  
  // Control pin settings:
  pinMode(right, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(OK, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(vib_motor, OUTPUT);
  
  // Initiate screen.
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);

  // Check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) { myGLCD.print("Connection Failed!", 0, 8); myGLCD.update(); while (true); }
  // Attempt to connect to the Wifi network:
  while (status != WL_CONNECTED) {
    myGLCD.print("Waiting...", 0, 8);
    myGLCD.print("Attempting to", 0, 16);
    myGLCD.print("connect to", 0, 24);
    myGLCD.print("WiFi !!!", 0, 32);
    myGLCD.update();
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // Wait 10 seconds for connection:
    delay(10000);
  }

  // Verify connection on the Nokia 5110 Screen.
  myGLCD.clrScr();
  myGLCD.print("Connected to", 0, 8);
  myGLCD.print("WiFi!!!", 0, 16);
  myGLCD.update();
  delay(2000);
  myGLCD.clrScr();
  myGLCD.update();
}

void loop() {
  read_controls();
  change_menu_options();
  interface();

  if(this_week == true){
    do{
      myGLCD.invertText(true);
      myGLCD.print("A. This Week", 0, 16);
      myGLCD.invertText(false);
      myGLCD.update();
      adjustColor(255, 0, 255);
      delay(100);
      if(_OK == false){
        myGLCD.clrScr();
        myGLCD.update();
        activated = true;
        // Play the Spider-Man Theme Song:
        myGLCD.print("Playing...", CENTER, 16);
        myGLCD.update();
        play_Melody();
        myGLCD.clrScr();
        myGLCD.update();
        // Get the requested list from the web application:
        make_a_get_request("/New_Comics_Releases/?q=this");
        myGLCD.clrScr();
        myGLCD.update();
        while(activated == true){
          read_controls();
          myGLCD.print("This Week:", 0, 0);
          myGLCD.print(_date, CENTER, 16);
          scrolling_text(comics_list, 32);
          // Exit.
          if(_up == false || _down == false){ activated = false; x = 0; _date = ""; comics_list = ""; myGLCD.clrScr(); myGLCD.update(); }         
        }
      }
    }while(this_week == false);
  }
  
  if(next_week == true){
    do{
      myGLCD.invertText(true);
      myGLCD.print("B. Next Week", 0, 24);
      myGLCD.invertText(false);
      myGLCD.update();
      adjustColor(0, 255, 255);
      delay(100);
      if(_OK == false){
        myGLCD.clrScr();
        myGLCD.update();
        activated = true;
        // Play the Spider-Man Theme Song:
        myGLCD.print("Playing...", CENTER, 16);
        myGLCD.update();
        play_Melody();
        myGLCD.clrScr();
        myGLCD.update();
        // Get the requested list from the web application:
        make_a_get_request("/New_Comics_Releases/?q=next");
        myGLCD.clrScr();
        myGLCD.update();
        while(activated == true){
          read_controls();
          myGLCD.print("Next Week:", 0, 0);
          myGLCD.print(_date, CENTER, 16);
          scrolling_text(comics_list, 32);
          // Exit.
          if(_up == false || _down == false){ activated = false; x = 0; _date = ""; comics_list = ""; myGLCD.clrScr(); myGLCD.update(); }          
        }
      }
    }while(next_week == false);
  }

  if(previous_week == true){
    do{
      myGLCD.invertText(true);
      myGLCD.print("C. Previously", 0, 32);
      myGLCD.invertText(false);
      myGLCD.update();
      adjustColor(255, 255, 0);
      delay(100);
      if(_OK == false){
        myGLCD.clrScr();
        myGLCD.update();
        activated = true;
        // Play the Spider-Man Theme Song:
        myGLCD.print("Playing...", CENTER, 16);
        myGLCD.update();
        play_Melody();
        myGLCD.clrScr();
        myGLCD.update();
        // Get the requested list from the web application:
        make_a_get_request("/New_Comics_Releases/?q=previous");
        myGLCD.clrScr();
        myGLCD.update();
        while(activated == true){
          read_controls();
          myGLCD.print("Previous Week:", 0, 0);
          myGLCD.print(_date, CENTER, 16);
          scrolling_text(comics_list, 32);
          // Exit.
          if(_up == false || _down == false){ activated = false; x = 0; _date = ""; comics_list = ""; myGLCD.clrScr(); myGLCD.update(); }          
        }
      }
    }while(previous_week == false);
  }
  
  if(graphics == true){
    do{
      myGLCD.invertText(true);
      myGLCD.print("D. Graphics", 0, 40);
      myGLCD.invertText(false);
      myGLCD.update();
      adjustColor(0, 255, 0);
      delay(100);
      if(_OK == false){
        myGLCD.clrScr();
        myGLCD.update();
        activated = true;
        while(activated == true){
          read_controls();
          // Define picture queue:
          int pic = map(_pot, 100, 950, 1, 10);
          if(pic == 1) myGLCD.drawBitmap(20,0,spiderman,40,40);
          if(pic == 2) myGLCD.clrScr();
          if(pic == 3) myGLCD.drawBitmap(10,0,batman,60,32);
          if(pic == 4) myGLCD.clrScr();
          if(pic == 5) myGLCD.drawBitmap(2,0,daredevil,80,40);
          if(pic == 6) myGLCD.clrScr();
          if(pic == 7) myGLCD.drawBitmap(12,0,marvel,60,33);
          if(pic == 8) myGLCD.clrScr();
          if(pic == 9) myGLCD.drawBitmap(12,0,superman,60,60);
          if(pic == 10) myGLCD.clrScr();
          myGLCD.update();
          // Exit.
          if(_up == false || _down == false){ activated = false; myGLCD.clrScr(); myGLCD.update(); } 
        }
      }
    }while(graphics == false);
  }
  
}

void read_controls(){
  _pot = analogRead(pot);
  _right = digitalRead(right);
  _left = digitalRead(left);
  _OK = digitalRead(OK);
  _up = digitalRead(up);
  _down = digitalRead(down);  
}

void interface(){
   // Define options.
   myGLCD.print("Get Comics:", 0, 0);
   myGLCD.print("A. This Week", 0, 16);
   myGLCD.print("B. Next Week", 0, 24);
   myGLCD.print("C. Previously", 0, 32);
   myGLCD.print("D. Graphics", 0, 40);
   myGLCD.update();
}

void change_menu_options(){
  // Increase or decrease the option number using Up and Down buttons.
  if(_up == false) { selected++; digitalWrite(vib_motor, HIGH); }
  if(_down == false) { selected--; digitalWrite(vib_motor, HIGH); }
  if(selected < 0) selected = 4;
  if(selected > 4) selected = 1;
  delay(100);
  digitalWrite(vib_motor, LOW);
  // Depending on the selected option number, change boolean status.
  switch(selected){
    case 1:
      this_week = true;
      next_week = false;
      previous_week = false;
      graphics = false;
    break;
    case 2:     
      this_week = false;
      next_week = true;
      previous_week = false;
      graphics = false;
    break;
    case 3:
      this_week = false;
      next_week = false;
      previous_week = true;
      graphics = false;
    break;
    case 4:
      this_week = false;
      next_week = false;
      previous_week = false;
      graphics = true;
    break;
  }
}

void make_a_get_request(String application){
  // Connect to the web application named New_Comics_Releases. Change '80' with '443' if you are using SSL connection.
  if(client.connect(server, 80)){
    // If successful:
    myGLCD.print("Connected to", 0, 8);
    myGLCD.print("the server!!!", 0, 16);
    myGLCD.update();
    // Make an HTTP Get request:
    client.println("GET " + application + " HTTP/1.1");
    client.println("Host: 192.168.1.22");
    client.println("Connection: close");
    client.println();
  }else{
    myGLCD.print("Connection", 0, 8);
    myGLCD.print("Error!!!", 0, 16);
    myGLCD.update();
  }
  delay(2000); // Wait 2 seconds after connection...
  // If there are incoming bytes available, get the response from the web application.
  String response = "";
  while (client.available()) { char c = client.read(); response += c; }
  if(response != "" && response.endsWith("%")){
    // Split the response string by a pre-defined delimiter in a simple way. '%'(percentage) is defined as the delimiter in this project.
    int delimiter_1, delimiter_2, delimiter_3;
    delimiter_1 = response.indexOf("%");
    delimiter_2 = response.indexOf("%", delimiter_1 + 1);
    delimiter_3 = response.indexOf("%", delimiter_2 + 1);
    // Glean information as substrings.
    comics_list = response.substring(delimiter_1 + 1, delimiter_2);
    _date = response.substring(delimiter_2 + 1, delimiter_3);
  }
}

void play_Melody(){
  // Notes in the melody:
  int melody[] = {
    NOTE_C4, NOTE_DS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4, NOTE_C4,
    NOTE_DS4, NOTE_G4, NOTE_GS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4,
    NOTE_F4, NOTE_GS4, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_F4, NOTE_C4,
    NOTE_DS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4, NOTE_GS4, NOTE_G4,
    NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_C4,
    NOTE_C4, NOTE_DS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4, NOTE_C4,
    NOTE_DS4, NOTE_G4, NOTE_GS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4,
    NOTE_F4, NOTE_GS4, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_F4, NOTE_C4,
    NOTE_DS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4, NOTE_GS4, NOTE_G4,
    NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_C4,
    NOTE_G4, NOTE_GS4, NOTE_AS4, NOTE_GS4, NOTE_G4, NOTE_F4, NOTE_DS4,
    NOTE_F4, NOTE_DS4, NOTE_D4, NOTE_DS4, NOTE_G4, NOTE_GS4, NOTE_AS4,
    NOTE_GS4, NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_G4,
    NOTE_C4, NOTE_DS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4, NOTE_C4,
    NOTE_DS4, NOTE_G4, NOTE_GS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4,
    NOTE_F4, NOTE_GS4, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_F4, NOTE_C4,
    NOTE_DS4, NOTE_G4, NOTE_FS4, NOTE_DS4, NOTE_C4, NOTE_GS4, NOTE_G4,
    NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_C4,
    NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_C4, NOTE_G4,
    NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_D5
  };
  // Note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {
    6, 8, 2, 6, 8, 2, 6,
    8, 6, 8, 8, 6, 6, 2,
    6, 8, 2, 6, 8, 2, 6,
    8, 2, 6, 8, 2, 6, 1,
    8, 8, 8, 8, 8, 1,
    6, 8, 2, 6, 8, 2, 6,
    8, 6, 8, 8, 6, 6, 2,
    6, 8, 2, 6, 8, 2, 6,
    8, 2, 6, 8, 2, 6, 1,
    8, 8, 8, 8, 8, 1,
    6, 8, 2, 6, 2, 6, 8,
    2, 6, 8, 2, 6, 8, 2,
    6, 2, 6, 8, 2, 6, 8, 1,
    6, 8, 2, 6, 8, 2, 6,
    8, 6, 8, 8, 6, 6, 2,
    6, 8, 2, 6, 8, 2, 6,
    8, 2, 6, 8, 2, 6, 1,
    8, 8, 8, 8, 8, 1,
    8, 8, 8, 8, 8, 1, 6,
    6, 8, 6, 6, 1
  };
  // Calculate the length of the melody array:
  int len = sizeof(melody) / sizeof(melody[0]);
  // Play the melody using the given notes and durations:  
  for (int thisNote = 0; thisNote < len; thisNote++) {
    // To calculate the note duration, take one second divided by the note type.
    // e.g. quarter note = 1000 / 4, eighth note = 1000 / 8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzer, melody[thisNote], noteDuration);
    // To distinguish the notes, set a minimum time between them.
    // The note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // Stop the tone playing:
    noTone(buzzer);
  }
}

void scrolling_text(String text, int y){
  int len = text.length();
  // Scroll text using Right and Left buttons.
  if(_right == false) x++;
  if(_left == false) x--;
  if(x>84) x = 84;
  if(x<=-(len*6)) x = -(len*6);
  // Print.
  myGLCD.print(text, x, y);
  myGLCD.update();
  //delay(25);
}

void adjustColor(int red, int green, int blue){
 analogWrite(redPin, red);
 analogWrite(greenPin, green);
 analogWrite(bluePin, blue);
}
