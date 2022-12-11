/********************
- www.geekstips.com
- Arduino Time Sync from NTP Server using ESP8266 WiFi module 
- Arduino code example
 ********************/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

char ssid[] = "itimeafricaupload";  //  your network SSID (name)
char pass[] = "itimeafricaupload";       // your network password

unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP;
const char* ntpServerName = "pool.ntp.org";  //sa ntp goes down all the time. THis one chooses the closest avaialbe one apparently

const int NTP_PACKET_SIZE = 48;
const int timeZone = 2;     // South Africa Time
byte packetBuffer[ NTP_PACKET_SIZE];
WiFiUDP udp;

void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
    Serial.print("Connecting..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  udp.begin(localPort);
 
}

void loop()
{
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP);
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    delay(1);
  }
  else {
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears + timeZone * 3600; //epoch time adjusted to timezone(in seconds) 
    Serial.print("UNX");
    Serial.println(epoch);
  }
  delay(5000);
}

unsigned long sendNTPpacket(IPAddress& address)
{
//  Serial.println("sending NTP packet...");
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(address, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
