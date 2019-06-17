//-------------------------------------------------------------------------
//PROGRAMA:ProgramaFinalEducaContAhorroBateria.ino (Adafruit Huzzah ESP-32)
//Autor: Enrique Rodriguez Vela (Junio-2018)
//Descripción: Programa final EducaCont con ahorro de bateria
//------------------------------------------------------------------------

#include <WiFi.h>
const char* ssid     = "FamilyRodriguezVela";
const char* password = "Mondejar1996";
const char* host = "api.thingspeak.com";
const char* THINGSPEAK_API_KEY = "R1L31NB2UFEI83CM";
const boolean IS_METRIC = true;

// LCD Librerias
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);  // Direccion LCD 0x27
int show;

//DHT Librerias
#include <DHT.h>

#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQ7
#define MQ7 A1
int valor_mq7;

// MQ135
#define MQ135 A2
int valor_mq135;


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  int error;
  Wire.begin();
  Wire.beginTransmission(0x27);
  Serial.println("LCD...");
  dht.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(255);

  pinMode(MQ7, INPUT);
  pinMode(MQ135, INPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  Serial.print("connecting to ");
  Serial.println(host);

  // Usamos WifiClient para hacer las conexiones, con el puerto 80
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // Esperamos  para relaizar la medidas
  delay(2000);

   // Leemos los valores de los sensores
  //Serial DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println("Temperatura: ");
  Serial.println(t);
  Serial.println("Humedad: ");
  Serial.println(h);

  //Serial MQ7
  valor_mq7 = analogRead(MQ7);
  Serial.println("Valor MQ7: ");
  Serial.print(valor_mq7);
  Serial.print(" ppm");
  Serial.println("        ");

  //Serial MQ135
  valor_mq135 = analogRead(MQ135);
  Serial.println("Valor MQ135: ");
  Serial.print(valor_mq135);
  Serial.print(" ppm");
  Serial.println("        ");

  Serial.println("----------------------");
  delay(5000);
 
 //Comenzamos a poner los valores en el lcd
 //1º Humedad con el DHT11
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(4, 0);
  lcd.print("Humedad");
  lcd.setCursor(5, 1);
  lcd.print(h);
  lcd.print("%");

  delay(5000);

//2º Lectura Sensor MQ7
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(4, 0);
  lcd.print("Lec. MQ7:");
  lcd.setCursor(4, 1);
  lcd.print(valor_mq7);
  lcd.print(" ppm");

  delay(5000);

//3º Lectura Sensor MQ7
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(4, 0);
  lcd.print("Lec.MQ135:");
  lcd.setCursor(4, 1);
  lcd.print(valor_mq135);
  lcd.print(" ppm");

  delay(5000);

//4º Temperatura con el DHT11
  lcd.clear();
  lcd.setBacklight(255);
  lcd.setCursor(2, 0);
  lcd.print("Temperatura");
  lcd.setCursor(5, 1);
  lcd.print(     t);
  lcd.print((char)223);
   delay(5000);
 
//5º Apagar LCD para ahorrar bateria
   lcd.clear();
  lcd.setBacklight(0);



  delay(5000);

  // Creamos la URL de respuesta en TingSpeak
  String url = "/update?api_key=";
  url += THINGSPEAK_API_KEY;
  url += "&field1=";
  url += String(t);
  url += "&field2=";
  url += String(h);
  url += "&field3=";
  url += String(valor_mq7);
  url += "&field4=";
  url += String(valor_mq135);

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // Esto enviara la respuesta al servidor
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  while (!client.available()) {
    delay(100);
    Serial.print(".");
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  // Añadimos un retraso para limtitar el número de escrituras en Thinhspeak
  int duracionDelay = 600; //En segundos
  for (int i = 0; i < duracionDelay; i ++) { //Esto es debido a que el máximo que el Arduino puede procesar con precisión es 5000ms o 5 segundos
    delay (1000);
  }

} // End Programa
