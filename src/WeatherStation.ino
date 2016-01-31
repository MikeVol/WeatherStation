// библиотека для работы I²C
#include <Wire.h>
// библиотека для работы с модулями IMU
#include <troyka-imu.h>
// создаём объект для работы с барометром
Barometer barometer;
// библиотека для работы с датчиком DHT11
//#include <TroykaDHT11.h>
#include "DHT.h"
// номер пина к которому подкючён датчик DHT11
#define DHTPIN 11
#define DHTTYPE DHT11 //тип датчика DHT11
DHT dht(DHTPIN, DHTTYPE);

#define TEMP_PIN A0 // Аналоговый датчик температуры подключен к A0
unsigned long time;
float temperature;
float pressure;

void setup() {
    // открываем последовательный порт для мониторинга действий в программе
    Serial.begin(115200);
    dht.begin();
    barometer.begin();
}

void loop() {
    if (millis() % 60000 == 0)
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
        int temp = readTemperature(TEMP_PIN);
        float atemp = (temp * (5.0 / 1024.0) - 0.5) * 100.0;
        Serial.print(atemp);
        Serial.print(";");

        // выводим показания влажности и температуры DHT11
        Serial.print(dht.readTemperature());
        Serial.print(";");
        Serial.println(dht.readHumidity());

        // // переменная состояния датчика
        // int check;
        // // мониторинг ошибок
        // check = dht.read(); // считывание данных с датчика DHT11
        // switch (check) {
        //     // всё OK
        //     case DHT_OK:
        //     // выводим показания влажности и температуры
        //     Serial.print(dht.readTemperatureC());
        //     Serial.print(";");
        //     Serial.println(dht.readHumidity());
        //     break;
        //     // ошибка контрольной суммы
        //     case DHT_ERROR_CHECKSUM:
        //     Serial.println("Checksum error;");
        //     break;
        //     // превышение времени ожидания
        //     case DHT_ERROR_TIMEOUT:
        //     Serial.println("Time out error;");
        //     break;
        //     // неизвестная ошибка
        //     default:
        //     Serial.println("Unknown error;");
        //     break;
        // }
    }
}

void printtime() {// Выводит время в Serial
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

#define NUM_READS 50
float readTemperature(int sensorpin){
    // read multiple values and sort them to take the mode
    int sortedValues[NUM_READS];
    for(int i=0;i < NUM_READS;i++){
        int value = analogRead(sensorpin);
        int j;
        if(value<sortedValues[0] || i == 0){
            j = 0; //insert at first position
        }
        else{
            for(j = 1;j < i;j++){
                if(sortedValues[j - 1] <= value && sortedValues[j] >= value){
                    // j is insert position
                    break;
                }
            }
        }
        for(int k = i;k > j;k--){
            // move all values higher than current reading up one position
            sortedValues[k] = sortedValues[k-1];
        }
        sortedValues[j] = value; //insert current reading
    }
    //return scaled mode of 10 values
    float returnval = 0;
    for(int i = NUM_READS / 2 - 5; i < (NUM_READS / 2 + 5); i++){
        returnval +=sortedValues[i];
    }
    return returnval / 10;
}
