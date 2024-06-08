#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <map>

#define POWER_PIN 12

// Replace with your network credentials
const String ssid = "ssid";
const char* password = "password";

// Google Script URL
const char* scriptURL = "script url";

// Example cell names, you can replace them with the actual cell names you want to check
String cellNames[] = {"powerBtn", "forceSD"};
String cellRequest = "[";

std::map<String, String> cellValues;
// Map to store cell names and corresponding functions
std::map<String, std::function<void()>> cellFunctions;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Add your cell names and corresponding functions to the map
  cellFunctions["powerBtn"] = powerBtnFunction;
  cellValues["powerBtn"] = "0";
  cellFunctions["forceSD"] = forceShutdownFunction;
  cellValues["forceSD"] = "0";

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  pinMode(POWER_PIN, OUTPUT);

  for (int i = 0; i < sizeof(cellNames) / sizeof(cellNames[0]); i++) {
    cellRequest += "\"" +cellNames[i] +"\"";
    if(i!=(sizeof(cellNames)/sizeof(cellNames[0])-1))
      cellRequest += ",";
  }
  cellRequest += "]";
}

void loop() {
  httpGet();
  doFunctions();
  doPosts();
  delay(5000);
}


//http part
void httpGet() {
  HTTPClient http;
  Serial.println("Making a request");

  // Use a WiFiClient object to begin the HTTP request
  WiFiClientSecure client;
  client.setInsecure();   // no need for a certificate
    
  // Construct the URL with the JSON payload
  String urlWithParam = (String)scriptURL + "?cellNames=" + cellRequest;
  
  http.begin(client, urlWithParam.c_str()); // Specify the WiFiClient and URL
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  Serial.println("url: " +urlWithParam);
  int httpCode = http.GET();
  String payload;
  Serial.println(httpCode);

  if (httpCode > 0) { // Check for the returning code
    payload = http.getString();
    Serial.println("JSON Response: " + payload);

    // Parse JSON response
    DynamicJsonDocument responseDoc(1024); // Adjust the size based on your JSON response
    deserializeJson(responseDoc, payload);

    // Iterate through the JSON object and handle the values
    for (int i = 0; i < sizeof(cellNames) / sizeof(cellNames[0]); i++) {
      String cellValue = responseDoc[cellNames[i]].as<String>();
      Serial.println("Cell " + cellNames[i] + ": " + cellValue);

      // Check the value and execute the corresponding function
      if (cellValue == "1") {
        cellValues[cellNames[i]] = "1";
      }
    }
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}

void doPosts(){
  for (const auto& pair : cellValues) {
        if(pair.second != "0"){
          httpPost(pair.first, "0");
          cellValues[pair.first] = "0";
        }
    }
}

void httpPost(const String& cellName, const String& cellValue) {
  HTTPClient http;

  // Use a WiFiClientSecure object to begin the HTTP request
  WiFiClientSecure client;
  client.setInsecure();   // no need for a certificate

  // Construct the URL
  String url = scriptURL;

  // Set the data to be sent in the POST request
  String postData = "{\"cellName\":\"" + cellName + "\",\"cellValue\":\"" + cellValue + "\"}";
  Serial.println(postData);
  // Begin the HTTP request
  http.begin(client, url.c_str()); // Specify the WiFiClient and URL
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");

  // Send the POST request
  int httpCode = http.POST(postData);
  Serial.println("HTTP code: " +(String)httpCode);
  String payload;

  if (httpCode > 0) { // Check for the returning code
    payload = http.getString();
    Serial.println("POST Response: " + payload);
    // Handle the response as needed
  } else {
    Serial.println("Error on HTTP POST request");
  }

  // End the HTTP request
  http.end();
}


// funtions part
void doFunctions(){
  for (const auto& pair : cellValues) {
        if(pair.second == "1"){
          cellFunctions[pair.first]();
        }
    }
}


void powerBtnFunction() {
  Serial.println("Executing powerBtn function");
  digitalWrite(POWER_PIN, HIGH);
  delay(250);
  digitalWrite(POWER_PIN, LOW);
}

void forceShutdownFunction() {
  Serial.println("Executing forceShutdown function");
  digitalWrite(POWER_PIN, HIGH);
  delay(5000);
  digitalWrite(POWER_PIN, LOW);
}
