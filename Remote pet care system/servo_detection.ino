#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP32Servo.h> // Biblioteca compatible con ESP32
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

Servo myservo;  // crea el objeto servo

// GPIO de salida del servo
static const int servoPin = 13;

// GPIO del sensor de movimiento
static const int pirPin = 12;

// Reemplazar con tus credenciales de Wifi
const char* ssid     = "red paredes";
const char* password = "M2934567";

// Inicializa Bot Telegram
#define BOTtoken "8123267335:AAHrBKa3R6pLJolA2miBzMHC-jxnwFL33c8"  // Tu Bot Token (Obtener de Botfather)

#define CHAT_ID "7132883650"

WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);

// Variable para guardar el estado del sensor PIR
bool motionDetected = false;

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Usuario no autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;

    if (text == "/comida") {
      bot.sendMessage(chat_id, "Alimentando", "");
      myservo.write(90); // Mueve el servo a 90 grados             
      delay(500);        // Espera 500 ms                     
      myservo.write(0);  // Regresa el servo a 0 grados              
    }
  }
}

void setup() {
  Serial.begin(115200);

  myservo.attach(servoPin);  // vincula el servo en el servoPin

  pinMode(pirPin, INPUT); // Configura el pin del PIR como entrada

  // Conecta a red WiFi con SSID y password
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Muestra IP local 
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot iniciado", "");
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  // Leer el estado del sensor PIR
  if (digitalRead(pirPin) == HIGH) {
    if (!motionDetected) {
      // Si se detecta movimiento y no se había detectado antes, envía el mensaje
      bot.sendMessage(CHAT_ID, "Movimiento detectado", "");
      motionDetected = true; // Marca que se detectó movimiento
    }
  } else {
    motionDetected = false; // Restablece el estado si no hay movimiento
  }
}

