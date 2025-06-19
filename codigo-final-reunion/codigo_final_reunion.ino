
#include <WiFi.h>
#include <WebServer.h>
#include <ThingSpeak.h>

// Replace with your network credentials
const char* ssid     = "Fran";
const char* password = "alabajat";
WiFiClient client;
unsigned long channelID = 2952594;
const char* writeAPIKey = "JU8XC38GBFHJ0VLA";

// Create an instance of the WebServer on port 80
WebServer server(80);
//Variables
bool color;
int sharpVal;
unsigned long t_thingspeak = 0;
unsigned long t_actual;
unsigned long t_giro;
unsigned long t_cycle_a = 0;
unsigned long t_cycle_g = 0;
bool enMarchaAvanzar = true;
bool enMarchaGirar = true;
const unsigned long duracionAvanzar = 150;  
const unsigned long duracionFrenar = 30;
const unsigned long duracionGirar = 20;
//Sensores  
const int sensorKY = 4;
const int SHARPpin = 38;

// Motor Derecho
int motor1Pin1 = 9; 
int motor1Pin2 = 10; 
int enable1Pin = 12;

// Motor Izquierdo
int motor2Pin1 = 7; 
int motor2Pin2 = 8; 
int enable2Pin = 14;

// Setting PWM properties
const int freq = 30000;
const int resolution = 8;
int dutyCycle = 0;
const int dutycycle_izq = 210;
const int dutycycle_der = 210;

String valueString = String(0);

void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
      html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }
      .button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: #4CAF50; border: none; color: white; padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer; }
      .button2 {background-color: #555555;}
      .mode-button { background-color: #2196F3; margin: 10px; }
    </style>
    <script>
      function updateThingSpeakMode(mode) {
        const url = `https://api.thingspeak.com/update?api_key=JU8XC38GBFHJ0VLA&field1=${mode}`;
        
        fetch(url)
          .then(response => response.text())
          .then(data => {
            if (data !== "0") {
              alert(`Modo: ${mode}`);
            } else {
              alert("Espera 15 segundos!");
            }
          })
          .catch(err => addToConsole(`Error: ${err})`));
      }

      function addToConsole(message) {
        console.log(message);
      }

      function moveForward() { fetch('/forward'); }
      function moveLeft() { fetch('/left'); }
      function stopRobot() { fetch('/stop'); }
      function moveRight() { fetch('/right'); }
      function moveReverse() { fetch('/reverse'); }
      function updateMotorSpeedL(pos) {
        document.getElementById('motorSpeedL').innerHTML = pos;
        fetch("/speedl?valuel=${pos}");
      }
      function updateMotorSpeedR(pos) {
        document.getElementById('motorSpeedR').innerHTML = pos;
        fetch("/speedr?valuer=${pos}");
      }
    </script>
  </head>
  <body>
    <h1>ESP32 Motor Control</h1>

    <div>
      <button class="button mode-button" onclick="updateThingSpeakMode(1)">Modo Balanceado</button>
      <button class="button mode-button" onclick="updateThingSpeakMode(2)">Modo Control</button>
      <button class="button mode-button" onclick="updateThingSpeakMode(3)">Modo Roomba</button>
    </div>
    
    <p><button class="button" onclick="moveForward()">FORWARD</button></p>
    <div style="clear: both;">
      <p>
        <button class="button" onclick="moveLeft()">LEFT</button>
        <button class="button button2" onclick="stopRobot()">STOP</button>
        <button class="button" onclick="moveRight()">RIGHT</button>
      </p>
    </div>
    <p><button class="button" onclick="moveReverse()">REVERSE</button></p>
    <p>Motor Speed l: <span id="motorSpeedL">0</span></p>
    <input type="range" min="0" max="255" step="5" id="motorSlider" oninput="updateMotorSpeedL(this.value)" value="0"style="width:300px"/>
    <p>Motor Speed r: <span id="motorSpeedR">0</span></p>
    <input type="range" min="0" max="255" step="5" id="motorSlider" oninput="updateMotorSpeedR(this.value)" value="0" style="width:300px"/>
  </body>
  </html>)rawliteral";
  server.send(200, "text/html", html);
}

void handleForward() {
  Serial.println("Forward");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
  server.send(200);
}

void handleLeft() {
  Serial.println("Left");
  digitalWrite(motor1Pin1, LOW); 
  digitalWrite(motor1Pin2, LOW); 
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
  server.send(200);
}

void handleStop() {
  Serial.println("Stop");
  digitalWrite(motor1Pin1, LOW); 
  digitalWrite(motor1Pin2, LOW); 
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);   
  server.send(200);
}

void handleRight() {
  Serial.println("Right");
  digitalWrite(motor1Pin1, LOW); 
  digitalWrite(motor1Pin2, HIGH); 
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);    
  server.send(200);
}

void handleReverse() {
  Serial.println("Reverse");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);          
  server.send(200);
}

