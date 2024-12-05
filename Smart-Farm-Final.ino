//กำหนด TOKEN BLYNK
#define BLYNK_TEMPLATE_ID "TMPL6z6a8Bjrq"
#define BLYNK_TEMPLATE_NAME "Smart Farm"
#define BLYNK_AUTH_TOKEN "Tw88N8tsfgaEte7111YgWfhrsVKuREDx"

#define BLYNK_PRINT Serial

// นำเข้าไลบรารี
// #include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>

// // Replace with your network credentials
// const char* ssid = "OPPO Reno10 5G";
// const char* password = "11111111";

//ตัวแปรเก็บเวลาเริ่มต้น-สิ้นสุด
bool pump_set[3];
long timer_start_set[3] = { 0xFFFF, 0xFFFF };
long timer_stop_set[3] = { 0xFFFF, 0xFFFF };
unsigned char weekday_set[3];

long rtc_sec;
unsigned char day_of_week;

bool pump_status[3];
bool update_blynk_status[3];
bool pump_timer_on_set[3];

// เซนเซอร์วัดความชื้น 1-3
#define SENSOR_1_PIN 32
#define SENSOR_2_PIN 33
#define SENSOR_3_PIN 34

// ปั๊มน้ำ 1-3
#define RELAY_1_PIN 25
#define RELAY_2_PIN 26
#define RELAY_3_PIN 27

// ตัวแปรเก็บค่าความชื้น 1-3 ใน BLYNK
#define VIRTUAL_MOISTURE_1 V0
#define VIRTUAL_MOISTURE_2 V1
#define VIRTUAL_MOISTURE_3 V2

// ปุ่มเปิด-ปิดปั้๊มบน BLYNK
#define VIRTUAL_RELAY_1 V3
#define VIRTUAL_RELAY_2 V4
#define VIRTUAL_RELAY_3 V5

// ตัวตั้งเวลาบน BLYNK
#define VIRTUAL_TIMER_1 V6
#define VIRTUAL_TIMER_2 V7
#define VIRTUAL_TIMER_3 V8

// หลอด LED ใน BLYNK
#define VIRTUAL_LED_1 V9
#define VIRTUAL_LED_2 V10
#define VIRTUAL_LED_3 V11

BlynkTimer timer;

// ตัวแปรเก็บค่าความชื้นในดิน
int moisture1, moisture2, moisture3;

// BLYNK คุมปั๊มน้ำตัวที่1
BLYNK_WRITE(VIRTUAL_RELAY_1) {
  int val = param.asInt();

  if (pump_timer_on_set[0] == 0)
    pump_set[0] = val;
  else
    update_blynk_status[0] = 1;
}

// BLYNK คุมปั๊มน้ำตัวที่2
BLYNK_WRITE(VIRTUAL_RELAY_2) {
  int val = param.asInt();

  if (pump_timer_on_set[1] == 0)
    pump_set[1] = val;
  else
    update_blynk_status[1] = 1;
}

// BLYNK คุมปั๊มน้ำตัวที่3
BLYNK_WRITE(VIRTUAL_RELAY_3) {
  int val = param.asInt();

  if (pump_timer_on_set[2] == 0)
    pump_set[2] = val;
  else
    update_blynk_status[1] = 1;
}

// ตั้งค่าเวลาเริ่มต้น-สิ้นสุดปั๊มน้ำตัวที่1
BLYNK_WRITE(VIRTUAL_TIMER_1) {
  unsigned char week_day;

  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime()) {
    timer_start_set[0] = (t.getStartHour() * 60 * 60) + (t.getStartMinute() * 60);
    timer_stop_set[0] = (t.getStopHour() * 60 * 60) + (t.getStopMinute() * 60);

    Serial.println(String("Start Time: ") + t.getStartHour() + ":" + t.getStartMinute());

    Serial.println(String("Stop Time: ") + t.getStopHour() + ":" + t.getStopMinute());

    for (int i = 1; i <= 7; i++) {
      if (t.isWeekdaySelected(i)) {
        week_day |= (0x01 << (i - 1));
        Serial.println(String("Day ") + i + " is selected");
      } else {
        week_day &= (~(0x01 << (i - 1)));
      }
    }

    weekday_set[0] = week_day;
  } else {
    timer_start_set[0] = 0xFFFF;
    timer_stop_set[0] = 0xFFFF;
  }
}

