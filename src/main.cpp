// Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <time.h>
#include <ESP32Servo.h> // Include the servo motor library
#include <PubSubClient.h>

// Definitions
#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64

#define BUZZER 18
#define LED_1 15
#define LED_2 2
#define LED_3 4
#define CANCEL 14
#define UP 27
#define DOWN 32
#define OK 33
#define ALARM 25
#define DHT 12
#define LEFT_LDR_PIN 34
#define RIGHT_LDR_PIN 35
#define SERVO_PIN 5

// Object Declarations
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dhtSensor;

Servo servoMotor;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Variables
int n_notes = 8;
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C, D, E, F, G, A, B, C_H};

int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

bool alarm_enabled = false;
bool alarm_ringing = false;
int n_alarms = 3;
int alarm_hours[] = {0, 0, 0};
int alarm_minutes[] = {1, 2, 3};
bool alarm_triggered[] = {false, false, false};

float THETA_OFFSET[] = {30, 55, 60, 0};
float GAMMA[] = {0.75, 0.5, 0.86, 0};
int tablet = 0;

volatile bool cancelButtonPressed = false;
volatile bool okButtonPressed = false;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 500;

int current_mode = 0;
int max_modes = 5;
String options[] = {"1 - Set Time", "2 - Set Alarm 1", "3 - Set Alarm 2", "4 - Set Alarm 3", "5 - Remove Alarm"};

int time_mode_current = 0;
int max_time_modes = 2;
String time_options[] = {"1 - Set Time Auto", "2 - Set Time Manual"};
bool time_mode = true; // true → time mode auto, false → time mode manual
String time_zones[][2] =
    {{"-1200", "<-12>12"}, {"-1130", "<-1130>11:30"}, {"-1100", "<-11>11"}, {"-1030", "<-1030>10:30"}, {"-1000", "<-10>10"}, {"-0930", "<-0930>9:30"}, {"-0900", "<-09>9"}, {"-0830", "<-0830>8:30"}, {"-0800", "<-08>8"}, {"-0730", "<-0730>7:30"}, {"-0700", "<-07>7"}, {"-0630", "<-0630>6:30"}, {"-0600", "<-06>6"}, {"-0530", "<-0530>5:30"}, {"-0500", "<-05>5"}, {"-0430", "<-0430>4:30"}, {"-0400", "<-04>4"}, {"-0330", "<-0330>3:30"}, {"-0300", "<-03>3"}, {"-0230", "<-0230>2:30"}, {"-0200", "<-02>2"}, {"-0130", "<-0130>1:30"}, {"-0100", "<-01>1"}, {"-0030", "<-0030>0:30"}, {"+0000", "<+00>-0"}, {"+0030", "<+0030>-0:30"}, {"+0100", "<+01>-1"}, {"+0130", "<+0130>-1:30"}, {"+0200", "<+02>-2"}, {"+0230", "<+0230>-2:30"}, {"+0300", "<+03>-3"}, {"+0330", "<+0330>-3:30"}, {"+0400", "<+04>-4"}, {"+0430", "<+0430>-4:30"}, {"+0500", "<+05>-5"}, {"+0530", "<+0530>-5:30"}, {"+0600", "<+06>-6"}, {"+0630", "<+0630>-6:30"}, {"+0700", "<+07>-7"}, {"+0730", "<+0730>-7:30"}, {"+0800", "<+08>-8"}, {"+0830", "<+0830>-8:30"}, {"+0900", "<+09>-9"}, {"+0930", "<+0930>-9:30"}, {"+1000", "<+10>-10"}, {"+1030", "<+1030>-10:30"}, {"+1100", "<+11>-11"}, {"+1130", "<+1130>-11:30"}, {"+1200", "<+12>-12"}, {"+1230", "<+1230>-12:30"}, {"+1300", "<+13>-13"}, {"+1330", "<+1330>-13:30"}, {"+1400", "<+14>-14"}};
int timezone = 25;

int leftLDRValue = 0;
int rightLDRValue = 0;
float maxIntensity = 0.0;
float l_LDR = 0.0;
float R_LDR = 0.0;

