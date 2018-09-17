// Von Robert K.
//More information at: https://www.aeq-web.com/esp32-wifi-http-webserver-und-wlan-scanner/?ref=arduinoide

#include <WiFi.h>
#include <FastLED.h>
#include <WiFiUdp.h>

// Set Network Name and Password for net to login
const char* ssid     = "Chaosnetz";
const char* password = "ChCh1212";
unsigned int localPort = 65506;      // local port to listen on

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN MOSI
#define CLOCK_PIN SCK
#define NUM_LEDS 4  // How many leds in your strip?

// Definiere globale Variablen
bool debuginfo = true;
CRGB leds[NUM_LEDS];
WiFiUDP udpserver;
WiFiServer httpserver(80);

enum Stati
{
  Start,
  Unverbunden,
  Verbinde,
  Verbunden,
  Arbeit  
};

Stati status = Stati::Start;



char linebuf[80];
int charcount = 0;

// Die startroutine
void setup() 
{
  //Initialire serielle Schnittstelle und warte auf das Port-Öffnen
  Serial.begin(115200);
  Serial.println();
  Serial.println("TPM2.Net -> SPI V.02");
  Serial.print("Debuginfo:");
  Serial.println(debuginfo);
  status = Stati::Unverbunden;
  Serial.println("Unverbunden");
}