void handleSpeed() {
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    int value = valueString.toInt();
    if (value == 0) {
      ledcWrite(enable1Pin, 0);
      ledcWrite(enable2Pin, 0);
      digitalWrite(motor1Pin1, LOW); 
      digitalWrite(motor1Pin2, LOW); 
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, LOW);   
    } else { 
      dutyCycle = map(value, 25, 100, 200, 255);
      ledcWrite(enable1Pin, dutyCycle);
      ledcWrite(enable2Pin, dutyCycle);
      Serial.println("Motor speed set to " + String(value));
    }
  }
  server.send(200);
}

void setup() {
  Serial.begin(9600);
  // Set the Motor pins as outputs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // Configure PWM Pins
  ledcAttach(enable1Pin, freq, resolution);
  ledcAttach(enable2Pin, freq, resolution);
    
  // Initialize PWM with 0 duty cycle
  ledcWrite(enable1Pin, 0);
  ledcWrite(enable2Pin, 0);
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/left", handleLeft);
  server.on("/stop", handleStop);
  server.on("/right", handleRight);
  server.on("/reverse", handleReverse);
  server.on("/speed", handleSpeed);
  ThingSpeak.begin(client);
  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();
  color = readKY();
  sharpVal = promedio(20);
  float modorobot = ThingSpeak.readFloatField(channelID, 1, writeAPIKey);
  Serial.println("Valor del modorobot: ");
  Serial.println(modorobot);
  if (millis() - t_thingspeak >= 15000) { //Mandar datos
    t_thingspeak = millis();
    mandardatos();
  }
  if (modorobot==2){ //Control
    Serial.print("Usando a: Modo Control");
    server.handleClient();
  }
  else{
    if (modorobot == 3) {
      server.handleClient();
      Serial.print("Usando: Modo Roomba");
      while (!color) {
        avanzarFrenado();
        color = readKY(); // actualizar sensor
      }
      frenar();
      t_actual = millis();
      if (color){
        while (millis() - t_actual < 800) retroceder();
        t_actual = millis();
        t_giro = random(75, 150);
        while (millis() - t_actual < t_giro) girarFrenado();        
      }
    } else {
      server.handleClient();
      Serial.print("Usando: Modo Balanceado");
      if (color) {
        t_actual = millis();
        while (millis() - t_actual < 500) retroceder();
        t_actual = millis();
        t_giro = random(75, 150);
        while (millis() - t_actual < t_giro) girarFrenado();
      } else {
        if (sharpVal > 500) {
          avanzarFrenado();
        } else {
            girarFrenado();
          
        }
      }
    }
  }
}

  void mandardatos(){
    ThingSpeak.setField(3, color ? 1 : 0);      // Campo 1: KY (1=blanco, 0=negro)
    ThingSpeak.setField(2, sharpVal);           // Campo 2: SHARP
    int resp = ThingSpeak.writeFields(channelID, writeAPIKey);
    if (resp == 200) {
      Serial.println("Datos enviados a ThingSpeak");
    } else {
      Serial.print("Error al enviar: ");
      Serial.println(resp);
    }
  }
void avanzar() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
  ledcWrite(enable1Pin, dutycycle_der);
  ledcWrite(enable2Pin, dutycycle_izq);
}

  void retroceder() {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    ledcWrite(enable1Pin, dutycycle_der);
    ledcWrite(enable2Pin, dutycycle_izq);
  }

  void girar() {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    ledcWrite(enable1Pin, dutycycle_der);
    ledcWrite(enable2Pin, dutycycle_izq);
  }
  void frenar() {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
    ledcWrite(enable1Pin, 0);
    ledcWrite(enable2Pin, 0);
  }
  bool readKY() {
    int val = digitalRead(sensorKY);

    if (val == HIGH) {
      Serial.println("Negro");  
      return true;
    } else {
      Serial.println("Blanco");
      return false;
    }
  }

  int promedio(int n) {
    long suma = 0;
    for (int i = 0; i < n; i++) {
      suma += analogRead(SHARPpin);
    }
    Serial.println(suma/n);
    return suma / n;
  }

  void avanzarFrenado() {
    enMarchaGirar = true;
    unsigned long ahora = millis();
    unsigned long duracionActual = enMarchaAvanzar ? duracionAvanzar : duracionFrenar;
    if (ahora - t_cycle_a >= duracionActual) {
      t_cycle_a = ahora;
      enMarchaAvanzar = !enMarchaAvanzar; 
    }
    if (enMarchaAvanzar) {
      avanzar();
    } else {
      frenar();
    }
  } 

void girarFrenado(){
      if (millis() - t_cycle_g >= 5) {
      t_cycle_g = millis();
      enMarchaGirar = !enMarchaGirar; 
    }
    if (enMarchaGirar) {
      girar();
    } else {
      frenar();
    }
}
/* void girarFrenado() {
    enMarchaAvanzar = true;
    unsigned long ahora = millis();
    unsigned long duracionActual = enMarchaGirar ? duracionGirar : duracionFrenar;
    if (ahora - t_cycle_g >= duracionActual) {
      t_cycle_g = ahora;
      enMarchaGirar = !enMarchaGirar; 
    }
    if (enMarchaGirar) {
      girar();
    } else {
      frenar();
    }
  } 
*/
 