char tempAr[6];
char humidAr[6];

// LDR Characteristics
const float GAMMA_LDR = 0.7;
const float RL10 = 50;

// Functions
void clear_top_half() // clear top half of the oled display
{
  display.fillRect(0, 0, display.width(), display.height() / 2, BLACK);
}

void clear_bottom_half() // clear bottom half of the oled display
{
  display.fillRect(0, display.height() / 2, display.width(), display.height() / 2, BLACK);
}

void print_line(String text, int text_size, int row, int column)
{
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);

  display.display();
  // delay(2000);
}

void update_time(void) // update time when not using NTP server
{
  timeNow = millis() / 1000;
  seconds = timeNow - timeLast;

  if (seconds > 59)
  {
    timeLast += 60;
    minutes += 1;
    seconds = 0;
  }

  if (minutes >= 60)
  {
    minutes = 0;
    hours += 1;
  }

  if (hours == 24)
  {
    hours = 0;
    days += 1;

    for (int i = 0; i < n_alarms; i++)
    {
      alarm_triggered[i] = false;
    }
  }
}

void update_time_wifi(void) // update time when connected to the NTP server
{
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char day_str[8];
  char hour_str[8];
  char min_str[8];
  char sec_str[8];
  strftime(day_str, 8, "%d", &timeinfo);
  strftime(sec_str, 8, "%S", &timeinfo);
  strftime(hour_str, 8, "%H", &timeinfo);
  strftime(min_str, 8, "%M", &timeinfo);

  hours = atoi(hour_str);
  minutes = atoi(min_str);
  days = atoi(day_str);
  seconds = atoi(sec_str);
}

void print_time_now(void)
{
  print_line(String(days) + ":" + String(hours) + ":" + String(minutes) + ":" + String(seconds), 2, 0, 0);
}

void ring_alarm()
{
  display.clearDisplay();

  while (cancelButtonPressed == false)
  {
    print_line("Medicine Time", 2, 0, 0);

    alarm_ringing = true;

    for (int i = 0; i < n_notes; i++)
    {
      if (cancelButtonPressed == true)
      {
        delay(200);
        break;
      }
      // Toggle LED state
      digitalWrite(LED_1, i % 2);

      tone(BUZZER, notes[i]);
      delay(500);
      noTone(BUZZER);
      delay(2);
    }
  }
  delay(200);
  digitalWrite(LED_1, LOW);
  alarm_ringing = false;
  cancelButtonPressed = false;
}

void update_time_with_check_alarm()
{
  clear_top_half();

  if (time_mode)
  {
    update_time_wifi();
  }
  else
  {
    update_time();
  }

  print_time_now();

  if (alarm_enabled)
  {
    for (int i = 0; i < n_alarms; i++)
    {
      if (alarm_triggered[i] == false && hours == alarm_hours[i] && minutes == alarm_minutes[i])
      {
        ring_alarm();
        alarm_triggered[i] = true;
      }
    }
  }
}

int wait_for_button_press()
{
  while (true)
  {
    if (digitalRead(UP) == LOW)
    {
      delay(200);
      return UP;
    }

    else if (digitalRead(DOWN) == LOW)
    {
      delay(200);
      return DOWN;
    }

    else if (digitalRead(CANCEL) == LOW)
    {
      delay(200);
      return CANCEL;
    }

    else if (digitalRead(OK) == LOW)
    {
      delay(200);
      return OK;
    }

    update_time();
  }
}

void set_time_Auto() // select time zone
{

  int temp_timezone = timezone;

  while (true)
  {
    display.clearDisplay();
    print_line("Select Time Zone", 2, 0, 1);
    print_line(time_zones[temp_timezone][0], 2, 35, 2);
    int pressed = wait_for_button_press();
    if (pressed == UP)
    {
      delay(200);
      temp_timezone += 1;
      temp_timezone = temp_timezone % 97;
    }
    else if (pressed == DOWN)
    {
      delay(200);
      temp_timezone -= 1;
      if (temp_timezone < 0)
      {
        temp_timezone = 96;
      }
    }
    else if (pressed == OK)
    {
      delay(200);
      timezone = temp_timezone;
      break;
    }
    else if (pressed == CANCEL)
    {
      delay(200);
      return;
    }
  }
  setenv("TZ", time_zones[timezone][1].c_str(), 1);
  tzset();
}

