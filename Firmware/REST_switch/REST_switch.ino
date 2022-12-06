#include <WiFiManager.h>
#include <WebServer.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32Time.h>

// WiFiManager object
WiFiManager wm;

int timer_reset_flag = true;
constexpr int TIMER_RESET_LED_PIN = 22;

constexpr int SW_GPIO = 23;

bool time_format_24h = true;
int tz_offset = 0;
ESP32Time rtc(tz_offset);
int epoch1 = 0;
int epoch2 = 0;

// Web server running on port 80
WebServer server(80);

// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];

// setup API resources
void setup_routing() {
  server.on("/name", getName);
  server.on("/sw_stat", get_sw_stat);
  server.on("/time", get_time);
  // server.on("/set_sw", HTTP_POST, set_sw_stat_cb);
  server.on("/set_time", HTTP_POST, set_time_cb);
  
  // start server
  server.begin();
}
 
void create_json(char *tag, char* value, char *unit) {  
  jsonDocument.clear();  
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}
 
void add_json_object(char *tag, char* value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void getName() {
  Serial.println("Get Name");
  create_json("name", "ESP32 Wifi Switch", " ");
  server.send(200, "application/json", buffer);
}
 
void get_sw_stat() {
  Serial.println("Switch Status");
  int sw_stat = is_switch_on();
  char temp_buff[10];
  itoa (sw_stat, temp_buff, 10);

  create_json("sw_stat", temp_buff, " ");
  server.send(200, "application/json", buffer);
}

void get_time() {
  const char* time = rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str();
  
  Serial.print("Time: ");
  Serial.println(time);

  create_json("time", const_cast<char*>(time), " ");
  server.send(200, "application/json", buffer);
}

// void set_sw_stat_cb() {
//   if (server.hasArg("plain") == false) {
//     Serial.println("Taking no action. No JSON argument found.");
//     return;
//   }

//   String body = server.arg("plain");
//   deserializeJson(jsonDocument, body);
  
//   // Get RGB components
//   uint8_t sw_stat = jsonDocument["sw_stat"];
//   Serial.print("Setting Switch Status: ");
//   Serial.println(sw_stat);

//   if (sw_stat == 1){
//     switch_on();
//     create_json("sw_stat", "1", " ");
//   }

//   if (sw_stat == 0) {
//     switch_off();
//     create_json("sw_stat", "0", " ");
//   }

//   server.send(200, "application/json", buffer);
// }

void switch_off()
{
  digitalWrite(SW_GPIO, HIGH);
}

void switch_on()
{
  digitalWrite(SW_GPIO, LOW);
}

bool is_switch_on()
{
  if (digitalRead(SW_GPIO) == LOW) return true;
  else return false;
}

void set_time_cb() {
  if (server.hasArg("plain") == false) {
    Serial.println("Taking no action. No JSON argument found.");
    return;
  }

  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  
  // Get time
  int h = jsonDocument["h"];
  int min = jsonDocument["min"];
  int s = jsonDocument["s"];
  int dd = jsonDocument["dd"];
  int mm = jsonDocument["mm"];
  int yyyy = jsonDocument["yyyy"];

  rtc.setTime(s, min, h, dd, mm, yyyy);
  // Respond to the client

  const char* time = rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str();  
  create_json("time", const_cast<char*>(time), " ");
  server.send(200, "application/json", buffer);

  timer_reset_flag = false;
}


void setup() 
{
    rtc.setTime(00, 00, 00, 11, 11, 2022);
    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(115200);
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    // WiFiManager wm;
    wm.setConfigPortalBlocking(false);
    // wm.setConfigPortalTimeout(60);

    bool res;
    res = wm.autoConnect("WifiSwitch", "vasantqunj"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        Serial.println("connected...yeey :)");
    }

    setup_routing();

    pinMode(SW_GPIO, OUTPUT);
    pinMode(TIMER_RESET_LED_PIN, OUTPUT);
    timer_reset_flag = true;

    epoch1 = rtc.getMillis();
    epoch2 = rtc.getEpoch();

    switch_off();
}

void loop() {
  wm.process();

  server.handleClient();

  if (timer_reset_flag == true) {
    if (rtc.getMillis() - epoch1 > 500) {
      digitalWrite(TIMER_RESET_LED_PIN, !digitalRead(TIMER_RESET_LED_PIN));
      epoch1 = rtc.getMillis();
    }
  }

    if (timer_reset_flag == false)
    {
      if ((rtc.getEpoch() - epoch2) >= 1)
      {
        if (rtc.getHour(time_format_24h) >= 18 && rtc.getHour(time_format_24h) <= 23)
        // if (rtc.getSecond() >= 10 && rtc.getSecond() <= 20)
          {
            if (is_switch_on() == false) switch_on();
            else switch_off();
          }
        else
          switch_off();
        
        epoch2 = rtc.getEpoch();
      }

      digitalWrite(TIMER_RESET_LED_PIN, LOW);
    }
}