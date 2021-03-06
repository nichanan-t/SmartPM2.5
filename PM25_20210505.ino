/********************************************************/
/*** ส่วนของการกำหนดค่าคงที่ที่ใช้ในโปรแกรม ***/
/*******************************************************/
#define BLYNK_PRINT Serial
  // กำหนดให้แสดงผลของ BLYNK ทาง Serial
  
#define filter D5   
  // กำหนดให้ชื่อ filter แทน pin D5 เพื่อใช้เป็นขาเอาท์พุต ในการควบคุมการทำงานของ
  // เครื่องกำจัดฝุ่นละออง (จำลองโดยใช้มอเตอร์พัดลมดูดอากาศ)
  
#define DHTPIN D7
  // กำหนดให้ชื่อ DHTPIN แทน pin D7 เพื่อใช้เป็นขาอินพุตรับสัญญาณจากเซนเซอร์ DHT22 
  // ในการตรวจวัดค่าอุณภูมิ
  
#define DHTTYPE DHT22 
  // กำหนดให้ชื่อ DHTTYPE แทนค่า DHT22 เพื่อกำหนดชนิดของตัวเซนเซอร์ที่ใช้วัดค่าอุณหภูมิ
  
#define HOTSPOT    "/** Hotspot name **/"
  // กำหนดค่า hotspot (ชื่อ hotspot ที่ต้องการใช้งาน)
  
#define PASSWORD    "/** Hotspot Password **/"
  // กำหนดค่ารหัสผ่าน hotspot ที่ต้องการใช้งาน
  
#define LINE_TOKEN "/** LINE Token **/"
  // กำหนด Token สำหรับการใช้งาน Line Notify

/***************************************************************************************/
/*** กำหนดการผนวกเฮดเดอร์ไฟล์ (Header Files) เข้ามาใช้งานในโปรแกรม ***/
/**************************************************************************************/
#include "DHT.h"
  // ผนวกไฟล์กำหนดการใช้งานฟังก์ชั่นและค่าต่างๆ ที่เกี่ยวกับเซนเซอร์ DHT
  
#include <ESP8266WiFi.h>
  // ผนวกไฟล์กำหนดการใช้งานฟังก์ชั่นและค่าต่างๆ ที่เกี่ยวกับการใช้ WiFi ของ ESP8266
  
#include <BlynkSimpleEsp8266.h>
  // ผนวกไฟล์กำหนดการใช้งานฟังก์ชั่นและค่าต่างๆ ที่เกี่ยวกับการใช้ Blynk
  
#include <LiquidCrystal_I2C.h>
  // ผนวกไฟล์กำหนดการใช้งานฟังก์ชั่นและค่าต่างๆ ที่เกี่ยวกับการใช้งาน LCD แบบ I2C
  
#include <TridentTD_LineNotify.h>
  // ผนวกไฟล์กำหนดการใช้งานฟังก์ชันและค่าต่างๆ ที่เกี่ยวกับการใช้งาน Line Notify

/*************************************************************/
/*** กำหนดค่าเริ่มต้นและตัวแปรที่ใช้งานในโปรแกรม ***/
/************************************************************/
DHT dht(DHTPIN, DHTTYPE);
  // กำหนดค่าเริ่มต้นการใช้งาน LCD ตามที่ระบุค่าไว้
  
LiquidCrystal_I2C lcd(0x27, 16, 2); 
  // กำหนดคุณลักษณะของ LCD เป็นแบบ 16 ตัวอักษร 2 บรรทัด
  
char AUTH[] = "/** Blynk Authentication code **/";
  // กำหนดรหัสที่ใช้สำหรับการเข้าใช้งานแอปพลิเคชัน Blynk
  
int measurePin = A0;
  // กำหนดให้ตัวแปร measurePin ใช้แทนขา A0 (Analog Pin) เพื่อรับสัญญาณจากเซนเซอร์ 
  // sharp GP2Y1014AU0F ในการตรวจวัดค่าฝุ่นละออง
  
int ledPower = D4;    //Pin LED
  // กำหนดให้ตัวแปร ledPower ใช้แทนขา D4 (Digital Output Pin) เพื่อส่งสัญญาณไปยังเซนเซอร์ 
  // sharp GP2Y1014AU0F ในการจ่ายพลังงานให้กับ LED เพื่อตรวจวัดค่าฝุ่นละออง
  