void set_time_manual() // configure time manually
{
  int temp_day = days;
  while (true)
  {
    display.clearDisplay();
    print_line("Enter day: " + String(temp_day), 2, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == UP)
    {
      delay(200);
      temp_day += 1;
      temp_day = temp_day % 31; // Assuming a month with 31 days
    }
    else if (pressed == DOWN)
    {
      delay(200);
      temp_day -= 1;
      if (temp_day < 0)
      {
        temp_day = 30; // Assuming a month with 31 days
      }
    }
    else if (pressed == OK)
    {
      delay(200);
      break;
    }
    else if (pressed == CANCEL)
    {
      delay(200);
      break;
    }
  }

  int temp_hour = hours;
  while (true)
  {
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 2, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == UP)
    {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }

    else if (pressed == DOWN)
    {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0)
      {
        temp_hour = 23;
      }
    }
    else if (pressed == OK)
    {
      delay(200);
      break;
    }
    else if (pressed == CANCEL)
    {
      delay(200);
      break;
    }
  }

  int temp_minute = minutes;
  while (true)
  {
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 2, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == UP)
    {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }
    else if (pressed == DOWN)
    {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0)
      {
        temp_minute = 59;
      }
    }
    else if (pressed == OK)
    {
      delay(200);
      minutes = temp_minute;
      hours = temp_hour;
      days = temp_day;
      display.clearDisplay();
      print_line("Time is set", 2, 0, 2);
      break;
    }
    else if (pressed == CANCEL)
    {
      delay(200);
      break;
    }
  }

  delay(1000);
}

void run_mode_time(int mode) // function for runing selected option in the sub menu within "set_time"
{
  if (mode == 0)
  {
    set_time_Auto();
    time_mode = true;
  }

  else if (mode == 1)
  {
    set_time_manual();
    time_mode = false;
  }
}

void set_time()
{

  while (okButtonPressed)
  {
    display.clearDisplay();
    print_line(time_options[time_mode_current], 2, 0, 0);

    int pressed = wait_for_button_press();

    if (pressed == UP)
    {
      time_mode_current += 1;
      time_mode_current %= max_time_modes;
      delay(200);
    }

    else if (pressed == DOWN)
    {
      time_mode_current -= 1;
      if (time_mode_current < 0)
      {
        time_mode_current = max_time_modes - 1;
      }
      delay(200);
    }

    else if (pressed == OK)
    {
      Serial.println(time_mode_current);
      delay(200);
      run_mode_time(time_mode_current);
    }

    else if (pressed == CANCEL)
    {
      cancelButtonPressed = false;
      break;
    }
  }
}

void set_alarm(int alarm)
{
  int temp_hour = alarm_hours[alarm];
  while (true)
  {
    display.clearDisplay();
    print_line("Enter hour:" + String(temp_hour), 2, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == UP)
    {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }

    else if (pressed == DOWN)
    {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0)
      {
        temp_hour = 23;
      }
    }
    else if (pressed == OK)
    {
      delay(200);
      break;
    }
    else if (pressed == CANCEL)
    {
      delay(200);
      break;
    }
  }

  int temp_minute = alarm_minutes[alarm];
  while (true)
  {
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 2, 0, 2);
    int pressed = wait_for_button_press();
    if (pressed == UP)
    {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }

    else if (pressed == DOWN)
    {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0)
      {
        temp_minute = 59;
      }
    }
    else if (pressed == OK)
    {
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      alarm_hours[alarm] = temp_hour;
      display.clearDisplay();
      print_line("Alarm is set", 2, 0, 2);
      break;
    }
    else if (pressed == CANCEL)
    {
      delay(200);
      break;
    }
  }

  delay(1000);
}

void run_mode(int mode) // function for runing selected option in the main menu
{
  if (mode == 0)
  {
    set_time();
  }

  else if (mode == 1 || mode == 2 || mode == 3)
  {
    set_alarm(mode - 1);
  }

  else if (mode == 4)
  {
    alarm_enabled = false;
    digitalWrite(LED_3, LOW);
  }
}

