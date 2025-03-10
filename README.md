# God-WalkieTalkie-V0
 A general propouse pocket God-consultor

# 🤖 ESP32-C3 OLED (Supermini) con ChatGPT

## 📌 Introducción
Este proyecto consiste en un módulo ESP32-C3 (versión "Supermini" con pantalla OLED integrada de 0.42 pulgadas SSD1306) conectado a la API de ChatGPT. La interfaz permite enviar consultas a través del monitor serie y visualizar respuestas generadas por inteligencia artificial directamente en la pantalla OLED.

## 🚀 Características del Proyecto
- Conexión Wi-Fi establecida mediante ESP32.
- Comunicación directa con la API de ChatGPT (GPT-3.5 Turbo).
- Visualización en tiempo real de respuestas en pantalla OLED monocromática integrada (72x40 px).
- Gestión automática del texto para ajustarse al pequeño tamaño del display.
- Inicialmente pensado para mostrar emojis, pero adaptado a respuestas textuales debido a limitaciones del hardware SSD1306.

---

## 🛠️ Hardware Utilizado
- ESP32-C3 Supermini (placa compacta con pantalla OLED integrada SSD1306 de 72x40 píxeles).
- Conexión I²C integrada (GPIO5 como SDA, GPIO6 como SCL).

---

## 🔧 Librerías y Software
- Arduino IDE
- [U8g2](https://github.com/olikraus/u8g2) para controlar el OLED.
- Librerías estándar ESP32 WiFi, HTTPClient y ArduinoJSON.

---

## ⚙️ Configuración del Hardware (clave del éxito)

Uno de los puntos clave del proyecto fue la identificación correcta de los pines I²C específicos del módulo Supermini OLED integrado:

```cpp
Wire.begin(5,6); // SDA = GPIO5, SCL = GPIO6 (Configuración crítica)


🔎 Progreso del Desarrollo
🟢 Fase Inicial:
Comunicación básica con API ChatGPT exitosa.
Pantalla OLED inicialmente no funcionaba con ejemplos estándar.
🔴 Problemas Encontrados:
Pantalla OLED no respondía con el código estándar (no mostraba texto ni gráficos).
Intento inicial de usar emojis Unicode complejos fracasó por limitaciones del display monocromo.
🔧 Soluciones Implementadas:
Confirmación del driver exacto del OLED: U8G2_SSD1306_72X40_ER_F_HW_I2C.
Inicialización explícita de pines I²C (Wire.begin(5,6)).
Uso exclusivo de fuentes compatibles (u8g2_font_ncenB08_tr).
🖥️ Ajuste del Texto:
Desarrollo de una rutina personalizada para dividir automáticamente el texto en líneas ajustadas a la pantalla OLED pequeña, evitando cortes abruptos de palabras y optimizando el espacio limitado.