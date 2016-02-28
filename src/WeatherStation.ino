#include <RTClib.h>
RTC_DS1307 rtc;

// библиотека для работы I²C
#include <Wire.h>
// библиотека для работы с модулями IMU
#include <troyka-imu.h>
// создаём объект для работы с барометром
Barometer barometer;
// библиотека для работы с датчиком DHT11
#include <TroykaDHT11.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define TEMP_PIN A0 // Аналоговый датчик температуры подключен к A0
unsigned long time;
float temperature;
float pressure;
uint8_t hum;

// создаём объект класса DHT11 и пераём номер пина к которому подкючён датчик
DHT11 dht(11);

void setup() {
  // открываем последовательный порт для мониторинга действий в программе
  Serial.begin(115200);
  dht.begin();
  barometer.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop() {
  if (millis() % 1000 == 0 || Serial.available() > 0)
  {
    Serial.read(); //читаем данные из порта
    // Временные костыли с подсчётом времени
    //printtime();
    DateTime now = rtc.now();

    // читаем значение абсолютного давления
    pressure = 0.0;
    for (int i = 0; i < 5; i++) {
      pressure += barometer.readPressureMillibars() / 1.3332;
    }
    pressure = pressure / 5;

    // читаем значение температуры окружающей среды
    temperature = 0.0;
    for (int i = 0; i < 5; i++) {
      temperature += barometer.readTemperatureC();
    }
    temperature = temperature / 5;

    // Вывод данных в Serial порт
    Serial.print(pressure);

    Serial.print(";");
    Serial.print(temperature);
    Serial.print(";");

    // считывание данных с аналог. термометра
    int temp = readTemperature(TEMP_PIN);
    float atemp = (temp * (5.0 / 1024.0) - 0.5) * 100.0;
    Serial.print(atemp);
    Serial.print(";");

    // переменная состояния датчика
    int check;
    // мониторинг ошибок
    check = dht.read(); // считывание данных с датчика DHT11
    switch (check) {
      // всё OK
      case DHT_OK:
        // выводим показания влажности и температуры
        Serial.print(dht.getTemperatureC());
        Serial.print(";");
        hum = dht.getHumidity();
        Serial.println(hum);
        break;
      // ошибка контрольной суммы
      case DHT_ERROR_CHECKSUM:
        Serial.println("Checksum error;");
        break;
      // превышение времени ожидания
      case DHT_ERROR_TIMEOUT:
        Serial.println("Time out error;");
        break;
      // неизвестная ошибка
      default:
        Serial.println("Unknown error;");
        break;
    }

    display.clearDisplay();
    //display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(40, 0);
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.println();
    //display.setCursor(0, 10);
    display.print("Temperature: "); display.println(temperature);
    display.print("Humidity:    "); display.println(hum);
    display.print("Pressure:    "); display.println(pressure);
    display.display();
  }
}

#define NUM_READS 50
float readTemperature(int sensorpin) {
  // read multiple values and sort them to take the mode
  int sortedValues[NUM_READS];
  for (int i = 0; i < NUM_READS; i++) {
    int value = analogRead(sensorpin);
    int j;
    if (value < sortedValues[0] || i == 0) {
      j = 0; //insert at first position
    }
    else {
      for (j = 1; j < i; j++) {
        if (sortedValues[j - 1] <= value && sortedValues[j] >= value) {
          // j is insert position
          break;
        }
      }
    }
    for (int k = i; k > j; k--) {
      // move all values higher than current reading up one position
      sortedValues[k] = sortedValues[k - 1];
    }
    sortedValues[j] = value; //insert current reading
  }
  //return scaled mode of 10 values
  float returnval = 0;
  for (int i = NUM_READS / 2 - 5; i < (NUM_READS / 2 + 5); i++) {
    returnval += sortedValues[i];
  }
  return returnval / 10;
}
