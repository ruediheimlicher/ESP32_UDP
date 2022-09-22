//Author of source code: Martin Chlebovec (martinius96@gmail.com)
//Project's website: https://arduino.clanweb.eu/udp-control-esp32.php?lang=en
//Buy me coffee: https://paypal.me/chlebovec
// https://www.instructables.com/ESP32-Control-Via-UDP-Datagrams/
#include "WiFi.h"
#include "AsyncUDP.h"
const char* ssid = "AP RE";
const char* pass = "eiramsor44wl";
const int rele = 23;
#define SERVO_OUT 33
#include "settings.h"

AsyncUDP udp;

uint16_t sliderwert = 0;


hw_timer_t *  kanalimpulsTimer;
  
volatile int interruptCounter;

// ppm-stream

#define PPM_FRAME_LENGTH 22500
#define PPM_PULSE_LENGTH 300
#define PPM_CHANNELS 8
#define DEFAULT_CHANNEL_VALUE 1500
#define OUTPUT_PIN 14

uint16_t channelValue[PPM_CHANNELS] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};
uint16_t kanalwerte[PPM_CHANNELS] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

enum ppmState_e
{
    PPM_STATE_IDLE,
    PPM_STATE_PULSE,
    PPM_STATE_FILL,
    PPM_STATE_SYNC
};


// end ppm-stream




void setup()
{
  Serial.begin(115200);
  pinMode(rele, OUTPUT);
   pinMode(SERVO_OUT, OUTPUT);
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  if (udp.listen(7844)) 
  {
      Serial.print("ESP_22 UDP Listening on IP: ");
      Serial.println(WiFi.localIP());
      
      udp.onPacket([](AsyncUDPPacket packet) 
      {
        
         uint8_t* sliderdata = (uint8_t*)packet.data();
         for(int i = 0; i < 16; i++)
         {
            Serial.print(sliderdata[i]);
            Serial.print(" ");
         }
         Serial.println("");
         for(int i = 0; i < 8; i++)
         {
            kanalwerte[i] =  (int(sliderdata[2*i+1]) <<8) | int(sliderdata[2*i]);
            
            Serial.print(kanalwerte[i]);
            Serial.print(" ");
         }
        Serial.println("");
         /*
         Serial.print("UDP Packet Type: ");
         Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
         Serial.print(", From: ");
         Serial.print(packet.remoteIP());
         Serial.print(":");
         Serial.print(packet.remotePort());
         Serial.print(", To: ");
         Serial.print(packet.localIP());
         Serial.print(":");
         Serial.print(packet.localPort());
         Serial.print(", Length: ");
         Serial.print(packet.length()); //dlzka packetu
         */
         Serial.print(", Data: ");
         
         Serial.print(int(sliderdata[0]));
         Serial.print(" ");
         Serial.print(int(sliderdata[1]));
         Serial.print(" ");
         
         sliderwert = (int(sliderdata[1]) <<8) | int(sliderdata[0]);
         Serial.println(sliderwert);
         
         
         //  Serial.write(packet.data(), packet.length());
         //Serial.println(" ");
         String myString = (const char*)packet.data();
         
          packet.printf("Habe %u bytes data bekommen", packet.length());
      });
   }
  
 }

void loop()
{
  delay(1000);
  udp.broadcast("Anyone here?");
}
