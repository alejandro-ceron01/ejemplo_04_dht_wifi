#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

const char *ssid = "LAPTOP-NNFS5F72 1383";
const char *pass = "12345678";
const char *host = "dweet.io";
const int port = 80; 

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE); //CONSTRUYO UN OBJETO DE LA CLASE DHT que representa el sensor 

void setup() {
  Serial.begin(115200);
  Serial.println("prueba del DHT22!");
  dht.begin();//inicializo el sensor
  WiFi.begin(ssid, pass); // Inicializar modulo WIFI y que se vincule a la red indicada en el ssid
  Serial.print("Intentando Conexion");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado");
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  pinMode(2, OUTPUT);    // Configurar pin 2 como salida
  digitalWrite(2, HIGH);

}


void loop() {
  delay(2000);
  float humedad = dht.readHumidity();//leo la humedad relativa
  float temperatura = dht.readTemperature();//leo la temperatura
  
  if(isnan(humedad)||isnan(temperatura)){//si hay un error en la lectura
   Serial.println("Error al leer el sensor DHT22");
   return;
  }

  Serial.print("Humedad: ");
  Serial.println(humedad);
  Serial.print("Temperatura: ");
  Serial.println(temperatura);

 WiFiClient cliente;

    if (!cliente.connect(host, port))
  {
    Serial.println("Conexion Fallida al servidor");
    delay(1000);
    return;
  }
 String request = String("GET ") +"/dweet/for/AlejandroCeron?"+"Humedad=" +
  String(humedad) +"&"+"Temperature=" + String(temperatura) + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" +
   "Connection: close\r\n\r\n";

  // Enviamos la peticion en  protocolo http al servidor
  cliente.print(request);
  unsigned long inicio = millis();
  while (cliente.available() == 0)
  {
    if (millis() - inicio > 2000)
    {
      Serial.println("Tiempo de espera del servidor agotado");
      cliente.stop();
      return;
    }
  }

  // Si se llega a este punto es porque se recibieron datos del servidor (cliente.available() !=0)
  while (cliente.available())
  {
    String linea = cliente.readStringUntil('\r'); // Lea un string hasta que encuentre el caracter
    Serial.println(linea);
  }

  Serial.println("Fin de conexion al servidor");
  cliente.stop();
  delay(2000); //Para evitar de que se hagan muchas peticiones al servidor y nos meta en un black list


}