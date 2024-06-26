#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

//   if (isnan(h) || isnan(t)) {
//     Serial.println("Failed to read from DHT sensor!");
//     return;
//   }

  //Serial.print("Humidity: ");
  Serial.println(h);
  Serial.flush();
//   Serial.print(" %\t");
//   Serial.print("Temperature: ");
//   Serial.print(t);
//   Serial.println(" *C");

  delay(1000);
}
