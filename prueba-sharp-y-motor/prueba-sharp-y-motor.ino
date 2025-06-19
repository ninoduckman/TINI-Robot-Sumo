#include < WiFi .h >
#include " ThingSpeak .h"
int contador = 0;
// Sensores
const int sensorKY = 4;
const int SHARPpin = 38;
// Motores
const int IN1 = 9;
const int IN2 = 10;
const int IN3 = 7;
const int IN4 = 8;
// WiFi
const char *ssid = " Fran ";
const char *password = "***";
// ThingSpeak
WiFiClient client;
unsigned long channelID = 2952594;
const char *writeAPIKey = " JU8XC38GBFHJ0VLA ";
void setup()
{
    Serial.begin(9600);
    pinMode(sensorKY, INPUT);
    pinMode(SHARPpin, INPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print(" Conectando ␣a␣ WiFi ");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("␣ Conectado ␣a␣ WiFi ");
    ThingSpeak.begin(client);
}
void loop()
{
    bool color = readKY();
    int sharpVal = promedio(20);
    Serial.print(" Sensor :␣");
    Serial.println(sharpVal);

    int threshold = 500;
    if (sharpVal > threshold)
    {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    }
    else
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }
    if (contador < 150)
    {
        contador++;
    }
    else
    {
        ThingSpeak.setField(3, color ? 1 : 0); // Campo
        KY(1 = blanco, 0 = negro)
        ThingSpeak.setField(2, sharpVal); // Campo
        SHARP
        int resp = ThingSpeak.writeFields(channelID,
                                          teAPIKey);
        if (resp == 200)
        {
            Serial.println(" Datos ␣ enviados ␣a␣ ThingSpeak ");
        }
        else
        {
            Serial.print(" Error ␣al␣ enviar :␣");
            Serial.println(resp);
        }

        contador = 0;
    }
    delay(10);
}
bool readKY()
{
    int val = digitalRead(sensorKY); // leer desde el sensor
    color if (val == HIGH)
    {
        Serial.println(" Negro ");
        return false;
    }
    else
    {
        Serial.println(" Blanco ");
        return true;
    }
}
int promedio(int n)
{
    long suma = 0;
    for (int i = 0; i < n; i++)
    {
        suma += analogRead(SHARPpin);
    }
    return (suma / n);
}