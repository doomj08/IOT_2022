#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "ESP32_MailClient.h"
#include <ESP32Ping.h>
// Incluimos librería
#include <DHT.h>
 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
 
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char *mqtt_user = "sensor";
const char *mqtt_pass = "swordfish";
const char *root_topic_subscribe_temperatura = "Sala/temperatura";
const char *root_topic_subscribe_humedad = "Sala/humedad";
const char *root_topic_subscribe_puerta = "Sala/puerta";
const char *root_topic_publish_temperatura = "Sala/public_temperatura";
const char *root_topic_publish_humedad = "Sala/public_humedad";
const char *root_topic_publish_puerta = "Sala/public_puerta";

//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "TINJACA";
const char* password =  "swordfish";

//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
SMTPData datosSMTP; //Datos correo electrónico
char msg[25];
long contador=0;

//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
//int count=0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //correo();
  // Comenzamos el sensor DHT
  dht.begin();
}


void loop() {  
  if (!client.connected()) {
    reconnect();
  }

  if (client.connected()){
    String str = "La cuenta es -> " + String(contador);
    str.toCharArray(msg,25);
    client.publish(root_topic_publish_temperatura,msg);
    contador++;
    temperatura();
    Serial.println(msg);
    delay(3000);
  }
  temperatura();
  delay(3000);
  client.loop();
}


//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(5000);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Broker...");
    // Creamos un cliente ID
    String clientId = "ESP_32";
    
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado al broker!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe_temperatura)){
        Serial.println("Suscripcion a topic "+ String(root_topic_subscribe_temperatura));
      }else{
        Serial.println("fallo Suscripciión a topic "+ String(root_topic_subscribe_temperatura));
      }
    } else {
      Serial.print("falló conexión broker:( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

}
 
void correo(){
digitalWrite(23, HIGH);
//Configuración del servidor de correo electrónico SMTP, host, puerto, cuenta y contraseña
datosSMTP.setLogin("smtp.gmail.com", 465, "jltinjaca@gmail.com", "pjqralrzzjxtkdcw");
// Establecer el nombre del remitente y el correo electrónico
datosSMTP.setSender("ESP32S", "jltinjaca@gmail.com");
// Establezca la prioridad o importancia del correo electrónico High, Normal, Low o 1 a 5 (1 es el más alto)
datosSMTP.setPriority("High");
// Establecer el asunto
datosSMTP.setSubject("Probando envio de correo con ESP32");
// Establece el mensaje de correo electrónico en formato de texto (sin formato)
datosSMTP.setMessage("Hola soy el esp32s! y me estoy comunicando contigo", false);
// Agregar destinatarios, se puede agregar más de un destinatario
datosSMTP.addRecipient("jltinjaca@gmail.com");
 //Comience a enviar correo electrónico.
if (!MailClient.sendMail(datosSMTP))
Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
//Borrar todos los datos del objeto datosSMTP para liberar memoria
datosSMTP.empty();
delay(10000);
digitalWrite(23, LOW);
}

void temperatura() {
    // Esperamos 5 segundos entre medidas
  delay(5000);
 
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  // Leemos la temperatura en grados Fahrenheit
  float f = dht.readTemperature(true);
 
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
 
  // Calcular el índice de calor en Fahrenheit
  float hif = dht.computeHeatIndex(f, h);
  // Calcular el índice de calor en grados centígrados
  float hic = dht.computeHeatIndex(t, h, false);
 
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Índice de calor: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
 
}
