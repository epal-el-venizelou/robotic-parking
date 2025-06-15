#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Replace with your network credentials
const char* ssid = "username";
const char* password = "password";

// Replace with your server's IP or domain and endpoint
const char* server = "users.sch.gr";  // Replace with your server URL

// WiFi Client object
WiFiClient client;

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if data is available in the Serial buffer
  if (Serial.available() > 0) {
    // Read the value from Serial
    String value = Serial.readStringUntil('\n');
    value.trim();  // Remove any extra whitespace or newline characters
    
    // Ensure the value is a valid integer between 0 and 10
    int intValue = value.toInt();
    
    // If it's a valid integer and within the desired range
    if (intValue >= 0 && intValue <= 10) {
      // Print the value to Serial Monitor
      Serial.print("Received Value: ");
      Serial.println(intValue);

      // Send the value to the server via GET request
      if (client.connect(server, 80)) {
        String url = "/*******/parking/update.php?id=1&totalspaces=10&availablespaces=" + String(intValue);  // το URL έχει τροποποιηθεί για λόγους ασφαλείας
        Serial.print("Requesting URL: ");
        Serial.println(url);

        // Send HTTP GET request
        client.print("GET " + url + " HTTP/1.1\r\n");
        client.print("Host: users.sch.gr\r\n");  // Change this to your server's host
        client.print("Connection: close\r\n\r\n");

        // Wait for the server to respond
        while (client.available()) {
          String response = client.readStringUntil('\r');
          Serial.print(response);
        }
        Serial.println();
      } else {
        Serial.println("Connection failed.");
      }
    } else {
      // If the input value is not valid (not an integer between 0 and 10)
      Serial.println("Invalid input. Please enter a number between 0 and 10.");
    }
  }

  // Wait 5 seconds before reading new data from Serial (optional)
  delay(5000);
}