// ตั้งค่าเวลาเริ่มต้น-สิ้นสุดปั๊มน้ำตัวที่2
BLYNK_WRITE(VIRTUAL_TIMER_2) {
  unsigned char week_day;

  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime()) {
    timer_start_set[1] = (t.getStartHour() * 60 * 60) + (t.getStartMinute() * 60);
    timer_stop_set[1] = (t.getStopHour() * 60 * 60) + (t.getStopMinute() * 60);

    Serial.println(String("Start Time: ") + t.getStartHour() + ":" + t.getStartMinute());

    Serial.println(String("Stop Time: ") + t.getStopHour() + ":" + t.getStopMinute());

    for (int i = 1; i <= 7; i++) {
      if (t.isWeekdaySelected(i)) {
        week_day |= (0x01 << (i - 1));
        Serial.println(String("Day ") + i + " is selected");
      } else {
        week_day &= (~(0x01 << (i - 1)));
      }
    }

    weekday_set[1] = week_day;
  } else {
    timer_start_set[1] = 0xFFFF;
    timer_stop_set[1] = 0xFFFF;
  }
}

// ตั้งค่าเวลาเริ่มต้น-สิ้นสุดปั๊มน้ำตัวที่3
BLYNK_WRITE(VIRTUAL_TIMER_3) {
  unsigned char week_day;

  TimeInputParam t(param);

  if (t.hasStartTime() && t.hasStopTime()) {
    timer_start_set[2] = (t.getStartHour() * 60 * 60) + (t.getStartMinute() * 60);
    timer_stop_set[2] = (t.getStopHour() * 60 * 60) + (t.getStopMinute() * 60);

    Serial.println(String("Start Time: ") + t.getStartHour() + ":" + t.getStartMinute());

    Serial.println(String("Stop Time: ") + t.getStopHour() + ":" + t.getStopMinute());

    for (int i = 1; i <= 7; i++) {
      if (t.isWeekdaySelected(i)) {
        week_day |= (0x01 << (i - 1));
        Serial.println(String("Day ") + i + " is selected");
      } else {
        week_day &= (~(0x01 << (i - 1)));
      }
    }

    weekday_set[2] = week_day;
  } else {
    timer_start_set[2] = 0xFFFF;
    timer_stop_set[2] = 0xFFFF;
  }
}

//แปลงเวลาให้เป็นปัจจุบัน
BLYNK_WRITE(InternalPinRTC) {
  const unsigned long DEFAULT_TIME = 1357041600;  // Jan 1 2013
  unsigned long blynkTime = param.asLong();

  if (blynkTime >= DEFAULT_TIME) {
    setTime(blynkTime);

    day_of_week = weekday();

    if (day_of_week == 1)
      day_of_week = 7;
    else
      day_of_week -= 1;

    rtc_sec = (hour() * 60 * 60) + (minute() * 60) + second();

    Serial.println(blynkTime);
    Serial.println(String("RTC Server: ") + hour() + ":" + minute() + ":" + second());
    Serial.println(String("Day of Week: ") + weekday());
  }
}

//เมื่อเชื่อมต่อ BLYNK สำเร็จ
BLYNK_CONNECTED() {
  //ซิงค์เวลากับค่าจาก BLYNK
  // rtc.begin();
  Blynk.sendInternal("rtc", "sync");
  Blynk.syncAll();
}

// เช็คเวลา
void checkTime() {
  Blynk.sendInternal("rtc", "sync");
}

