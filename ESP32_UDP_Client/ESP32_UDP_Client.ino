#include <WiFi.h>
#include <WiFiUdp.h>

const byte BUFF_SIZE = 2;
const byte NUM_CHARS = 100;
char receivedChars[NUM_CHARS];
char tempChars[NUM_CHARS]; // Temporary array for use when parsing

// Information to send UDP data to
char ssid[NUM_CHARS] = {0};
char pwd[NUM_CHARS] = {0};
char udpAddress[NUM_CHARS] = {0};
int udpPort = 9999; // Some random port

bool newData = false;
bool sendAvailable = false;

WiFiUDP udp;

void connectToWiFi()
{
  //Connect to the WiFi network
  WiFi.begin(ssid, pwd);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  udp.begin(udpPort);

  sendAvailable = true;
}

void sendData()
{
  // Data to be sent
  uint8_t buffer[BUFF_SIZE];

  Serial2.readBytes(buffer, BUFF_SIZE);
  Serial.write(buffer, BUFF_SIZE);

  // Send data to server
  udp.beginPacket(udpAddress, udpPort);
  udp.write(buffer, BUFF_SIZE);
  udp.endPacket();

  delay(100);
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= NUM_CHARS) {
          ndx = NUM_CHARS - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // Terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parseData() { // Split the data into its parts

  char * strtokIndx; // This is used by strtok() as an index

  strtokIndx = strtok(tempChars, ":"); // Get the first part - the SSID
  strcpy(ssid, strtokIndx); // Copy it to ssid

  strtokIndx = strtok(NULL, ":"); // Get the second part - the password
  strcpy(pwd, strtokIndx); // Copy it to pwd

  strtokIndx = strtok(NULL, ":"); // Get the third part - the UDP address
  strcpy(udpAddress, strtokIndx); // Copy it to udpAddress

  strtokIndx = strtok(NULL, ","); // Get the fourth part - the UDP port
  udpPort = atoi(strtokIndx); // Convert this part to an integer and copy it to udpPort
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // This temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();
    newData = false;
    connectToWiFi();
  }
  uint8_t buffer[BUFF_SIZE];

  //Serial2.readBytes(buffer, BUFF_SIZE);
  //Serial.write(buffer, BUFF_SIZE);
  //for(int i = 0; i < BUFF_SIZE; i++)
  //{
  //  Serial.print((char)buffer[i]);
  //}

  if (sendAvailable == true) // Allowed to send data to C# application
  {
    sendData();
  }
}
