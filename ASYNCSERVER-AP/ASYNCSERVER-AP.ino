#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid     = "ARIDO-SC-AP";
const char* password = "hailhydra";
// ip: 192.168.4.1
AsyncWebServer server(80);

const char* PARAM_SSID = "ssid";
const char* PARAM_PASS = "pass";
const char* PARAM_SUB  = "sub";
const char* PARAM_PUB  = "pub";
const char* PARAM_SEN  = "sensor";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ARIDO</title>
</head>

<body>
    <div style="
    display: flex; 
    flex-direction: column; 
    align-items: center; 
    justify-content: center;">
        <h3>ARIDO SMART CONFIG</h3>
        %TOSEND%
        <form style="
        display: flex; 
        flex-direction: column; 
        align-items: start;" action="/">
            <label for="lssid">SSID</label>
            <input type="text" id="lssid" name="ssid" placeholder="SSID..">

            <label for="lpass">PASS</label>
            <input type="text" id="lpass" name="pass" placeholder="Pass..">

            <label for="ltopicsubs">TOPIC SUBSCRIBE</label>
            <input type="text" id="ltopicsubs" name="sub" placeholder="Topic subscribe..">

            <label for="ltopicpub">TOPIC PUBLISH</label>
            <input type="text" id="ltopicpub" name="pub" placeholder="Topic publish..">

            <label for="lsensor">SENSOR</label>
            <select id="lsensor" name="sensor">
                <option value="temp">Temperatura</option>
                <option value="press">Presion</option>
                <option value="other">Otro</option>
            </select>
            <input style="
            border: none;
            padding: 10px 15px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            transition-duration: 0.4s;
            background-color: white;
            color: black;
            border: 2px solid #008CBA;
            cursor: pointer;" type="submit" value="Guardar">
        </form>
    </div>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String inputSSID = "NO SSID";
      String inputPASS = "NO PASS";
      String inputPUB  = "NO PUB";
      String inputSUB  = "NO SUB";
      String inputSEN  = "NO SEN";
      
      if (request->hasParam(PARAM_SSID)) {
          inputSSID = request->getParam(PARAM_SSID)->value();
      }
      if (request->hasParam(PARAM_PASS)) {
          inputPASS = request->getParam(PARAM_PASS)->value();
      }
      if (request->hasParam(PARAM_PUB)) {
          inputPUB = request->getParam(PARAM_PUB)->value();
      }
      if (request->hasParam(PARAM_SUB)) {
          inputSUB = request->getParam(PARAM_SUB)->value();
      }
      if (request->hasParam(PARAM_SEN)) {
          inputSEN = request->getParam(PARAM_SEN)->value();
      }

    request->send_P(200, "text/html", index_html);
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop(){
    //to-do
}
