#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "MyDevice";
const char *password = "87654321";

String html = R"rawliteral(<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Lamp Controller</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            background-color: rgb(255, 236, 236);
        }

        .bg {
            background-color: rgb(0, 151, 238);
            width: 100%;
            min-height: 50vh;
            display: flex;
            justify-content: center;
            align-items: end;
            box-shadow: 1px 29px 125px 90px rgba(0, 151, 238, 1);
            -webkit-box-shadow: 1px 29px 125px 90px rgba(0, 151, 238, 1);
            -moz-box-shadow: 1px 29px 125px 90px rgba(0, 151, 238, 1);
        }

        .container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .on-container {
            margin: 1rem;
            padding: 1rem 0em 1.3rem 0;
            background-color: rgb(3, 202, 252);
            border-radius: 30px;
            border: solid 3px;
            border-top: 0;
            border-color: rgb(240, 248, 255, 1);
        }

        .on {
            font-family: fantasy;
            font-size: 18px;
            font-weight: 500;
            text-decoration: none;
            padding: 1rem 3rem 1rem 3rem;
            color: rgb(0, 26, 255);
            background-color: rgba(255, 255, 255, 0.7);
            text-align: center;
            justify-content: center;
            border-radius: 30px;

        }

        .on:hover,
        .off:hover {
            background-color: rgb(240, 248, 255, .9);
        }

        .off-container {
            margin: 1rem;
            padding: 1rem 0em 1.3rem 0;
            background-color: rgba(252, 34, 34, 0.945);
            border-radius: 30px;
            border: solid 3px;
            border-top: 0;
            border-color: rgb(240, 248, 255, 1);
        }

        .off {
            font-family: fantasy;
            text-decoration: none;
            padding: 1rem 3rem 1rem 3rem;
            color: rgb(255, 0, 0);
            background-color: rgba(255, 255, 255, 0.7);
            text-align: center;
            justify-content: center;
            border-radius: 30px;

        }
    </style>
</head>

<body>

    <div class="bg">
        <div class="container">
            <div class="on-container">
                <a class="on" href="/H\">ON</a>
            </div>
            <br>
            <div class="off-container">
                <a class="off" href="/L\">OFF</a>
            </div>
        </div>
    </div>

</body>

</html>
)rawliteral";

NetworkServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);  // set the LED pin mode

  delay(10);

  // Connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  NetworkClient client = server.accept(); 

  if (client) {                    
    Serial.println("New Client.");  
    String currentLine = "";       
    while (client.connected()) {    
      if (client.available()) {    
        char c = client.read();    
        Serial.write(c);            
        if (c == '\n') {           
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print(html);

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(4, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(4, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
