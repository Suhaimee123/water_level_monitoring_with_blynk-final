#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6a_YNa5KW"
#define BLYNK_TEMPLATE_NAME "water level monitoring"
#include <Wire.h>
#include <time.h> // Include the time.h library
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <ESP_Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#define LED1 D3
#define LED2 D4
#define LED3 D5
#define trig D7
#define echo D8
#define relay D6


// sync all status
BLYNK_CONNECTED() {
    Blynk.syncAll();
}

char auth[] = "lDSY4uBTY0Zu5rH1yvxFl82Ms_undwgB";
//char ssid[] = "Realme";
//char pass[] = "123456789";
char ssid[] = "pig_2.4G";
char pass[] = "95699999";

int MaxLevel = 15;
int Level1 = (MaxLevel * 75) / 100;
int Level2 = (MaxLevel * 55) / 100;
int Level3 = (MaxLevel * 35) / 100;
int distance = 0;
int blynkDistance = 0;
int desiredWaterLevel = 0; 
float powerGeneration = 0; 
int totalPowerGeneration = 0; 

int timezone = 7;       // Zone +7 for Thailand
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int dst = 0;
int Sec = 0;
int interval = 10;
time_t lastExecutionTime = 0;


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.println("DHTxx test!");
  display.begin(SSD1306_SWITCHCAPVCC, 0x78 >> 1);
  display.display();
  delay(2000);
  display.clearDisplay();


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  /*---- Config Time ----*/
  configTime(timezone * 3600, dst, ntp_server1, ntp_server2, ntp_server3);
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  } 
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

}

void ultrasonic() {
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  distance = t / 29 / 2;

  blynkDistance = (distance - MaxLevel) * -1;

  if (distance <= MaxLevel) {
    Blynk.virtualWrite(V0, blynkDistance);
  } else {
    Blynk.virtualWrite(V0, 0);
  }
  Serial.println(blynkDistance);

  if (blynkDistance >= desiredWaterLevel) {
    digitalWrite(relay, LOW); // Turn on the relay
  } else if (blynkDistance <= (desiredWaterLevel - 1)) {
    digitalWrite(relay, HIGH); // Turn off the relay
  }

   if (distance <= Level3) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    Blynk.virtualWrite(V5, +1); 
    Blynk.virtualWrite(V6, -1); 
    Blynk.virtualWrite(V7, -1); 

  } else if (distance <= Level2) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    Blynk.virtualWrite(V7, +1); 
    Blynk.virtualWrite(V5, -1); 
    Blynk.virtualWrite(V6, -1); 


  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    Blynk.virtualWrite(V6, +1); 
    Blynk.virtualWrite(V7, -1);
    Blynk.virtualWrite(V5, -1); 
 

  }
}

BLYNK_WRITE(V1) {
  bool Relay = param.asInt();
  if (Relay == 1) {
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
    desiredWaterLevel = 15;
    Blynk.virtualWrite(V2, desiredWaterLevel);
  }
}

BLYNK_WRITE(V2) {
  desiredWaterLevel = param.asInt(); 
}



void updatePowerGeneration() {
  float mappedDesiredWaterLevel = map(desiredWaterLevel, 0, 15, 7.5 * 10, 1); // Map to the desired range
  powerGeneration = mappedDesiredWaterLevel * 0.5;
  // Map powerGeneration back to the original range (optional)
  // float mappedPowerGeneration = map(powerGeneration, 0, 7.5 * 0.5, 0, 15);
  // Format powerGeneration as a string with 2 decimal places
  String formattedPowerGeneration = String(powerGeneration, 2);

  Blynk.virtualWrite(V3, formattedPowerGeneration + " KW");
}





void updatePowerSummary() {
  float currentPowerGeneration = powerGeneration + powerGeneration ;
  totalPowerGeneration += currentPowerGeneration; 
  Blynk.virtualWrite(V4, totalPowerGeneration , "KW"); 
}



