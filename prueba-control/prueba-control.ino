#include < WiFi .h >
#include < WebServer .h >

// Replace with your network credentials
const char *ssid = " Fran ";
const char *password = " alabajat ";

// Create an instance of the WebServer on port 80
ebServer server(80);

/ Motor 1 nt motor1Pin1 = 9;
nt motor1Pin2 = 10;
nt enable1Pin = 12;

/ Motor 2 nt motor2Pin1 = 7;
nt motor2Pin2 = 8;
nt enable2Pin = 14;

/ Setting PWM properties
        onst int freq = 30000;
onst int resolution = 8;
nt dutyCycle = 0;

tring valueString = String(0);

oid handleRoot()
{
onst char html [] PROGMEM = R ‘ ‘ rawliteral (
! DOCTYPE HTML > < html >
 head >
 meta name =" viewport " content =" width =device -width ,␣
ial - scale =1">

 link rel =" icon " href =" data : ,">
 style >
tml {
        font - family : Helvetica;
    display:
        inline - block;
    in:
        0 px auto;
        text - align : center; }
 button {
        -webkit - user - select : none;
        -moz - user - select :;
        -ms - user - select : none;
        user - select : none;
        ground - color : #4 CAF50;
    border:
        none;
    color:
        e;
    padding:
        12 px 28 px;
        text - decoration : none;
        -size : 26 px;
    margin:
        1 px;
    cursor:
        pointer; }
 button2 {
        background - color : #555555;}
/ style >
 script >
unction moveForward () {
        fetch(’/ forward ’); }
unction moveLeft () {
        fetch(’/ left ’); }
unction stopRobot () {
        fetch(’/ stop ’); }
unction moveRight () {
        fetch(’/ right ’); }
unction moveReverse () {
        fetch(’/ reverse ’); }

unction updateMotorSpeed ( pos ) {
        ocument.getElementById(’motorSpeed ’).innerHTML =
            ;
etch ( ‘/ speed ? value =${
            pos } ‘) ;

/ script >
/ head >
 body >
h1 > ESP32 Motor Control </ h1 >
p > < button class =" button " onclick =" moveForward ()">
ARD </ button > </p >
 div style =" clear :␣ both ;">
p >
 button class =" button " onclick =" moveLeft ()"> LEFT </
on >
 button class =" button ␣ button2 " onclick =" stopRobot ()"
OP </ button >

 button class =" button " onclick =" moveRight ()"> RIGHT </
on >
/p >
/ div >
p > < button class =" button " onclick =" moveReverse ()">
RSE </ button > </p >
p > Motor Speed : < span id =" motorSpeed " >0 </ span > </p >
 input type =" range " min ="0" max ="100 " step ="25" id ="
rSlider " oninput =" updateMotorSpeed ( this . value )"
e ="0"/ >
/ body >
/ html >) rawliteral ";
␣ server . send (200 ,␣ ‘‘text / html ", html ) ;


oid handleForward () {
            erial.println(" Forward ");
            igitalWrite(motor1Pin1, LOW);
            igitalWrite(motor1Pin2, HIGH);
            igitalWrite(motor2Pin1, LOW);
            igitalWrite(motor2Pin2, HIGH);
            erver.send(200);

            oid handleLeft()
            {
                erial.println(" Left ");
                igitalWrite(motor1Pin1, LOW);
                igitalWrite(motor1Pin2, LOW);
                igitalWrite(motor2Pin1, LOW);
                igitalWrite(motor2Pin2, HIGH);
                erver.send(200);

                oid handleStop()
                {
                    erial.println(" Stop ");
                    igitalWrite(motor1Pin1, LOW);

                    igitalWrite(motor1Pin2, LOW);
                    igitalWrite(motor2Pin1, LOW);
                    igitalWrite(motor2Pin2, LOW);
                    erver.send(200);

                    oid handleRight()
                    {
                        erial.println(" Right ");
                        digitalWrite(motor1Pin1, LOW);
                        digitalWrite(motor1Pin2, HIGH);
                        digitalWrite(motor2Pin1, LOW);
                        digitalWrite(motor2Pin2, LOW);
                        server.send(200);
                    }

                    void handleReverse()
                    {
                        Serial.println(" Reverse ");
                        digitalWrite(motor1Pin1, HIGH);
                        digitalWrite(motor1Pin2, LOW);
                        digitalWrite(motor2Pin1, HIGH);
                        digitalWrite(motor2Pin2, LOW);
                        server.send(200);
                    }

                    void handleSpeed()
                    {
                        if (server.hasArg(" value "))
                        {
                            valueString = server.arg(" value ");
                            int value = valueString.toInt();
                            if (value == 0)
                            {
                                ledcWrite(enable1Pin, 0);
                                ledcWrite(enable2Pin, 0);
                                digitalWrite(motor1Pin1, LOW);
                                digitalWrite(motor1Pin2, LOW);
                                digitalWrite(motor2Pin1, LOW);
                                digitalWrite(motor2Pin2, LOW);
                            }
                            else
                            {

                                dutyCycle = map(value, 25, 100, 200, 255);
                                ledcWrite(enable1Pin, dutyCycle);
                                ledcWrite(enable2Pin, dutyCycle);
                                Serial.println(" Motor ␣ speed ␣set ␣to␣" + String(value));
                            }
                        }
                        server.send(200);
                    }

                    void setup()
                    {
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

                        // Connect to Wi -Fi
                        Serial.print(" Connecting ␣to␣");
                        Serial.println(ssid);
                        WiFi.begin(ssid, password);
                        while (WiFi.status() != WL_CONNECTED)
                        {
                            delay(500);
                            Serial.print(".");
                        }
                        Serial.println("");
                        Serial.println(" WiFi ␣ connected .");

                        Serial.println("IP␣ address :␣");
                        Serial.println(WiFi.localIP());

                        // Define routes
                        server.on("/", handleRoot);
                        server.on("/ forward ", handleForward);
                        server.on("/ left ", handleLeft);
                        server.on("/ stop ", handleStop);
                        server.on("/ right ", handleRight);
                        server.on("/ reverse ", handleReverse);
                        server.on("/ speed ", handleSpeed);

                        // Start the server
                        server.begin();
                    }

                    void loop()
                    {
                        server.handleClient();
                    }
