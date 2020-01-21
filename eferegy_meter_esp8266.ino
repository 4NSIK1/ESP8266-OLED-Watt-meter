/*********************************************************************
This is an example for Efergy Energy Meter website data acquisition
by wemos D1 and OLED display
You first need to register at https://engage.efergy.com/user/register then register your Efergy CT Sensor and network hub
You will be given a API Token to be used inserted in the code below
You will also need your WIFI SSID and password
Robert Fearn (c) 2020 
Using Adafruit SSD_1306 labrary with the following additions
#define SSD1306_64_48
#if defined SSD1306_64_48
  #define SSD1306_LCDWIDTH                  64
  #define SSD1306_LCDHEIGHT                 48
#endif
anc commenting out other size variant

Compiled with Board Wemos D1
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
//Replace YourToken with the Efergy supplied API_Token
#define ENERGY_API2 "https://engage.efergy.com/mobile_proxy/getInstant?token=YourToken"
#define ENERGY_API "http://www.energyhive.com/mobile_proxy/getCurrentValuesSummary?token=YourToken"

// [{"cid":"PWER","data":[{"1570927896000":459}],"sid":"788072","units":"W","age":2}]
const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5) + 60;

DynamicJsonDocument doc(capacity);
//replace the SSID and password below 
const char* ssid = "Your Wifi SSID";
const char* password = "Your Wifi password";
int time_delay = 6000;
bool Redraw1 = true;

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {
  Serial.begin(9600);
  delay(10);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  delay(1000); //2 seconds
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("WIFI cnx..");
  display.display();
  
  WiFi.hostname("Name");
  WiFi.begin(ssid, password);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // init done

  // Show image buffer on the display hardware.
  
  display.clearDisplay();
  delay(1000); //2 seconds
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); //0.5s
    display.print(".");
    display.display();
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi connected");
  
  // Print the IP address
  display.println(WiFi.localIP());
  display.display();
  delay(5000);
  
  // Clear the buffer.
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
}


void loop() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  HTTPClient http;  //Object of class HTTPClient
  http.begin(ENERGY_API2);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload=http.getString();
    int c =payload.length();
    int colon=0;
    int rightp=0;
    int l=0;
    int r =0;
    for (int x = 0; x < c; x++)
    {
      if (payload[x] == ':'){
        colon=colon+1;
        if (colon==3){
          l=x+1;
        }
       }
    }
    for (int x = 0; x < c; x++)
    {
      if (payload[x] == '}'){
        rightp=rightp+1;
        if (rightp==1){
          r=x;
        }
       }
     }
    //display.println(l);
    //display.println(r);
    for (int x = l; x < r; x++)
    {
      display.print(payload[x]);
       
     }
    display.print(" W");
  
    display.display();
  }   
  delay(time_delay);
}
