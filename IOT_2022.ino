#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "ESP32_MailClient.h"
#include <ESP32Ping.h>
#include "DHT.h"

//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "node02.myqtthub.com";
const int mqtt_port = 1883;
const char *mqtt_user = "32";
const char *mqtt_pass = "32";
const char *root_topic_subscribe = "Temperatura/esp32";
const char *root_topic_publish = "Temperatura/public_esp32";

//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "FLOWERALARCON";
const char* password =  "1118573653";

//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
long count=0;
SMTPData datosSMTP;
//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  correo();
}


void loop() {
  
  if (!client.connected()) {
    reconnect();
  }

  if (client.connected()){
    String str = "La cuenta es -> " + String(count);
    str.toCharArray(msg,25);
    client.publish(root_topic_publish,msg);
    count++;
    Serial.println(msg);
    delay(3000);
  }
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
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion a topic "+ String(root_topic_subscribe));
      }else{
        Serial.println("fallo Suscripciión a topic "+ String(root_topic_subscribe));
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