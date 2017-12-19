// Nicholas Stonecipher
// December 12, 2017
// REMOTE

#include <SPI.h>
#include "RF24.h"
#include "WiFiEsp.h"
#include "SoftwareSerial.h"

// radio
RF24 radio(7,8);
const int PACKETSIZE = 6;
byte packet[PACKETSIZE];
int radioChannel = 131;
byte address[6] = {"[REDACTED]"};

// wifi
SoftwareSerial Serial1(9, 10);
WiFiEspClient client;
int charLength = 256;
char str[256] = "";
char ssid[] = "[REDACTED]";
char pass[] = "[REDACTED]";

// Controls
int joyXPin = A0;
int joyYPin = A1;
int selCPin = A2;

// Command strings
String pingCmd = "`ping";
String moveCmd = "`move ";
String turnCmd = "`turn ";

// Just so these don't get initialized too often
unsigned long currentTime = 0;
unsigned long previousTime = 0;

void setup() {
  // Initialize wifi
  Serial.begin(115200);
  Serial1.begin(9600);
  WiFi.init(&Serial1);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while ( WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Retrying");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
    
  // Initialize radio
  radio.begin();
  radio.setChannel(radioChannel);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();
  radio.openWritingPipe(address);
}

void loop() {
  // Reconnects incase ESP times out
  if (!client.connected()) {
    if (client.connect("irc.twitch.tv", 6667)) {
      client.println("PASS oauth:somerandomstringyoucantseelol\r");  // Authorizes to twitch through oauth
      client.println("NICK TwitchPlaysRCCar\r");                      // Sets NICK so that twitch can verify
      client.println("JOIN #twitchplaysrccar\r");                     // Joins channel to listen to requests
    }
  }

  // Loops as long as ircControl() is true
  while(ircControl()) {
    while(client.available()) {
      String temp = getString();
      Serial.print(temp);
      if(isPing(temp)) {
        client.println("PONG tmi.twitch.tv\r\n");   // Required to keep connection alive. Auto-disconnects after two failed pings.
      } else if (isCommand(temp, pingCmd)) {
        computePing();                              // Blinks an LED on the car for 1 second.
      } else if (isCommand(temp, moveCmd)){
        computeMove(strchr(temp.c_str(), 96));      // Computes the move command, 'forward' or 'back'
      } else if (isCommand(temp, turnCmd)) {
        computeTurn(strchr(temp.c_str(), 96));      // Computes the turn command, 'left' or 'right'
      }                                             // The subsequant functions essentially ignore the command if conditions are not met.
    }
  }

  // Loops as long as ircControl() is false
  while(!ircControl()) {
    sendToCar(analogRead(joyXPin), analogRead(joyYPin), 1, 0);
    if(ircControl()) {
      sendToCar(512, 512, 0, 0);
      break;
    }
  }
}

// Reads the analog signal from the potentiometer.
// I would have used a switch, but that was unavailable.
boolean ircControl() {
  return (analogRead(A2) > 512) ? true : false;
}

// Takes the stream of characters and puts it in a String.
// Yes this is C, and yes it kinda does that already.
// This was to make my Java brain process this a little easier.
String getString() {
  memset(str, 0, charLength);
  for (int i = 0; i < charLength; i++) {
    if (client.available()){
      str[i] = client.read();
    } else {
      break;
    }
  }
  return str;
}

// See point made above.
// Makes it easier than declaring a variable, then c_str(), in the code
const char* getChar(String temp) {
  return temp.c_str();
}

// Once `move is met, it compares to 'forward' and 'back' and computes
// the integer following the direction.
void computeMove(String temp) {
  String alt = temp;
  if (!strncmp(temp.c_str(), "`move forward", 13) && (strlen(temp.c_str()) > 14)) {
    int thisTime = temp[14];
    if (thisTime > 48 && thisTime < 59) {
      sendToCar(1023, 512, 0, thisTime-48);
    }
  } else if (!strncmp(alt.c_str(), "`move back", 10) && (strlen(alt.c_str()) > 11)) {
    int thisTime = alt[11];
    if (thisTime > 48 && thisTime < 59) {
      sendToCar(0, 512, 0, thisTime-48);
    }
  }
}

// Once `turn is met, it compares to 'left' and 'right' and computes 
// the integer following the direction. 
void computeTurn(String temp) {
  String alt = temp;
  if (!strncmp(temp.c_str(), "`turn left", 10) && (strlen(temp.c_str()) > 11)) {
    int thisTime = temp[11];
    if (thisTime > 48 && thisTime < 59) {
      sendToCar(512, 0, 0, thisTime-48);
    }
  } else if (!strncmp(alt.c_str(), "`turn right", 11) && (strlen(alt.c_str()) > 12)) {
    int thisTime = alt[12];
    if (thisTime > 48 && thisTime < 59) {
      sendToCar(512, 1023, 0, thisTime-48);
    }
  }
}

// Simply tells the car to turn on LED for 1 second.
void computePing() {
  sendToCar(512,512,1,4);
}

// Sends X, Y, boolean light (in 1 and 0), and the amount of time to trigger
// timeOn is divided by 4.
void sendToCar(int xValue, int yValue, int light, int timeOn) {
  int *intPacket = (int *) packet;
  intPacket[0] = xValue;
  intPacket[1] = yValue;
  intPacket[2] = light;
  if (timeOn != 0) {                                    // Enters a loop if there is a specified time.
    previousTime = millis();
    while (true) {
      currentTime = millis();
      if (currentTime - previousTime <= 250*timeOn) {
        radio.write((void*)packet, PACKETSIZE, false);
      } else {                                          // Breaks the while loop once time expires.
        intPacket[0] = 512;
        intPacket[1] = 512;
        intPacket[2] = 0;
        radio.write((void*)packet, PACKETSIZE, false);  // Writes a packet to "stop" the car from moving.
        previousTime = currentTime;
        return;
      }
    }
  } else {
    radio.write((void*)packet, PACKETSIZE, false);      // Writes a packet with the basic movement command.
  }
}

// Checks if the line is a ping from the server.
boolean isPing(String temp) {
  return (bool) (strncmp(temp.c_str(), "P", strlen("P") == 0));
}

// Checks if the line contains the specified command string.
boolean isCommand(String phrase, String cmd) {
  return (bool) (strstr(phrase.c_str(), cmd.c_str()) != NULL);
}
