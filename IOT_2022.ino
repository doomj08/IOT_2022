#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "ESP32_MailClient.h"
#include <ESP32Ping.h>
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
//#include "DHT.h"

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
DHTesp dht;
char msg[25];
long count=0;
int dhtPin = 4;

//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
bool getTemperature();

bool initTemp() {
  //byte resultValue = 0;
  // Initialize temperature sensor
  //dht.setup(dhtPin, DHTesp::DHT11);
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT iniciado");
}
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //correo();
}


void loop() {  
  if (!client.connected()) {
    reconnect();
  }

  if (client.connected()){
//    String str = "La cuenta es -> " + String(count);
    //str.toCharArray(msg,25);
    //client.publish(root_topic_publish,msg);
    //count++;
    //Serial.println(msg);
    getTemperature();
    Serial.println(___---___);
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

bool getTemperature() {
  // Reading temperature for humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
  // Check if any reads failed and exit early (to try again).
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }

  //float heatIndex = dht.computeHeatIndex(newValues.temperature, newValues.humidity);
  //float dewPoint = dht.computeDewPoint(newValues.temperature, newValues.humidity);
//  float cr = dht.getComfortRatio(cf, newValues.temperature, newValues.humidity);

  String comfortStatus;
/*  switch(cf) {
    case Comfort_OK:
      comfortStatus = "Comfort_OK";
      break;
    case Comfort_TooHot:
      comfortStatus = "Comfort_TooHot";
      break;
    case Comfort_TooCold:
      comfortStatus = "Comfort_TooCold";
      break;
    case Comfort_TooDry:
      comfortStatus = "Comfort_TooDry";
      break;
    case Comfort_TooHumid:
      comfortStatus = "Comfort_TooHumid";
      break;
    case Comfort_HotAndHumid:
      comfortStatus = "Comfort_HotAndHumid";
      break;
    case Comfort_HotAndDry:
      comfortStatus = "Comfort_HotAndDry";
      break;
    case Comfort_ColdAndHumid:
      comfortStatus = "Comfort_ColdAndHumid";
      break;
    case Comfort_ColdAndDry:
      comfortStatus = "Comfort_ColdAndDry";
      break;
    default:
      comfortStatus = "Unknown:";
      break;
  };*/

  Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity));
  return true;
}
