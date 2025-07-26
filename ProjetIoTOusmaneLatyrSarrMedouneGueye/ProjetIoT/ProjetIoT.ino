#include <ESP8266WiFi.h>  
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT11

#define TRIG_PIN D6
#define ECHO_PIN D7
#define BUZZER_PIN D5

DHT dht(DHTPIN, DHTTYPE);
bool buzzerDesactive = false;  

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  
  if (duration == 0) return -1;

  float distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float distance = readDistance();

  // Activation du buzzer si la distance dépasse 15 cm et qu’il n’est pas désactivé
  if (distance > 15 && !buzzerDesactive) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  if (Serial.available()) {
    String commande = Serial.readStringUntil('\n');
    commande.trim();  
    if (commande == "UNLOCK") {
      buzzerDesactive = true;
      Serial.println("Buzzer désactivé");
    }
  }

  // Envoi des données en JSON
  Serial.print("{\"temperature\":");
  Serial.print(isnan(temperature) ? "null" : String(temperature));
  Serial.print(",\"humidity\":");
  Serial.print(isnan(humidity) ? "null" : String(humidity));
  Serial.print(",\"distance\":");
  Serial.print(distance < 0 ? "null" : String(distance));
  Serial.println("}");

  delay(2000); // Attente de 2 secondes entre chaque lecture
}