int samplingTime = 280;
  // กำหนดค่าจังหวะเวลาในการตรวจวัดค่าฝุ่นละอองของเซนเซอร์ sharp GP2Y1014AU0F
  
int deltaTime = 40;
  // กำหนดค่าระยะเวลาในการทำงานตรวจวัดค่าฝุ่นละอองของเซนเซอร์ sharp GP2Y1014AU0F
  
int sleepTime = 9680;
  // กำหนดค่าช่วงเวลาพักหรือหยุดทำงานในการตรวจวัดค่าฝุ่นละอองของเซนเซอร์ 
  // sharp GP2Y1014AU0F
  
double voMeasured = 0.0;
  // กำหนดตัวแปร voMeasured เพื่อใช้เก็บค่าแรงดันไฟฟ้าที่อ่านได้จากการตรวจวัดค่าฝุ่นละออง
  // ของเซนเซอร์ sharp GP2Y1014AU0F
  
double calcVoltage = 0.0;
  // กำหนดตัวแปร calcVoltage เพื่อใช้เก็บค่าผลลัพธ์ที่ได้การประมวลผลค่าแรงดัน voMeasured
  
double pm = 0.0;
  // กำหนดตัวแปร pm เพื่อใช้เก็บผลลัพธ์จากการคำนวณค่าฝุ่นละออง PM2.5
  
float t = 0.0;
  // กำหนดตัวแปร t เป็นชนิด float (ทศนิยม) เพื่อใช้เก็บค่าอุณหภูมิที่อ่านได้จากเซนเซอร์ DHT22 

float h = 0.0;
  /* กำหนดตัวแปร h เป็นชนิด float (ทศนิยม) เพื่อใช้เก็บค่าความชื้นในอากาศที่อ่านได้จากเซนเซอร์ DHT22 */ 
  
WidgetLED fan(V1);
  // กำหนดให้ WidgetLED ที่กำหนดในโปรแกรม Blynk มีชื่อว่า fan และช่องติดต่อที่ V1

int count = 0;
  // กำหนดตัวแปร count สำหรับการนับจำนวนรอบเพื่อแจ้งเตือนบน Line Notify
/**************************************************************/
/*** ส่วนของค่าเริ่มต้นโปรแกรม หรือฟังก์ชั่น setup()  ***/
/*************************************************************/
void setup() 
{
  Serial.begin(9600);
    // กำหนดความเร็วในการรับส่งข้อมูลของ Serial COM Port กับบอร์ด NodeMCU ESP8266

  Serial.println(LINE.getVersion());
    // แสดงค่าเวอร์ชันของ LINE
  
  Blynk.begin(AUTH, HOTSPOT, PASSWORD, "blynk.en-26.com", 9600);  
    // กำหนดการเชื่อมต่อของบอร์ด NodeMCU ESP8266 กับเครือข่ายอินเตอร์เน็ตที่กำหนด
    
  pinMode(ledPower, OUTPUT);
    // กำหนดให้ ledPower หรือขา D4 ที่กำหนดไว้ก่อนหน้าเป็นขาเอาท์พุต

  pinMode(filter, OUTPUT);
    // กำหนดให้ filter หรือขา D5 ที่กำหนดไว้ก่อนหน้าเป็นขาเอาท์พุต

  dht.begin();
    // เริ่มต้นใช้งานเซนเซอร์ DHT22
    
  lcd.begin();
    // เริ่มต้นใช้งาน LCD 16x2 I2C
    
  lcd.backlight();       
  // เปิดใช้งานไฟ backlight ที่ด้านหลังของตัว LCD

  LINE.setToken(LINE_TOKEN);
  // กำหนดค่า Token LINE ตามค่าตัวแปร LINE_TOKEN

  WiFi.begin(HOTSPOT, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", HOTSPOT);
  
  while(WiFi.status() != WL_CONNECTED) 
  { 
    Serial.print("."); delay(400); 
  }
  
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP()); 
}

