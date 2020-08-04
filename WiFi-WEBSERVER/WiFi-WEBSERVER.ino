#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

const char* ssid = "Pineaple-mansion";
const char* password = "holdthedoor";
// ip: 192.168.4.1
//  WiFi.softAPdisconnect();
AsyncWebServer server(80);

int dotsCount = 0;

String netList = "[";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>ARIDO</title>
    <style type="text/css">
        * {box-sizing: border-box;font-family: sans-serif;}
        h3 {
            text-shadow: 1px 1px 2px #F29A63, 0 0 1em #F7C797, 0 0 0.2em #F5DBBC;
            color: #E04C2B;
        }
        .center {display: flex;justify-content: center;align-items: center;}
        input[type=text],
        select,
        textarea {
            width: 100%;
            padding: 12px;
            border: 1px solid #ccc;
            border-radius: 4px;
            resize: vertical;
        }
        .list {
            width: 100%;
            padding: 12px;
            border: 1px solid #ccc;
            border-radius: 4px;
            resize: vertical;
        }
        label {padding: 12px 12px 12px 0;display: inline-block;
        }
        #snd {
            background-color: #4CAF50;
            color: white;
            width: 100%;
            padding: 12px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            margin-top: 6px;
        }
        #snd:hover {background-color: #45a049;}
        .container {
            width: 35%;
            border-radius: 5px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center
        }
        .col-75 {float: left;width: 100%;margin-top: 1%;}
        @media screen and (max-width: 600px) {
            .col-25,
            .col-75,
            .container,
            #snd {
                width: 100%;
                margin-top: 6px;
            }
        }
        .chip {
            vertical-align: auto;
            display: inline-block;
            margin-right: 5px;
            margin-top: 5px;
            line-height: 2;
            padding: 0 5px;
            height: 30px;
            font-size: 16px;
            border-radius: 25px;
            background-color: #F5DBBC;
        }
    </style>
</head>
<body>
    <div class="center">
        <div class="container">
            <h3 class="title">ARIDO SMART CONFIG</h3>
            <div id="networksList"></div>
            <form style="display: flex; flex-direction: column; align-items: start;">
                <div class="row">
                    <div class="col-75">
                        <label for="lssid">SSID</label>
                        <input type="text" id="lssid" name="ssid" placeholder="SSID.." />
                    </div>
                    <div class="col-75">
                        <label for="lpass">PASS</label>
                        <input type="text" id="lpass" name="pass" placeholder="Pass.." />
                    </div>
                    <div class="col-75">
                        <label for="lbroker">IP BROKER SERVER</label>
                        <input type="text" id="lbroker" name="brokerServer" placeholder="Ip server.." />
                    </div>
                    <div class="col-75">
                        <label for="ltopicsubs">TOPIC SUBSCRIBE</label>
                        <input type="text" id="ltopicsubs" name="sub" placeholder="Topic subscribe.." />
                    </div>
                    <div class="col-75">
                        <label for="ltopicpub">TOPIC PUBLISH</label>
                        <input type="text" id="ltopicpub" name="pub" placeholder="Topic publish.." />
                    </div>
                    <div class="col-75">
                        <label for="lsensor">SENSOR</label>
                        <select id="lsensor" name="sensor">
                            <option value="temp">Temperatura</option>
                            <option value="press">Presion</option>
                            <option value="other">Otro</option>
                        </select>
                    </div>
                    <div class="col-75">
                        <input class="btnSave" id="snd" value="Guardar" />
                    </div>
                </div>
            </form>
        </div>
    </div>
</body>
<script>
    const ssidInput = document.getElementById("lssid");
    const passInput = document.getElementById("lpass");
    const brokerServer = document.getElementById("lbroker");
    const subInput = document.getElementById("ltopicsubs");
    const pubInput = document.getElementById("ltopicpub");
    const senInput = document.getElementById("lsensor");
    const saveBtn = document.getElementById("snd");
    const xhr = new XMLHttpRequest();
    const xhrPOST = new XMLHttpRequest();
    
    xhr.open("GET", "/networks", true);
    xhr.responseType = 'json';
    xhr.onload = () => {
        const listNets = xhr.response; 
        listNets.forEach((netItem) => {
            const element = document.createElement("div");
            const text = document.createTextNode(netItem);
            element.className = "chip";
            element.onclick = () => {
                ssidInput.value = netItem;
            };
            element.appendChild(text);
            document.getElementById("networksList").appendChild(element);
        });
    };
    xhr.send();
    
    saveBtn.addEventListener("click", () => {
        const data = {
            ssid: ssidInput.value,
            pass: passInput.value,
            ipBroker: brokerServer.value,
            sub: subInput.value,
            pub: pubInput.value,
            sensor: senInput.value,
        };
        console.log(data);
        xhrPOST.open('POST', '/config', true);
        xhrPOST.setRequestHeader('Content-Type', 'application/json');
        xhrPOST.responseType = 'json';
        xhrPOST.onload = () => {
          console.log(xhrPOST.status);
          console.log(xhrPOST.response);
        };
        xhrPOST.send(JSON.stringify(data));
    });
</script></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void scan_networks() {
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
      if ((i + 1) == n) {
        netList += "\"" + WiFi.SSID(i) + "\"]";
      } else {
        netList += "\"" + WiFi.SSID(i) + "\",";
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
    if (dotsCount == 5) {
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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/networks", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", netList);
  });

  server.on("/config", HTTP_POST, [](AsyncWebServerRequest * request) {}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, (char *)data);
    if (error) {
      request->send(400, "application/json", "{\"code\":0,\"message\":\"Json parse syntax error..\"}");
    }

    const char* PARAM_SSID = doc["ssid"];
    const char* PARAM_PASS = doc["pass"];
    const char* PARAM_BROKER = doc["ipBroker"];
    const char* PARAM_SUB = doc["sub"];
    const char* PARAM_PUB = doc["pub"];
    const char* PARAM_SEN = doc["sensor"];


    if (PARAM_SSID && !PARAM_SSID[0]) {
      request->send(400, "application/json", "{\"code\":1,\"message\":\"SSID not found..\"}");
    }
    if (PARAM_PASS && !PARAM_PASS[0]) {
      request->send(400, "application/json", "{\"code\":2,\"message\":\"PASS not found..\"}");
    }
    if (PARAM_BROKER && !PARAM_BROKER[0]) {
      request->send(400, "application/json", "{\"code\":3,\"message\":\"BROKER IP not found..\"}");
    }
    if (PARAM_SUB && !PARAM_SUB[0]) {
      request->send(400, "application/json", "{\"code\":4,\"message\":\"SUB not found..\"}");
    }
    if (PARAM_PUB && !PARAM_PUB[0]) {
      request->send(400, "application/json", "{\"code\":5,\"message\":\"PUB not found..\"}");
    }
    if (PARAM_SEN && !PARAM_SEN[0]) {
      request->send(400, "application/json", "{\"code\":6,\"message\":\"SENSOR not found..\"}");
    }

    request->send(200, "application/json", "{\"message\":\"SUCCESS, all data is stored..\"}");
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  //to-do
}
