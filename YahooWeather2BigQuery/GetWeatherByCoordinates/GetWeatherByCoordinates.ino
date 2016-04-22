#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information   

// We limit this so you won't use all of your Temboo calls while testing
int maxCalls = 10;

// The number of times this Choreo has been run so far in this sketch
int calls = 0;


int choreoInterval = 30000; // Choreo execution interval in milliseconds
uint32_t lastChoreoRunTime = millis() - choreoInterval; // store the time of the last Choreo execution

int streamInterval = 30000; // streaming interval in milliseconds
uint32_t lastStreamRunTime = millis() - streamInterval; // store the time of the last stream write


void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);
  Bridge.begin();
  
  Serial.println("Setup complete.\n");
}

void loop() {
  uint32_t now = millis(); // check current time
  if (now - lastChoreoRunTime >= choreoInterval) { // see if enough time has passed to run the Choreo
    if (calls < maxCalls) {
      Serial.println("Calling GetWeatherByCoordinates Choreo...");
      runGetWeatherByCoordinates();
      lastChoreoRunTime = now;
      calls++;
    } else {
      Serial.println("Skipping to save Temboo calls. Adjust maxCalls as required.");
    }
  }
  now = millis();
  if (now - lastStreamRunTime >= streamInterval) {
    lastStreamRunTime = now;
    stream();
  }
}

void stream() {
  TembooChoreo stream;

  // Invoke the Temboo client
  stream.begin();

  // Set Temboo account credentials
  stream.setAccountName(TEMBOO_ACCOUNT);
  stream.setAppKeyName(TEMBOO_APP_KEY_NAME);
  stream.setAppKey(TEMBOO_APP_KEY);

  // Identify the Choreo to run
  stream.setChoreo("/Library/Util/StreamSensorData");

  // Set the Streaming profile to use
  stream.setProfile("streamingTest");

  // Generate sensor data to stream
  String pinData = "{";
  pinData += "\"DIGITAL1\":" + String(digitalRead(1));
  pinData += ",";
  pinData += "\"DIGITAL2\":" + String(digitalRead(2));
  pinData += ",";
  pinData += "\"ANALOG2\":" + String(analogRead(A2));
  pinData += ",";
  pinData += "\"ANALOG1\":" + String(analogRead(A1));
  pinData += "}";

  // Add sensor data as an input to the streaming Choreo
  stream.addInput("SensorData", pinData);
  // NOTE: for debugging set "Async" to false (indicating that a response should be returned)
  // stream.addInput("Async", "false");

  // Stream the data; when results are available, print them to serial
  stream.run();

  while(stream.available()) {
    char c = stream.read();
    Serial.print(c);
  }
  stream.close();
}

void runGetWeatherByCoordinates() {
  TembooChoreo GetWeatherByCoordinatesChoreo;

  // Invoke the Temboo client
  GetWeatherByCoordinatesChoreo.begin();

  // Set Temboo account credentials
  GetWeatherByCoordinatesChoreo.setAccountName(TEMBOO_ACCOUNT);
  GetWeatherByCoordinatesChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  GetWeatherByCoordinatesChoreo.setAppKey(TEMBOO_APP_KEY);

  // Set profile to use for execution
  GetWeatherByCoordinatesChoreo.setProfile("TestYahooWeatherBigQuery");
  // Identify the Choreo to run
  GetWeatherByCoordinatesChoreo.setChoreo("/Library/Yahoo/Weather/GetWeatherByCoordinates");

  // Run the Choreo
  unsigned int returnCode = GetWeatherByCoordinatesChoreo.run();

  // Read and print the error message
  while (GetWeatherByCoordinatesChoreo.available()) {
    char c = GetWeatherByCoordinatesChoreo.read();
    Serial.print(c);
  }
  Serial.println();
  GetWeatherByCoordinatesChoreo.close();
}