/********************************************************************************/
/*** ส่วนของคำสั่งควบคุมการทำงานของโปรแกรม หรือฟังก์ชั่น loop()  ***/
/********************************************************************************/
void loop() 
{
  t = dht.readTemperature();
    // กำหนดตัวแปร t รับค่าอุณหภูมิ (C) จากเซนเซอร์ DHT22

  h = dht.readHumidity();
    // กำหนดตัวแปร h รับค่าความชื้นสัมพัทธ์ในอากาศ (%) จากเซนเซอร์ DHT22
    
  digitalWrite(ledPower, LOW); 
    // กำหนดให้ LED ที่ตัวเซนเซอร์ sharp GP2Y1014AU0F ทำงาน เพื่อตรวจวัดค่าฝุ่นละออง

  delayMicroseconds(samplingTime);
    // กำหนดจังหวะช่วงเวลาของการตรวจวัดค่าฝุ่นละออง (Sampling Time)

  voMeasured = analogRead(measurePin); 
    // กำหนดให้ค่าสัญญาณแรงดันที่อ่านได้จากเซนเซอร์ sharp GP2Y1014AU0F 
    // ผ่านทางขา Pin A0 (Analog Pin) จัดเก็บไว้ที่ตัวแปร voMeasured
    
  delayMicroseconds(deltaTime);
    // กำหนดจังหวะช่วงเวลาของการทำงานของ LED ในการตรวจวัดค่าฝุ่นละออง (Delta Time)
    
  digitalWrite(ledPower, HIGH); 
    // กำหนดให้ LED ที่ตัวเซนเซอร์ sharp GP2Y1014AU0F หยุดทำงานหรือดับ
    
  delayMicroseconds(sleepTime);
    // กำหนดช่วงเวลาพักของเซนเซอร์ sharp GP2Y1014AU0F (Sleeping Time)
    
  calcVoltage = voMeasured * (3.3 / 1024.0);
    // คำนวณค่าแรงดันที่อ่านได้จากเซนเซอร์ sharp GP2Y1014AU0F
    
  pm = 0.3 * voMeasured;
    // คำนวณค่าฝุ่นละอองที่อ่านได้จากเซนเซอร์ sharp GP2Y1014AU0F


/*********************************************************************************************/
/*** การควบคุมการทำงานเครื่องฟอกอากาศ (จำลองด้วยพัดลมดูดอากาศ 5 โวลต์) ***/
/********************************************************************************************/
  if (pm <= 50.99)
  {
    digitalWrite(filter, HIGH);
    fan.off();
    count = count+1;

    if (count == 15 && pm <= 25.99)
    {
      LINE.notify("\n Temp = " +String(t) +" °C \n Humid = " +String(h) + " %");
      LINE.notify("\n PM 2.5 = " +String(pm)+ " µg/m³\n คุณภาพอากาศดีมาก\n เครื่องฟอกอากาศ : OFF");
      count = 0;
    }
    
    else if (count == 15 && pm >= 26.00 && pm <=37.99)
    {
      LINE.notify("\n Temp = " +String(t) +" °C \n Humid = " +String(h) + " %");
      LINE.notify("\n PM 2.5 = " +String(pm)+ " µg/m³\n คุณภาพอากาศดี\n เครื่องฟอกอากาศ : OFF");
      count = 0;
    }
  
    else if (count == 15 && pm >= 38.00 && pm <=50.99)
    {
      LINE.notify("\n Temp = " +String(t) +" °C \n Humid = " +String(h) + " %");
      LINE.notify("\n PM 2.5 = " +String(pm)+ " µg/m³\n คุณภาพอากาศปานกลาง\n เครื่องฟอกอากาศ : OFF");
      count = 0;
    }
  }

  else
  {
    digitalWrite(filter, LOW);
    fan.on();
    count = count+1;

    if (count == 15 && pm >= 51.00 && pm <= 90.99)
    {
      LINE.notify("\n Temp = " +String(t) +" °C \n Humid = " +String(h) + " %");
      LINE.notify("\n PM 2.5 = " +String(pm)+ " µg/m³\n เริ่มมีผลกระทบต่อสุขภาพ\n เครื่องฟอกอากาศ : ON \n ควรเฝ้าระวังสุขภาพ และหลีกเลี่ยงการทำกิจกรรมกลางแจ้ง");
      count = 0;
    }
  
    else if (count == 15 && pm >= 91.00)
    {
      LINE.notify("\n Temp = " +String(t) +" °C \n Humid = " +String(h) + " %");
      LINE.notify("\n PM 2.5 = " +String(pm)+ " µg/m³\n มีผลกระทบต่อสุขภาพ\n เครื่องฟอกอากาศ : ON \n โปรดสวมหน้ากากป้องกัน PM 2.5 และงดกิจกรรมกลางแจ้ง");
      count = 0;
    }
  }
  
    /* หากค่าฝุ่นละออง (pm) มีค่าตั้งแต่ 51 µg/m³ ขึ้นไป กำหนดให้สถานะของ D5 หรือ filter เป็น LOW เพื่อให้พัดลมทำงาน และแสดงสถานะของ fan บน Dash Board ของโปรแกรม Blynk ทำงาน */
    /* แต่หากค่าฝุ่นละออง PM 2.5 มีค่าลดลงไม่ถึง 50 mg กำหนดให้สถานะของ D5 หรือ filter เป็น HIGH เพื่อให้พัดลมหยุดทำงาน และสถานะของ fan บน Dash Board ของโปรแกรม Blynk ไม่ทำงาน */
/*************************************************************/
/*** ส่วนของการแสดงผลบนหน้าต่าง Serial Monitor ***/
/************************************************************/
  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);
  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);
  Serial.print(" - PM 2.5: ");
  Serial.print(pm);
  Serial.println(" µg/m³");
    // กำหนดให้แสดงค่าต่างๆ บนหน้าต่าง Serial Monitor เพื่อใช้สำหรับตรวจสอบการทำงาน

