# 🌱 Invernadero Inteligente con ESP32

Este proyecto implementa un sistema de monitoreo y control para un **invernadero inteligente**, utilizando un microcontrolador **ESP32**, sensores ambientales y un servidor web para visualizar datos en tiempo real y ajustar parámetros clave.

## 📷 Vista General

![Imagen de WhatsApp 2025-03-23 a las 23 23 48_4d2c2ad3](https://github.com/user-attachments/assets/d55d8e07-ecd2-43e9-88f3-3877b799ef78)

El sistema mide y controla:

- **Humedad del suelo** (YL-100)
- **Humedad y temperatura ambiente** (DHT11)
- **Nivel de luz** (LDR)
- **Nivel del tanque de agua** (sensor ultrasónico HC-SR04)
- **Estado del tanque de agua** (con alerta de "Tanque Vacío")
- **Control de actuadores**: bomba de riego, ventilador, y segunda bomba de alerta

Además, expone una **interfaz web local** vía WiFi para visualizar datos y modificar umbrales de actuación.

---

## 🔧 Hardware Utilizado

| Componente               | Descripción                    |
|--------------------------|--------------------------------|
| ESP32                    | Microcontrolador WiFi principal |
| Sensor DHT11             | Temperatura y humedad          |
| Sensor de luz (LDR)      | Medición de luminosidad        |
| Sensor ultrasónico HC-SR04 | Nivel del tanque de agua    |
| Sensor de humedad YL-100 | Humedad del suelo              |
| Bomba de agua            | Riego automático               |
| Ventilador               | Control de temperatura         |
| LED                      | Indicación visual              |

---

## 📡 Configuración WiFi

Este sistema funciona en **modo punto de acceso (AP)**. Al iniciar, el ESP32 crea una red WiFi a la cual puedes conectarte:

- **SSID**: `Invernadero`
- **Contraseña**: `12345678`

Una vez conectado, abre tu navegador y accede a:  
`http://192.168.4.1`

---

## 🖥️ Interfaz Web

La página principal muestra en tiempo real:

- Nivel del tanque (cm)
- Humedad del suelo (%)
- Humedad y temperatura ambiente
- Nivel de luz
- Estado del tanque

También permite **ajustar umbrales** para:

- Activación del LED por baja luz
- Control del ventilador por temperatura
- Activación de bomba por humedad del suelo
- Mínimo nivel de agua para activar bomba

---

## 📂 Estructura del Código

El código realiza:

- **Lectura periódica** de sensores
- **Control de actuadores** (bomba, ventilador)
- **Servidor web embebido** (WebServer ESP32)
- **Respuesta JSON** con datos de sensores (`/data`)
- **Formulario web** para cambiar umbrales (`/updateThresholds`)

---

## 🚀 Instalación y Uso

1. Clona este repositorio en tu IDE (Arduino IDE, PlatformIO, etc.)
2. Instala las siguientes bibliotecas:
   - `DHT sensor library` by Adafruit
   - `ESP32 WebServer` (ya incluida en el framework ESP32)
3. Conecta los sensores a los pines indicados en el código.
4. Sube el código al ESP32.
5. Conéctate a la red WiFi generada por el ESP32 (`Invernadero`) y navega a `http://192.168.4.1`.

---

## 📌 Pines Utilizados

| Función          | Pin ESP32 |
|------------------|-----------|
| LED indicador    | GPIO 2    |
| Sensor de luz    | GPIO 34   |
| Trigger ultrasónico | GPIO 27 |
| Echo ultrasónico | GPIO 26   |
| Humedad suelo    | GPIO 35   |
| Sensor DHT11     | GPIO 4    |
| Bomba de agua    | GPIO 18   |
| Bomba 2 (alerta) | GPIO 19   |
| Ventilador       | GPIO 5    |

---

## ⚙️ Umbrales Iniciales

| Parámetro           | Valor Inicial |
|---------------------|---------------|
| Umbral de luz       | 500           |
| Umbral de distancia | 13.0 cm       |
| Umbral humedad suelo| 20 %          |
| Umbral temperatura  | 25.0 °C       |

Puedes ajustarlos desde la interfaz web.

---

## 📄 Licencia

Este proyecto está bajo la licencia MIT. Libre para usar, modificar y compartir.

---

## Créditos

Desarrollado como proyecto personal.

Desarrollado por Daniel Alejandro Cangrejo López y Camila Andrea Cangrejo López.

##

Cualquier duda o mejora, ¡no dudes en contribuir! 🚀