void go_to_menu(void) // function for main menu
{
  while (okButtonPressed)
  {
    display.clearDisplay();
    print_line(options[current_mode], 2, 0, 0);

    int pressed = wait_for_button_press();

    if (pressed == UP)
    {
      current_mode += 1;
      current_mode %= max_modes;
      delay(200);
    }

    else if (pressed == DOWN)
    {
      current_mode -= 1;
      if (current_mode < 0)
      {
        current_mode = max_modes - 1;
      }
      delay(200);
    }

    else if (pressed == OK)
    {
      Serial.println(current_mode);
      run_mode(current_mode);
    }

    else if (pressed == CANCEL)
    {
      cancelButtonPressed = false;
      break;
    }
  }
}

void check_temp(void) // function for DHT22 sensor data
{
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool all_good = true;
  String warnings = "";

  if (data.temperature > 35)
  {
    all_good = false;
    warnings += "TEMP HIGH\n\n";
  }
  else if (data.temperature < 25)
  {
    all_good = false;
    warnings += "TEMP LOW\n\n";
  }

  if (data.humidity > 85)
  {
    all_good = false;
    warnings += "HUMD HIGH\n\n";
  }
  else if (data.humidity < 35)
  {
    all_good = false;
    warnings += "HUMD LOW\n\n";
  }

  clear_bottom_half();

  if (!all_good)
  {
    digitalWrite(LED_2, HIGH);
    print_line(warnings, 1, 40, 0); // Adjust the position as needed
  }
  else
  {
    digitalWrite(LED_2, LOW);
  }

  String(data.temperature, 2).toCharArray(tempAr, 6);
  String(data.humidity, 2).toCharArray(humidAr, 6);
}

void IRAM_ATTR okInterrupt()
{
  okButtonPressed = true;
}

void IRAM_ATTR cancelInterrupt()
{
  // Check if either go_to_menu() function is running or alarm is ringing
  if (okButtonPressed || alarm_ringing)
  {
    cancelButtonPressed = true;
  }
}

void IRAM_ATTR alarmInterrupt()
{
  // Check if enough time has passed since the last button press
  if (millis() - lastDebounceTime > debounceDelay)
  {
    // Update the last debounce time
    lastDebounceTime = millis();

    // Toggle the alarm_enabled variable
    if (alarm_enabled)
    {
      digitalWrite(LED_3, LOW);
      alarm_enabled = false;
    }
    else
    {
      digitalWrite(LED_3, HIGH);
      alarm_enabled = true;
    }
  }
}

void measureLightIntensity()
{
  leftLDRValue = analogRead(LEFT_LDR_PIN);
  rightLDRValue = analogRead(RIGHT_LDR_PIN);

  // Calculate normalized LDR values
  float l_LDR = 1.0 - ((leftLDRValue - 32.0) / (4063.0 - 32.0));
  float r_LDR = 1.0 - ((rightLDRValue - 32.0) / (4063.0 - 32.0));

  // Determine max intensity and publish
  if (l_LDR > r_LDR)
  {
    maxIntensity = l_LDR;
    mqttClient.publish("ADMIN-INTENSITY", String(maxIntensity).c_str());
    mqttClient.publish("ADMIN-LDR", "LEFT");
  }
  else if (r_LDR > l_LDR)
  {
    maxIntensity = r_LDR;
    mqttClient.publish("ADMIN-INTENSITY", String(maxIntensity).c_str());
    mqttClient.publish("ADMIN-LDR", "RIGHT");
  }
  else
  {
    maxIntensity = l_LDR;
    mqttClient.publish("ADMIN-INTENSITY", String(maxIntensity).c_str());
    mqttClient.publish("ADMIN-LDR", "BOTH");
  }
}

void adjustShadedWindow(float maxIntensity)
{
  // Calculate motor angle based on max intensity
  float D = (l_LDR > R_LDR) ? 1.5 : 0.5;
  float theta = min(float(THETA_OFFSET[tablet] * D + (180.0f - THETA_OFFSET[tablet]) * maxIntensity * GAMMA[tablet]), 180.0f);

  // Move servo to calculated angle
  servoMotor.write(theta);
}

