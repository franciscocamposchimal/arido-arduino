// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "ARIDO-SC-AP";
const char* password = "hailhydra";
// ip: 192.168.4.1

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

void setup() {
  Serial.begin(115200);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                        
            // Display the HTML web page
            client.println("<!DOCTYPE html><html lang=\"es\">");
            client.println("<head><meta charset=\"UTF-8\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<title>ARIDO</title></head>");
    
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
            
            // Web Page Heading
            client.println("<body><div style=\"display: flex; flex-direction: column; align-items: center; justify-content: center;\">");
            client.println("<h3>ARIDO SMART CONFIG</h3>");

            client.println("</body></html>");
            client.println("<form style=\"display: flex; flex-direction: column; align-items: start;\">");
            // SSID input
            client.println("<label for=\"lssid\">SSID</label><input type=\"text\" id=\"lssid\" name=\"ssid\" placeholder=\"SSID..\">");
            // PASS
            client.println("<label for=\"lpass\">PASS</label><input type=\"text\" id=\"lpass\" name=\"pass\" placeholder=\"Pass..\">");
            // SUBSCRIBE TOPIC
            client.println("<label for=\"ltopicsubs\">TOPIC SUBSCRIBE</label><input type=\"text\" id=\"ltopicsubs\" name=\"topicsubs\" placeholder=\"Topic subscribe..\">");
            // PUBLISH TOPIC
            client.println("<label for=\"ltopicpub\">TOPIC PUBLISH</label><input type=\"text\" id=\"ltopicpub\" name=\"topicpub\" placeholder=\"Topic publish..\">");
            // SENSOR SELECT
            client.println("<label for=\"lsensor\">SENSOR</label>");
            client.println("<select id=\"lsensor\" name=\"sensor\">");
            client.println("<option value=\"temp\">Temperatura</option>");
            client.println("<option value=\"press\">Presion</option>");
            client.println("<option value=\"other\">Otro</option>");
            client.println("</select>");
            client.println("<button>Guardar</button>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