void setupLED()
{
  // Uncomment/edit one of the following lines for your leds arrangement.
  // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);

  // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);

  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  
  for( int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::DodgerBlue;
  FastLED.show();
  Serial.println("DodgerBlue");
  delay(900);
  for( int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Red;
  FastLED.show();
  Serial.println("Rot");
  delay(900);
  for( int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Green;
  FastLED.show();
  Serial.println("Green");
  delay(900);
  //fill_rainbow( &(leds[i]), 1 /*led count*/, 222 /*starting hue*/);
  FastLED.clear();
  FastLED.show();
}

int led_index=0;
void BehandleUDP()
{
  // if there's data available, read a packet
  int cb = udpserver.parsePacket();
  if(cb > 0)
  {   
    if(debuginfo)
    {
      Serial.print("Received packet of size ");
      Serial.println(cb);
      Serial.print("From ");
      IPAddress remoteIp = udpserver.remoteIP();
      Serial.print(remoteIp);
      Serial.print(", port ");
      Serial.println(udpserver.remotePort());
    }

    // read the packet into packetBufffer
    byte packetBuffer[1500];
    int len = udpserver.read(packetBuffer, 1500);
    if (cb >= 6 && packetBuffer[0] == 0x9C)
    { // header identifier (packet start)
  
  
      // send a reply, to the IP address and port that sent us the packet we received
      //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      //Udp.write(ReplyBuffer);
      //Udp.endPacket();
      
      //=======================
      byte blocktype = packetBuffer[1]; // block type (0xDA)
      unsigned int framelength = ((unsigned int)packetBuffer[2] << 8) | (unsigned int)packetBuffer[3]; // frame length (0x0069) = 105 leds
      if(debuginfo)
      {
         Serial.print("FrameL=");
         Serial.println(framelength); // chan/block
         Serial.print("BlockT=");
         Serial.println(blocktype, HEX);
      }
      byte packagenum = packetBuffer[4];   // packet number 0-255 0x00 = no frame split (0x01)
      byte numpackages = packetBuffer[5];   // total packets 1-255 (0x01)
      if (blocktype == 0xDA)
      {
        if(debuginfo)
        {
            Serial.print("Pkgnum=");  Serial.print(packagenum);   Serial.print("/");   Serial.println(numpackages);
        }
        int packetindex;

        // teste auch auf End-Byte !
        if (cb >= framelength + 7 && packetBuffer[6 + framelength] == 0x36)
        {
          int i = 0;
          packetindex = 6;
          if(packagenum == 1)
          {
            if(debuginfo)
            { Serial.println("ledindex=0 (reset)"); }
            led_index =0;
          }

          if(debuginfo)
          {  Serial.print("packetindex="); Serial.println(packetindex); }
          while(packetindex < (framelength + 6))
          {
            leds[led_index] = CRGB(((int)packetBuffer[packetindex]),
                ((int)packetBuffer[packetindex+1]),
                ((int)packetBuffer[packetindex+2]) );
            led_index++;         
            packetindex +=3;
          }
        }
      }
      else
      {
        // Unklares UDP-Paket
        Serial.println("Ungültiges Paket bekommen.");
        ;  // ignorieren      
      }
  
      if(debuginfo)
      {
          Serial.print("Pkgnum=");  Serial.print(packagenum);   Serial.print("/");   Serial.println(numpackages);
          Serial.print("led_index="); Serial.println(led_index);
      }
      if((packagenum == numpackages) && (led_index == NUM_LEDS))
      {
          if(debuginfo)
          {
            Serial.println("Rufe FastLED.show()");
          }
          FastLED.show();
          led_index==0;
      }  
    }
  }
}

/*
 * ESP is a pain to ensure statup
 * happens on a clean slate.
 */
void WiFiReset() 
{
    Serial.println("Reset WLAN");
    WiFi.persistent(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
}

void loop() 
{
  switch(status)
  {
    case Unverbunden:
    {
      Serial.println("Setup-LEDs");
      setupLED();

      Serial.print("Verbinde mit SSID ");
      Serial.println(ssid);
      WiFiReset();
      WiFi.begin(ssid, password);
      status = Stati::Verbinde;
    }
    break;
    
    case Verbinde:
    {
      //prüfe verb v, Wifi net:
      int wifistatus = WiFi.status();
      if (wifistatus != WL_CONNECTED) 
      {
        delay(500);
        Serial.print(".");
        Serial.print(wifistatus);
      }
      else
      {
        Serial.println("");
        Serial.println("WiFi verbunden");
        Serial.println("IP-Adresse: ");
        Serial.println(WiFi.localIP());
        status = Stati::Verbunden;
      }
    }
    break;
    
    case Verbunden:
    {
      Serial.println("starte UDP-Server...");
      // starte ud-pserver
      udpserver.begin(localPort);  
      status = Stati::Arbeit;
    }
    break;
    
    case Arbeit:
    {
      //BehandleWebAnfragen();
      BehandleUDP();
      
      if(WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Verbindung verloren!");
        status = Stati::Verbinde;
      }
    }
    break;
    
    default: 
    ;
  }
}


/*
  server.begin();
  
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");
    long rssi = WiFi.RSSI();
    memset(linebuf, 0, sizeof(linebuf));
    charcount = 0;
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        //read char by char HTTP request
        linebuf[charcount] = c;
        if (charcount < sizeof(linebuf) - 1) charcount++;
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.println("<!DOCTYPE HTML><html><head>");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
          client.println("<h1>ESP32 WiFi HTTP Server</h1>");
          client.print("<p>RSSI: ");
          client.print(rssi);
          client.println(" dBm</p>");
          client.print("<p>Local MAC: ");
          getMacAddress(client);
          client.println("</p>");
          client.print("<p>Neighborhood networks:</p>");
          getNetworks(client);
          client.println("");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          memset(linebuf, 0, sizeof(linebuf));
          charcount = 0;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive data
    delay(1);

    // close the connection
    client.stop();
    Serial.println("client disconnected");



void getMacAddress(WiFiClient client) 
{
  byte mac[6];
  WiFi.macAddress(mac);
  client.print(mac[5], HEX);
  client.print(":");
  client.print(mac[4], HEX);
  client.print(":");
  client.print(mac[3], HEX);
  client.print(":");
  client.print(mac[2], HEX);
  client.print(":");
  client.print(mac[1], HEX);
  client.print(":");
  client.println(mac[0], HEX);
}

void getNetworks(WiFiClient client) 
{
  int numSsid = WiFi.scanNetworks(); client.println("<table  border='1' style='width: 300px'>");
  client.println("<tr>");
  client.println("<th style='width: 15px'>NR</th>");
  client.println("<th style='width: 255px'>Name</th>");
  client.println("<th style='width: 20px'>dBm</th>");
  client.println("</tr>");
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    client.println("<tr>");
    client.println("<td>");
    client.print(thisNet);
    client.println("</td>");
    client.println("<td>");
    client.print(WiFi.SSID(thisNet));
    client.println("</td>");
    client.println("<td>");
    client.print(WiFi.RSSI(thisNet));
    client.println("</td>");
    client.println("</tr>");
  }
  client.println("</table>");
}

*/
