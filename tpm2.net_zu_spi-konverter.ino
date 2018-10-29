
// Von Robert K. aka robertofon
// URL: https://github.com/Robertofon/pixelpanel-TPM2.Net-SPI
// More information at: https://www.aeq-web.com/esp32-wifi-http-webserver-und-wlan-scanner/?ref=arduinoide
// Basecamp:   git clone https://github.com/merlinschumacher/Basecamp.git --recurse-submodules '''

#include <Basecamp.hpp>
#include <FastLED.h>
#include <WiFiUdp.h>

Basecamp iot;
AsyncWebSocket ws("/cmd");

// Set Network Name and Password for net to login
unsigned int localPort = 65506;      // local port to listen on

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN MOSI
#define CLOCK_PIN SCK
#define NUM_LEDS 96  // How many leds in your strip? Leider fix
#define FIELD_WIDTH 8
#define FIELD_HEIGHT 12


// Definiere globale Variablen
bool debuginfo = false;
CRGB leds[NUM_LEDS];
WiFiUDP udpserver;
const byte CmdLEDCount = 0x10;

enum Stati
{
  Start,
  Unverbunden,
  Verbinde,
  Verbunden,
  Arbeit  
};

Stati status = Stati::Start;


void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  // handling code
  if(type == WS_EVT_CONNECT)
  {
     Serial.println("Websocket client connection received");
     client->text("Hello from ESP32 Server");
  } 
  else if(type == WS_EVT_DISCONNECT)
  {
    Serial.println("Client disconnected");
  }
  else if(type == WS_EVT_DATA)
  {
    // Datenempfang
    if( len >=1)
    {
      switch(data[0])
      {
        case 1: // links
        break;
        case 2: // rechts
        break;
        case 3: // oben
        break;
        case 4: // unten;
        break;
        case 5: // A
        break;
        case 6: // B
        break;
        case 7: // start/stop
        break;
        
      }
    }
  }
}

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

  iot.web.server.on("/test",  HTTP_GET, [](AsyncWebServerRequest * request)
  {
      request->send(200, "text/plain", "Hello World!");
  });

  ws.onEvent(onWsEvent);
  iot.web.server.addHandler(&ws);
  
  // basecamp innitalisieren
  iot.begin();
  
  //Use the web object to add elements to the interface
  iot.web.addInterfaceElement("color", "input", "", "#configform", "LampColor");
  iot.web.setInterfaceElementAttribute("color", "type", "text");
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

  // limit my draw to 4A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5,3000); 
  
  for( int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::DodgerBlue;
  FastLED.show();
  Serial.println("DodgerBlue");
  delay(500);
  for( int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Red;
  FastLED.show();
  Serial.println("Rot");
  delay(500);
  for( int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Yellow;
  FastLED.show();
  Serial.println("Gelb");
  delay(500);
  for( int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGB::Green;
  FastLED.show();
  Serial.println("gruen");
  delay(500);
  //fill_rainbow( &(leds[i]), 1 /*led count*/, 222 /*starting hue*/);
  FastLED.clear();
  FastLED.show();
}

// Blinkt einfach mal auf, um positiv zu Melden
void BlinkLED()
{
  fill_rainbow( &(leds[0]),  NUM_LEDS, /*led count*/ 222 /*starting hue*/);
  FastLED.show();
  delay(900);
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
      // Command modus  (um größeninfo zu liefern)
      if (blocktype == 0xC0)
      {
        // Befehlsmodus
        // Einziger Befehl: Lese Matrix-Size 0x10  Lesen der Anzahl angeschlossener  Pixel  0x)
        // Erstes Byte der Nutzdaten: Kommandokontrollbyte ; Zweites Byte: Befehl
        // Byte#1: MSB   (bit8) :  Richtung des Befehls (0=Lesebefehl, 1=Schreibbefehl) 
        // Byte#1: MSB-1 (bit7) :  Befehl erwartet Antwort? (0=Nein, 1=Ja)
        // Byte#1: MSB-2..LSB : Reserviert
        if( framelength >=2 )
        {
          bool schreibbefehl = packetBuffer[6] & 0x80 > 0;
          bool lesebefehl = !schreibbefehl;
          bool antworterwartet = packetBuffer[6] & 0x40 > 0;
          byte befehl = packetBuffer[7];
          if( befehl == CmdLEDCount && lesebefehl && antworterwartet)
          {
             byte antwort[3];
             antwort[0] = 0xAD;
             antwort[1] = (NUM_LEDS>>8) & 0xFF;
             antwort[2] = NUM_LEDS & 0xFF; 
             // send a reply, to the IP address and port that sent us the packet we received
             udpserver.beginPacket(udpserver.remoteIP(), udpserver.remotePort());
             udpserver.write(antwort, 3);
             udpserver.endPacket();             
          }
          
        }
        else // Fehler
        {
           // Befehle passen nicht in Nutzdaten weil weniger als 2 B. Fail!
        }
        
      }
      // Daten modus : Block hat 0xDA typ.
      else if (blocktype == 0xDA)
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
            // Sicherheitsabbruch
            if(led_index == NUM_LEDS)
            {
              packagenum = numpackages;
              break;
            }
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


void loop() 
{
  switch(status)
  {
    case Unverbunden:
    {
      Serial.println("Setup-LEDs");
      setupLED();

      Serial.print("Verbinde mit basecamp");
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
        BlinkLED();
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