// จัดการสถานะปั๊มน้ำ
void pump_mng() {
  bool time_set_overflow;
  bool pump_status_buf[3];

  for (int i = 0; i < 3; i++) {
    pump_status_buf[i] = pump_status[i];
    time_set_overflow = 0;

    if (timer_start_set[i] != 0xFFFF && timer_stop_set[i] != 0xFFFF) {
      if (timer_stop_set[i] < timer_start_set[i]) time_set_overflow = 1;

      if ((((time_set_overflow == 0 && (rtc_sec >= timer_start_set[i]) && (rtc_sec < timer_stop_set[i])) || (time_set_overflow && ((rtc_sec >= timer_start_set[i]) || (rtc_sec < timer_stop_set[i])))) && (weekday_set[i] == 0x00 || (weekday_set[i] & (0x01 << (day_of_week - 1)))))) {
        pump_timer_on_set[i] = 1;
      } else
        pump_timer_on_set[i] = 0;
    } else
      pump_timer_on_set[i] = 0;

    if (pump_timer_on_set[i]) {
      pump_status[i] = 1;
      pump_set[i] = 0;
    } else {
      pump_status[i] = pump_set[i];
    }

    if (pump_status_buf[i] != pump_status[i])
      update_blynk_status[i] = 1;
  }
  // HARDWARE CONTROL
  digitalWrite(RELAY_1_PIN, pump_status[0]);
  digitalWrite(RELAY_2_PIN, pump_status[1]);
  digitalWrite(RELAY_3_PIN, pump_status[2]);

  Blynk.virtualWrite(VIRTUAL_LED_1, pump_status[0]);
  Blynk.virtualWrite(VIRTUAL_LED_2, pump_status[1]);
  Blynk.virtualWrite(VIRTUAL_LED_3, pump_status[2]);
}

// อัพเดทสถานะปั๊มน้ำ
void blynk_update() {
  if (update_blynk_status[0]) {
    update_blynk_status[0] = 0;
    Blynk.virtualWrite(RELAY_1_PIN, pump_status[0]);
  }

  if (update_blynk_status[1]) {
    update_blynk_status[1] = 0;
    Blynk.virtualWrite(RELAY_2_PIN, pump_status[1]);
  }

  if (update_blynk_status[2]) {
    update_blynk_status[2] = 0;
    Blynk.virtualWrite(RELAY_3_PIN, pump_status[2]);
  }
}

// อ่านค่าความชื้นส่งไปที่ BLYNK
void readMoistureSensors() {
  moisture1 = analogRead(SENSOR_1_PIN);
  moisture2 = analogRead(SENSOR_2_PIN);
  moisture3 = analogRead(SENSOR_3_PIN);

  Blynk.virtualWrite(VIRTUAL_MOISTURE_1, map(moisture1, 0, 4095, 0, 100));
  Blynk.virtualWrite(VIRTUAL_MOISTURE_2, map(moisture2, 0, 4095, 0, 100));
  Blynk.virtualWrite(VIRTUAL_MOISTURE_3, map(moisture3, 0, 4095, 0, 100));
}

void setup() {
  Serial.begin(115200);

  WiFiManager wm;

  bool res;

  res = wm.autoConnect("Smart Farm", "123456789");

  if (!res) {
    Serial.println("Failed to connect");
    ESP.restart();
  } else {
    Serial.println("connected...yeey :)");
  }

  // เชื่อมต่อ BLYNK
  Blynk.config(BLYNK_AUTH_TOKEN);

  // ตั้งค่า PINS เซนเซอร์วัดความชื้น
  pinMode(SENSOR_1_PIN, INPUT);
  pinMode(SENSOR_2_PIN, INPUT);
  pinMode(SENSOR_3_PIN, INPUT);

  // ตั้งค่า PINS ปั๊มน้ำ
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(RELAY_3_PIN, OUTPUT);

  // ปิดปั๊มน้ำเมื่อเริ่มทำงาน
  digitalWrite(RELAY_1_PIN, HIGH);  // Relays off by default
  digitalWrite(RELAY_2_PIN, HIGH);
  digitalWrite(RELAY_3_PIN, HIGH);

  setSyncInterval(10 * 60);

  timer.setInterval(60000L, readMoistureSensors); //อัพเดตค่าความชื้นทุก 1 นาที
  timer.setInterval(60000L, checkTime); //อัพเดตเวลาทุก 1 นาที
}

void loop() {
  Blynk.run();
  timer.run();
  pump_mng();
  blynk_update();
}