void moniter() {
  display.setTextSize(1);
  display.setTextColor(WHITE);  
  display.setCursor(8, 8);
  display.print("blynkDistance: ");  
  display.setTextSize(2);
  display.setCursor(42, 36);
  display.print(blynkDistance);  
  display.drawRect(24, 22, 86, 40, WHITE); // drawRect(x,y,w,h,color)
  display.display();
  delay(1000);
  display.clearDisplay(); 

}



void GG_ScriptSheet() {

  axTLS::WiFiClientSecure client;
  const char* HOST = "script.google.com";
  const char* GScriptId = "AKfycbxczuNxH4-jGOyR4yCYv79qxiLK4PpoCYPqDiRgeRlZPiOgTuHdRvkRycB3Ie7CZkCJ";

  // Use the obtained values in the dataToLog string
  String dataToLog = "/macros/s/" + String(GScriptId) + "/exec?FIELD1=" + String(desiredWaterLevel) + "&FIELD2=" + String(blynkDistance) + "&FIELD3=" + String(totalPowerGeneration); // Assuming you want to pass "0" for Summarytoday

  Serial.print("Client.Connecting...");

  if (client.connect(HOST, 443)) {
    client.println("GET " + dataToLog + " HTTP/1.0");
    client.println("Host: " + String(HOST));
    client.println("User-Agent: ESP8266\r\n");
    client.println("Connection: close\r\n\r\n");
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(dataToLog.length()));
    client.println();
    client.print(dataToLog);

    Serial.print("Response...");
  }
  Serial.println("OK !!!");
}

String NowString() {
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  String tmpNow = "";
  tmpNow += String(newtime->tm_year + 1900);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mon + 1);
  tmpNow += "-";
  tmpNow += String(newtime->tm_mday);
  tmpNow += " ";
  tmpNow += String(newtime->tm_hour);
  tmpNow += ":";
  tmpNow += String(newtime->tm_min);
  tmpNow += ":";
  tmpNow += String(newtime->tm_sec);
  Serial.println(tmpNow);
  Sec = newtime->tm_sec;
  return tmpNow;

}

void loop() {
  Blynk.run();

  static unsigned long lastUltrasonicTime = 0;
  static unsigned long lastPowerGenerationTime = 0;
  static unsigned long lastPowerSummaryTime = 0;
  static unsigned long lastMonitorTime = 0;
  static unsigned long lastNowStringTime = 0;
  static unsigned long lastGGScriptSheetTime = 0;
  static unsigned long lastResetTime = 0;


  unsigned long currentMillis = millis();

  // Get the current time string
  String currentNowString = NowString();

  // Check if it's midnight (00:00)
  if (currentNowString.startsWith("00:00:00")) {
    // Reset all timers to the current time
    lastUltrasonicTime = currentMillis;
    lastPowerGenerationTime = currentMillis;
    lastPowerSummaryTime = currentMillis;
    lastMonitorTime = currentMillis;
    lastNowStringTime = currentMillis;
    lastGGScriptSheetTime = currentMillis;

    
    totalPowerGeneration = 0;

  }

  // Check if it's time to run ultrasonic()
  if (currentMillis - lastUltrasonicTime >= 1000) {
    lastUltrasonicTime = currentMillis;
    ultrasonic();
  }

  // Check if it's time to run updatePowerGeneration()
  if (currentMillis - lastPowerGenerationTime >= 1000) {
    lastPowerGenerationTime = currentMillis;
    updatePowerGeneration();
  }

  // Check if it's time to run updatePowerSummary()
  if (currentMillis - lastPowerSummaryTime >= 5000) {
    lastPowerSummaryTime = currentMillis;
    updatePowerSummary();
  }

  // Check if it's time to run moniter()
  if (currentMillis - lastMonitorTime >= 1000) {
    lastMonitorTime = currentMillis;
    moniter();
  }

  // Use the stored NowString value if needed in other functions

  // Check if it's time to run GG_ScriptSheet()
  if (currentMillis - lastGGScriptSheetTime >= 10000) {
    lastGGScriptSheetTime = currentMillis;
    GG_ScriptSheet();
  }  
    if (currentMillis - lastResetTime >= 600000) {
    lastResetTime = currentMillis;
    totalPowerGeneration = 0;

}
}

  



