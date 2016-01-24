// библиотека для работы I²C
#include <Wire.h>
// библиотека для работы с модулями IMU
#include <troyka-imu.h>
// создаём объект для работы с барометром
Barometer barometer;
// библиотека для работы с датчиком DHT11
#include <TroykaDHT11.h>
#define TEMP_PIN A0 // Аналоговый датчик температуры подключен к A0 
unsigned long time;
float temperature;
float pressure;

// создаём объект класса DHT11 и пераём номер пина к которому подкючён датчик
DHT11 dht(11);

void setup()
{ 
  // открываем последовательный порт для мониторинга действий в программе
  Serial.begin(115200);
  dht.begin();
  barometer.begin();
}

void loop()
{
  if (millis() % 10000 == 0)
  {
  
  // Временные костыли с подсчётом времени
  printtime();

  // читаем значение абсолютного давления
  pressure = 0.0;
  for(int i=0; i<5; i++){
      pressure += barometer.readPressureMillibars() / 1.3332;
  }
  pressure = pressure / 5; 
  
  // читаем значение температуры окружающей среды
  temperature = 0.0;
  for(int i=0; i<5; i++){
      temperature += barometer.readTemperatureC();
  }
  temperature = temperature / 5;

  // Вывод данных в Serial порт
  Serial.print(pressure);
  Serial.print(";");
  Serial.print(temperature);
  Serial.print(";");

  // считывание данных с аналог. термометра
  float atemp = 0.0;
  for(int i=0; i<5; i++){
      atemp = atemp + (analogRead(TEMP_PIN) * (5.0 / 1024.0) - 0.5) * 100.0;
  }
  atemp = atemp / 5;
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
      Serial.println(dht.getHumidity());
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
}
}

void printtime() // Выводит время в Serial
{
  int time = millis() / 1000;
  if (time / 60 / 60 < 10) {
    Serial.print ("0");
  }
  Serial.print (time / 60 / 60);
  Serial.print (":");
  if (time / 60 % 60 < 10) {
    Serial.print ("0");
  }
  Serial.print ((time / 60) % 60);
  Serial.print (":");
  if (time % 60 < 10) {
    Serial.print ("0");
  }
  Serial.print (time % 60);
  Serial.print(";");
}

