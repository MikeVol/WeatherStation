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

// создаём объект класса DHT11 и пераём номер пина к которому подкючён датчик
DHT11 dht(11);

// uint16_t raw_bandgap = 0;      // значение внутреннего bandgap
// float volt_battery = 0.0;
// unsigned int step = 1;
// float average = 0.0;

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
  // Чтение напряжения батареи
  // analogReference(DEFAULT);                   // использовать Vcc как AREF
  // raw_bandgap = analogRead(14);               // холостое чтение после смены AREF (см. 23.5.2 в руководстве)
  // average = 0.0;
  // for (int i = 0; i < 10; i++)
  // {
    // raw_bandgap += analogRead(14);             // измерить значение внутреннего bandgap
    // delay(10);
    // step++;
  // }        
  // volt_battery = (1.1 * 1024) / average / 10;  // вычислить Vcc

  // Временные костыли с подсчётом времени
  printtime();

  // Serial.print(volt_battery);
  // Serial.print("\t");

  // создаём переменную и присваиваем ей значения абсолютного давления
  float pressure = barometer.readPressureMillibars() / 1.3332;
  // создаём переменную и присваиваем ей температуру окружающей среды
  float temperature = barometer.readTemperatureC();

  // Вывод данных в Serial порт
  //Serial.print("p: ");
  Serial.print(pressure);
  Serial.print(";");
  //Serial.print(" mm Hg \t");
  //Serial.print("t: ");
  Serial.print(temperature);
  Serial.print(";");
  //Serial.println(" C");

  // переменная состояния датчика
  int check;

  // мониторинг ошибок

  int val = analogRead(TEMP_PIN); // считывание данных с аналог. термометра
  float atemp = (val*(5.0/1024.0)-0.5)*100;

  check = dht.read(); // считывание данных с датчика DHT11
  switch (check) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры

      //Serial.print("Температура = ");
      Serial.print(dht.getTemperatureC());
      Serial.print(";");
      //Serial.print("C \t");
      //Serial.print("Влажность = ");
      Serial.print(dht.getHumidity());
      Serial.print(";");
      //Serial.print("% \t");
      Serial.println(atemp);
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
