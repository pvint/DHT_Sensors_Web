#include <Arduino.h>
#include <WebServer.h>


#include "Ticker.h"
#include "DHTesp.h"

// The html for each sensor is currently hard coded below in index_html etc
#define NUM_SENSORS 2

// Set pin for each sensor
int dhtPin[NUM_SENSORS];

#include <WiFi.h>

const char* ssid     = "SSID";
const char* password = "PASSPHRASE";

WebServer server(80);


DHTesp dhtSensor[NUM_SENSORS];  
TempAndHumidity sensorData;


// The index.html page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h3>Wine Room</h3>
  <span class="dht-labels">Inside:</span>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature1">--</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity1">--</span>
    <sup class="units">&percnt;</sup>
  </p> 
  <span class="dht-labels">Outside:</span>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature2">--</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity2">--</span>
    <sup class="units">%</sup>
  </p>
  <h3>
    Last update: <span id="lastUpdate"></span>
  </h3>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature1").innerHTML = this.responseText;
      document.getElementById('lastUpdate').innerHTML = new Date();
    }
  };
  xhttp.open("GET", "/temperature1", true);
  xhttp.send();
}, 3000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity1").innerHTML = this.responseText;
      document.getElementById('lastUpdate').innerHTML = new Date();
    }
  };
  xhttp.open("GET", "/humidity1", true);
  xhttp.send();
}, 4000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature2").innerHTML = this.responseText;
      document.getElementById('lastUpdate').innerHTML = new Date();
    }
  };
  xhttp.open("GET", "/temperature2", true);
  xhttp.send();
}, 3500 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity2").innerHTML = this.responseText;
      document.getElementById('lastUpdate').innerHTML = new Date();
    }
  };
  xhttp.open("GET", "/humidity2", true);
  xhttp.send();
}, 4500 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE1"){
    return String(dhtSensor[0].getTemperature(), 1); 
  }
  else if(var == "HUMIDITY1"){
    return String(dhtSensor[0].getHumidity(), 1);
  }
  else if(var == "TEMPERATURE2"){
    return String(dhtSensor[1].getTemperature(), 1);
  }
    else if(var == "HUMIDITY2"){
    return String(dhtSensor[0].getHumidity(), 1);
  }
  return String();
}

void setup()
{
  Serial.begin(115200);
  delay(4000);

  Serial.printf("Serve sensor data from %d DHT Temp/Humidity sensors\n", NUM_SENSORS);
  dhtPin[0] = 21;
  dhtPin[1] = 22;

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    dhtSensor[i].setup(dhtPin[i], DHTesp::DHT22);
  }

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
    
    server.on("/", [](){
      server.send(200, "text/html", index_html);
    });

    server.on("/temperature1", [](){
      server.send(200, "text/plain", String(dhtSensor[0].getTemperature(),1));
    });
    server.on("/temperature2", [](){
      server.send(200, "text/plain", String(dhtSensor[1].getTemperature(),1));
    });
    server.on("/humidity1", [](){
      server.send(200, "text/plain", String(dhtSensor[0].getHumidity(),1));
    });
    server.on("/humidity2", [](){
      server.send(200, "text/plain", String(dhtSensor[1].getHumidity(),1));
    });

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
  /*      float v = 0.0;
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /temperature1")) {
          v = dhtSensor[1].getTemperature();
          Serial.printf("Got temp: %.1f\n", v);
          client.println(String(v,1));
          client.println();
          break;
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }

  */
}