void alarmOn(bool on)
{
  if (on)
  {
    alarm_enabled = true;
    digitalWrite(LED_3, HIGH);
  }
  else
  {
    alarm_enabled = false;
    digitalWrite(LED_3, LOW);
  }
}

void medicineSelect(char medicine)
{
  if (medicine == 'A')
  {
    tablet = 0;
  }
  else if (medicine == 'B')
  {
    tablet = 1;
  }
  else if (medicine == 'C')
  {
    tablet = 2;
  }
  else if (medicine == 'D')
  {
    tablet = 3;
  }
}

void setupWifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println("Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WiFi", 2, 0, 0); // (String, text_size, cursor_row, cursor_column)
    Serial.print(".");
  }

  display.clearDisplay();
  print_line("Connected to WiFi", 2, 0, 0);

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void recieveCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");

  char payloadCharAr[length];
  for (int i = 0; i < length; i++)
  {
    // Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];
  }

  if (strcmp(topic, "ADMIN-ALARM-ENABLER") == 0)
  {
    alarmOn(payloadCharAr[0] == '1');
  }
  else if (strcmp(topic, "ADMIN-MEDICINE") == 0)
  {
    medicineSelect(payloadCharAr[0]);
    Serial.println(tablet);
  }
  else if (strcmp(topic, "ADMIN-MINANGLE") == 0)
  {
    if (tablet == 3)
    {
      String payloadValue = String(payloadCharAr);
      float floatValue = atof(payloadValue.substring(0, payloadValue.indexOf('.') + 3).c_str());
      THETA_OFFSET[3] = floatValue;
      Serial.println(THETA_OFFSET[3]);
    }
  }
  else if (strcmp(topic, "ADMIN-CONFACTOR") == 0)
  {
    if (tablet == 3)
    {
      String payloadValue = String(payloadCharAr);
      float floatValue = atof(payloadValue.substring(0, payloadValue.indexOf('.') + 3).c_str());
      GAMMA[3] = floatValue;
      Serial.println(GAMMA[3]);
    }
  }
}

void setupMqtt()
{
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(recieveCallback);
}

void connectToBroker()
{
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32-123456789"))
    {
      Serial.println("Connected");
      // Subscribe functions
      mqttClient.subscribe("ADMIN-ALARM-ENABLER");
      mqttClient.subscribe("ADMIN-MEDICINE");
      mqttClient.subscribe("ADMIN-MINANGLE");
      mqttClient.subscribe("ADMIN-CONFACTOR");
    }
    else
    {
      Serial.println("failed ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(CANCEL, INPUT);
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(OK, INPUT);
  pinMode(ALARM, INPUT);
  pinMode(LEFT_LDR_PIN, INPUT);
  pinMode(RIGHT_LDR_PIN, INPUT);

  servoMotor.attach(SERVO_PIN);

  dhtSensor.setup(DHT, DHTesp::DHT22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  print_line("Welcome to Medibox", 2, 0, 0); // (String, text_size, cursor_row, cursor_column)
  delay(3000);

  setupWifi();

  setupMqtt();

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  delay(1000);

  pinMode(OK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(OK), okInterrupt, FALLING);

  pinMode(CANCEL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CANCEL), cancelInterrupt, FALLING);

  pinMode(ALARM, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ALARM), alarmInterrupt, RISING);
}

void loop()
{

  if (!mqttClient.connected())
  {
    connectToBroker();
  }
  mqttClient.loop();

  if (okButtonPressed)
  {
    delay(1000); // Add a small delay to avoid multiple interrupts
    Serial.println("Menu");
    go_to_menu();
    okButtonPressed = false;
  }

  check_temp();
  update_time_with_check_alarm();

  measureLightIntensity();
  adjustShadedWindow(maxIntensity);

  mqttClient.publish("ADMIN-TEMP", tempAr);
  mqttClient.publish("ADMIN-HUMID", humidAr);
}
