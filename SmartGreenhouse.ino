#include <DHT.h>
#include <driver/adc.h>
#include <WiFi.h>
#include <WebServer.h>

// Pines
const int ledPin = 2;
const int lightSensorPin = 34;
const int trigPin = 27;
const int echoPin = 26;
const int yl100Pin = 35;
const int dhtPin = 4;
const int BombaPin = 18;
const int Ventilador = 5;
const int Bomba2Pin = 19;

// Configuración WiFi
const char* ssid = "Invernadero";   // Nombre de la red
const char* password = "12345678";                     // Clave de la red

// Variables de sensores
float distance = 0.0;
float distance1 = 0.0;
float tanque = 0.0;
int valor_tierra = 0;
float humedad = 0.0;
float temperatura = 0.0;
int lightLevel = 0;
int yl100Value = 0;

// Aviso de tanque vacio
char x[12];

// Temporización
unsigned long previousMillis = 0;
const long interval = 1000;

// Objetos
DHT dht(dhtPin, DHT11);
WebServer server(80);

// -- UMBRALES (Ahora variables, para poder ajustarlos desde la web) --
int   lightThreshold    = 500;  // Umbral de luz (LED ON si lightLevel < lightThreshold)
float distanceThreshold = 13.0; // Usado para cálculo del nivel del tanque
int   yl100Threshold    = 20;   // Umbral de humedad de suelo (Bomba ON si < yl100Threshold)
float tempThreshold     = 25.0; // Umbral de temperatura (Ventilador ON si > tempThreshold)

// Prototipos de funciones
void handleRoot();
void handleData();
void handleUpdateThresholds();
float readUltrasonicDistance();
float LeerSensorh();
float LeerSensort();

void setup() {
  // Inicialización puerto serie y sensor DHT
  Serial.begin(115200);
  dht.begin();

  // Configuración de pines
  pinMode(ledPin, OUTPUT);
  pinMode(lightSensorPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(yl100Pin, INPUT);
  pinMode(BombaPin, OUTPUT);
  pinMode(Ventilador, OUTPUT);
  pinMode(Bomba2Pin, OUTPUT);

  // Apagamos Ventilador y Bomba (HIGH porque está invertido en este ejemplo)
  digitalWrite(Ventilador, HIGH);
  digitalWrite(BombaPin, HIGH);
  digitalWrite(Bomba2Pin, HIGH);

  // Configuración WiFi modo AP (punto de acceso)
  WiFi.softAP(ssid, password);
  Serial.println("\nIP address: ");
  Serial.println(WiFi.softAPIP());

  // Configuración servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  // Nueva ruta para actualización de umbrales
  server.on("/updateThresholds", HTTP_GET, handleUpdateThresholds);

  server.begin();
}

void loop() {
  server.handleClient();                  // Gestiona las comunicaciones del servidor con clientes
  unsigned long currentMillis = millis(); // Tiempo actual del sistema
  
  // Ejecuta acciones cada cierto intervalo sin bloquear
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Reinicia el temporizador
    
    // Lectura de sensores
    lightLevel    = analogRead(lightSensorPin);
    distance1     = readUltrasonicDistance();
    tanque        = distanceThreshold - distance;
    yl100Value    = analogRead(yl100Pin);
    valor_tierra  = map(yl100Value, 0, 4095, 100, 0);
    humedad       = LeerSensorh();
    temperatura   = LeerSensort();

    if(distance1 == 0.0){
      while(distance == 0.0){
      distance = readUltrasonicDistance();
      }
    } else{
      distance = distance1;
      }

    // Control de la Bomba
    if ((valor_tierra < yl100Threshold)) {
      // Activa la bomba 1
      if(tanque >= 5){
      digitalWrite(BombaPin, LOW);
      digitalWrite(Bomba2Pin, HIGH);
      strcpy(x, "OKAY        ");
      }else{
        if(tanque < 5){
          digitalWrite(BombaPin, HIGH);
          digitalWrite(Bomba2Pin, LOW);
          strcpy(x, "TANQUE VACIO");
          }
        }
      }else{
      // Humedad de tierra por encima del umbral o tanque bajo
      if ((valor_tierra >= yl100Threshold)) {
        if(tanque < 5){
          digitalWrite(Bomba2Pin, LOW);
          digitalWrite(BombaPin, HIGH);
          strcpy(x, "TANQUE VACIO");
          }
        if(tanque >= 5){
         strcpy(x, "OKAY        ");
         digitalWrite(BombaPin, HIGH);
         digitalWrite(Bomba2Pin, HIGH);
        }
      }
    }

    // Control del Ventilador
    if (temperatura >= tempThreshold) {
      digitalWrite(Ventilador, LOW);   // Ventilador encendido
      if (temperatura < tempThreshold) {
        digitalWrite(Ventilador, HIGH);
      }
    } else {
      digitalWrite(Ventilador, HIGH);  // Ventilador apagado
    }

    // Envío y presentación de datos en el monitor serie
    Serial.print("Luz: ");
    Serial.println(lightLevel);
    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.print("Nivel Tanque: ");
    Serial.print(tanque);
    Serial.println(" cm");
    Serial.print("Estado del Tanque: ");
    Serial.println(x);
    Serial.print("Humedad Tierra: ");
    Serial.print(valor_tierra);
    Serial.println(" %");
    Serial.print("Humedad Ambiente: ");
    Serial.print(humedad);
    Serial.println(" %");
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" C");
  }
  
  // Control LED (no bloqueante)
  digitalWrite(ledPin, (lightLevel < lightThreshold) ? HIGH : LOW);
}

