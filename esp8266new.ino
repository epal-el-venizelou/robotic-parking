#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Replace with your network credentials

const char* ssid = "*********";
const char* password = "aaaaaaaa";
const int totalspaces=5;
int freespaces=totalspaces;
const int id=1;
// Replace with the URL where you want to send the data
const char* serverName = "http://users.sch.gr/*******/testparking/update.php";  // PHP file to handle the request
long delayBetweenMessages=0;
String message = "Hello from ESP8266";  // The message you want to send (this can be modified)

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  sendMessage();  // Send message to the server
  delay(200);
}

void loop() {
  // You can modify this to send a new message or repeat the action as needed
  //delay(10000);  // Wait for 10 seconds before sending the message again
 if (Serial.available() > 0) {
    // Αν υπάρχει διαθέσιμο δεδομένο στη σειριακή θύρα
    freespaces = Serial.parseInt();  // Ανάγνωση του αριθμού από τη σειριακή θύρα
    if(millis()-delayBetweenMessages>5000)
    {
    sendMessage();
    Serial.println("got:" + String(freespaces));
    delayBetweenMessages=millis();
    }
 }

}

void sendMessage() {
  if(WiFi.status() == WL_CONNECTED) {  // Ensure Wi-Fi is connected
    HTTPClient http;
    WiFiClient wifi;
    // Create the URL with the message data
    String url = serverName;
    url += "?id=" + String(id) ;  // Append the message as a URL parameter
    url += "&totalSpaces=" + String(totalspaces);
    url += "&freespaces=" + String(freespaces);
    Serial.println(url);
    http.begin(wifi,url);  // Start the connection
    int httpCode = http.GET();  // Make the GET request
    
    if (httpCode > 0) {
      Serial.println("Message Sent!");
      String payload = http.getString();  // Get the server response (optional)
      Serial.println(payload);  // Print server response (if any)
    } else {
      Serial.println("Failed to send message");
    }
    
    http.end();  // End the HTTP connection
  } else {
    Serial.println("WiFi Disconnected!");
  }
}
