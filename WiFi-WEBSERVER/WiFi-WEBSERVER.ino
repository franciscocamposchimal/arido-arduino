#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Pineaple-mansion";
const char* password = "holdthedoor";
// ip: 192.168.4.1
//  WiFi.softAPdisconnect();
AsyncWebServer server(80);

const char* PARAM_SSID = "ssid";
const char* PARAM_PASS = "pass";
const char* PARAM_SUB  = "sub";
const char* PARAM_PUB  = "pub";
const char* PARAM_SEN  = "sensor";

int dotsCount = 0;

String netList = "[";

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
        <div id="avNets"></div>
        <form style="
        display: flex; 
        flex-direction: column; 
        align-items: start;">
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
            cursor: pointer;" id="snd" value="Guardar">
        </form>
    </div>
</body>
<script>
    const ulNet = document.getElementById("avNets");
    const ssidInput = document.getElementById("lssid");
    const passInput = document.getElementById("lpass");
    const subInput = document.getElementById("ltopicsubs");
    const pubInput = document.getElementById("ltopicpub");
    const senInput = document.getElementById("lsensor");
    const saveBtn = document.getElementById("snd");
    const xhr = new XMLHttpRequest();
    
    xhr.open("GET", "/networks", true);
    xhr.responseType = 'json';
    xhr.onload = () => {
        const listNets = xhr.response; 
        listNets.forEach(netItem => {
           const li = document.createElement('li');
           li.appendChild(document.createTextNode(netItem));
           li.className = 'item';
           ulNet.appendChild(li);
        });
    };
    xhr.send();
    ulNet.addEventListener("click", (e) => {
        ssidInput.value = e.target.innerText;
    });
    saveBtn.addEventListener("click", () => {
        
        const dataToSend = {
            ssid: ssidInput.value,
            pass: passInput.value,
            sub: subInput.value,
            pub: pubInput.value,
            sensor: senInput.value 
        };
        console.log(dataToSend);
    });
</script></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void scan_networks(){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    Serial.println("Setup done");
    Serial.println("scan start");
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            if((i+1) == n){
                netList += "\""+WiFi.SSID(i)+"\"]";
            } else {
                netList += "\""+WiFi.SSID(i)+"\",";
            }
            
            delay(10);
        }
        WiFi.disconnect();
    }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if(dotsCount == 5) {
          Serial.println("");
          dotsCount = 0;
    } else {
        Serial.print(".");
        dotsCount++;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected...");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  
  scan_networks();
  setup_wifi();
  
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

  server.on("/networks", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println(netList);
      request->send(200, "text/plain", netList);
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop(){
    //to-do
}