/***********************************************************************/
/*** ส่วนของการแสดงผลค่าฝุ่นละอองและอุณหภูมิบนจอ LCD ***/
/**********************************************************************/
  lcd.home();
    // กำหนดให้ LCD เริ่มต้นทำงาน
    
  lcd.setCursor(0, 0);
    // กำหนดให้เคอร์เซอร์อยู่ที่ตำแหน่งเริ่มต้น (บรรทัดแรกและคอลัมน์แรก)
    
  lcd.print("PM : ");
    // แสดงข้อความ PM:
    
  lcd.print(pm);
    // แสดงค่า PM 2.5
    
  lcd.print(" ug/m3 ");
    // แสดงข้อความ ug/m³(หน่วยไมโครกรัมต่อลูกบาศก์เมตร)
    
  lcd.setCursor(0, 1);
    // กำหนดให้เคอร์เซอร์อยู่ที่ตำแหน่งคอลัมน์แรกของบรรทัดที่สอง

  lcd.print("T:");
    // แสดงข้อความ T:
    
  lcd.print(t);
    // แสดงค่าอุณหภูมิจากตัวแปร t

 //lcd.print(" C");
    // แสดงข้อความ C (หน่วยองศาเซลเซียส)
  lcd.print(" H:");
    // แสดงข้อความ , H:

  lcd.print(h);
    // แสดงค่าความชื้นจากตัวแปร h

  //lcd.print(" %");
    // แสดงข้อความ % (หน่วยความชื้น (%))

/****************************************************************************/
/*** ส่วนของแสดงผลบนหน้าจอ Dash Board ของโปรแกรม Blynk ***/
/***************************************************************************/
  Blynk.run();
    // กำหนดให้ส่วนของโปรแกรม Blynk เริ่มต้นทำงาน
    
  Blynk.virtualWrite(V3,pm);
    // ส่งค่าฝุ่นละอองจากตัวแปร pm ไปแสดงผลที่ช่อง V3 บน Dash Board ของ Blynk

  Blynk.virtualWrite(V2,t);
    // ส่งค่าอุณหภูมิจากตัวแปร t ไปแสดงผลที่ช่อง V2 บน Dash Board ของ Blynk
    
  Blynk.virtualWrite(V4,h);
    // ส่งค่าอุณหภูมิจากตัวแปร h ไปแสดงผลที่ช่อง V4 บน Dash Board ของ Blynk
  
  delay(1000);
    // กำหนดการหน่วงเวลาของโปรแกรม
    // จบโปรแกรม
}
