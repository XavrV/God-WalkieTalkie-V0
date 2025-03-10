#include "secrets.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include <Wire.h>

// Credenciales Wi-Fi y API Key

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* apiKey = OPENAI_API_KEY;

const int max_chars = 12;
// Endpoint OpenAI
const char* openai_endpoint = "https://api.openai.com/v1/chat/completions";
const char* model = "gpt-3.5-turbo";
const int max_tokens = 10;      // respuesta breve
const float temperature = 1.5;  // creatividad

// OLED ESP32-C3 Supermini integrado
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// **Función para convertir caracteres UTF-8 a ASCII extendido**
String convertUTF8toASCII(String str) {
  str.replace("á", "\xE1");
  str.replace("é", "\xE9");
  str.replace("í", "\xED");
  str.replace("ó", "\xF3");
  str.replace("ú", "\xFA");
  str.replace("ñ", "\xF1");
  str.replace("Á", "\xC1");
  str.replace("É", "\xC9");
  str.replace("Í", "\xCD");
  str.replace("Ó", "\xD3");
  str.replace("Ú", "\xDA");
  str.replace("Ñ", "\xD1");
  return str;
}

void displayTextOLED(String text) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_mf); // **Fuente más compacta para mejor uso del espacio**

  // Ajustes de alineación fina para la pantalla 72x40
  const int xOffset = 33;  // Pequeño margen izquierdo
  const int yOffset = 20;  // **Reducimos aún más el margen superior**
  const int lineHeight = 10;  // **Menos separación entre líneas para mostrar más texto**
  const int maxCharsPerLine = 12;  // **Máximo de caracteres ajustado al nuevo tamaño de fuente**
  int y = yOffset;  

  while (text.length() > 0 && y <= 70 - lineHeight) {
    int lineLength = (text.length() > maxCharsPerLine) ? maxCharsPerLine : text.length();
    int lastSpace = text.substring(0, lineLength).lastIndexOf(' ');

    if (lineLength == maxCharsPerLine && lastSpace != -1) {
      lineLength = lastSpace;
    }

    // **Corrección de caracteres acentuados**
    String line = text.substring(0, lineLength);
    line = convertUTF8toASCII(line);

    u8g2.drawStr(xOffset, y, line.c_str());  // **Texto mejor alineado**

    text = text.substring(lineLength);
    text.trim();
    y += lineHeight;  // **Espaciado optimizado**
  }

  u8g2.sendBuffer();
}


void setup() {
  Serial.begin(115200);
  Wire.begin(5, 6);  // SDA=GPIO5, SCL=GPIO6

  u8g2.begin();
  displayTextOLED("Iniciando...");

  WiFi.begin(ssid, password);
  int max_retries = 20;
  int retry_count = 0;

  while (WiFi.status() != WL_CONNECTED && retry_count < max_retries) {
    delay(500);
    retry_count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    displayTextOLED("WiFi OK");
    delay(2000);
  } else {
    displayTextOLED("WiFi FAIL");
  }

  delay(2000);
  displayTextOLED("Esperando...");
}

String askOpenAI(String userQuery) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error: No hay conexión WiFi.");
    return "Sin WiFi";
  }

  HTTPClient http;
  http.begin(openai_endpoint);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(apiKey));

  StaticJsonDocument<256> doc;
  doc["model"] = model;
  doc["max_tokens"] = max_tokens;
  doc["temperature"] = temperature;

  JsonArray messages = doc.createNestedArray("messages");
  JsonObject msg = messages.createNestedObject();
  msg["role"] = "user";
  msg["content"] = "Responde sugerentemente, en muy pocas palabras, con picardía con sarcasmo y atrevimiento a: " + userQuery;

  String requestBody;
  serializeJson(doc, requestBody);

  int httpResponseCode = http.POST(requestBody);
  String response = "Error API";

  if (httpResponseCode > 0) {
    String payload = http.getString();
    StaticJsonDocument<512> responseJson;

    DeserializationError error = deserializeJson(responseJson, payload);
    if (!error) {
      if (responseJson["choices"].size() > 0) {
        response = responseJson["choices"][0]["message"]["content"].as<String>();
      } else {
        response = "Respuesta vacía";
      }
    } else {
      response = "Error JSON";
    }
  } else {
    Serial.print("Error HTTP: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Liberar memoria
  return response;
}

// Muestra texto en OLED con ajuste automático

void loop() {
  if (Serial.available()) {
    String userQuery = Serial.readStringUntil('\n');
    userQuery.trim();

    if (!userQuery.isEmpty()) {
      Serial.println("Pregunta: " + userQuery);
      displayTextOLED("Procesando...");

      String respuesta = askOpenAI(userQuery);
      respuesta.trim();

      Serial.println("Respuesta: " + respuesta);
      displayTextOLED(respuesta);

      delay(15000);
      displayTextOLED("Esperando...");
    }
  }
}