// --------------------------------------------------------------------
// Manejador de la página principal
// --------------------------------------------------------------------
void handleRoot() {
  // Se define la variable 'html' que contiene el contenido completo de la página web
  // Incluimos un formulario para actualizar umbrales
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>Invernadero Inteligente</title>
        <style>
            body {
                font-family: Arial; 
                text-align: center; 
                margin: 20px;
            }
            h1 {
                color: #2c3e50;
            }
            .sensor-container {
                background: #ecf0f1;
                padding: 20px;
                border-radius: 10px;
                margin: 10px auto;
                max-width: 500px;
                box-shadow: 0 0 10px rgba(0,0,0,0.1);
            }
            .sensor-data {
                font-size: 20px;
                margin: 15px 0;
                color: #34495e;
            }
            .valor {
                font-weight: bold;
                color: #e74c3c;
            }
            .threshold-form {
                background: #ecf0f1;
                padding: 15px;
                border-radius: 10px;
                margin: 20px auto;
                max-width: 500px;
            }
            .threshold-form input {
                width: 100px;
                margin: 5px;
            }
        </style>
        <script>
            function actualizarDatos() {
                // Se realiza una petición para obtener los datos de sensores en formato JSON
                fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Actualizamos los elementos HTML con los valores recibidos
                    document.getElementById('tanque').innerHTML = data.tanque.toFixed(2);
                    document.getElementById('tierra').innerHTML = data.tierra;
                    document.getElementById('humedad').innerHTML = data.humedad.toFixed(2);
                    document.getElementById('temperatura').innerHTML = data.temperatura.toFixed(2);
                    document.getElementById('luz').innerHTML = data.light;
                    document.getElementById('level').innerHTML = data.level;

                    // Se programa la siguiente actualización después de 1 segundo
                    setTimeout(actualizarDatos, 1000);
                });
            }
            // Se inicia la actualización de datos al cargar la página
            window.onload = actualizarDatos;
        </script>
    </head>
    <body>
        <h1>Invernadero Inteligente</h1>
        
        <div class="sensor-container">
            <div class="sensor-data">
                Nivel del Tanque: <span class="valor" id="tanque">0</span> cm
            </div>
            <div class="sensor-data">
                Humedad de la Tierra: <span class="valor" id="tierra">0</span> %
            </div>
            <div class="sensor-data">
                Humedad Ambiente: <span class="valor" id="humedad">0</span> %
            </div>
            <div class="sensor-data">
                Temperatura: <span class="valor" id="temperatura">0</span> C
            </div>
            <div class="sensor-data">
                Nivel de Luz: <span class="valor" id="luz">0</span> lx
            </div>
            <div class="sensor-data">
                Estado del tanque: <span class="valor" id="level">0</span>
            </div>
        </div>

        <!-- Formulario para actualizar los umbrales -->
        <div class="threshold-form">
            <h2>Actualizar Umbrales</h2>
            <form action="/updateThresholds" method="GET">
                <div>
                    <label for="light">Umbral Luz:</label>
                    <input type="number" id="light" name="light" value=")rawliteral" + String(lightThreshold) + R"rawliteral(">
                </div>
                <div>
                    <label for="distance">Umbral Distancia (cm):</label>
                    <input type="number" step="0.01" id="distance" name="distance" value=")rawliteral" + String(distanceThreshold) + R"rawliteral(">
                </div>
                <div>
                    <label for="yl100">Umbral Humedad Tierra:</label>
                    <input type="number" id="yl100" name="yl100" value=")rawliteral" + String(yl100Threshold) + R"rawliteral(">
                </div>
                <div>
                    <label for="temp">Umbral Temperatura (C):</label>
                    <input type="number" step="0.01" id="temp" name="temp" value=")rawliteral" + String(tempThreshold) + R"rawliteral(">
                </div>
                <br>
                <input type="submit" value="Actualizar">
            </form>
        </div>
    </body>
    </html>
  )rawliteral";

  // Se envía la respuesta HTTP con el contenido HTML al cliente
  server.send(200, "text/html", html);
}

// --------------------------------------------------------------------
// Manejador para enviar datos en formato JSON
// --------------------------------------------------------------------
void handleData() {
  // Construimos la cadena JSON a enviar
  String json = "{";
  json += "\"tanque\":"      + String(tanque)       + ",";
  json += "\"tierra\":"      + String(valor_tierra) + ",";
  json += "\"humedad\":"     + String(humedad)      + ",";
  json += "\"temperatura\":" + String(temperatura)  + ",";
  json += "\"light\":"       + String(lightLevel)   + ",";
  json += "\"level\":\""     + String(x)            + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

// --------------------------------------------------------------------
// Manejador para actualizar los umbrales
// --------------------------------------------------------------------
void handleUpdateThresholds() {
  // Verifica si existen los argumentos en la petición GET
  if (server.hasArg("light")) {
    lightThreshold = server.arg("light").toInt();
  }
  if (server.hasArg("distance")) {
    distanceThreshold = server.arg("distance").toFloat();
  }
  if (server.hasArg("yl100")) {
    yl100Threshold = server.arg("yl100").toInt();
  }
  if (server.hasArg("temp")) {
    tempThreshold = server.arg("temp").toFloat();
  }
  
  // Redirecciona de nuevo a la página principal
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Umbrales actualizados. Redirigiendo...");
}

// --------------------------------------------------------------------
// Función para leer la distancia con ultrasonido
// --------------------------------------------------------------------
float readUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2.0;

}

// --------------------------------------------------------------------
// Función para leer la humedad ambiente
// --------------------------------------------------------------------
float LeerSensorh() {
  float h = dht.readHumidity();
  return isnan(h) ? 0.0 : h;
}

// --------------------------------------------------------------------
// Función para leer la temperatura ambiente
// --------------------------------------------------------------------
float LeerSensort() {
  float t = dht.readTemperature();
  return isnan(t) ? 0.0 : t;
}
