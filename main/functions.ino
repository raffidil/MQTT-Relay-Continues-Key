int firstWifiloop = 0;

void callback(char* p_topic, byte* p_payload, unsigned int p_length) {

  //convert topic to string to make it easier to work with
  String topicStr = p_topic;
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  //EJ: Note: the "topic" value gets overwritten everytime it receives confirmation (callback) message from MQTT

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

  if (topicStr == relay_set_channel) {

    if (payload.equals(String("ON"))) {
      digitalWrite(relayPin, LOW);
      delay(200);
      state = LOW;
      client.publish(relay_state_channel, "ON");
    }

    else if (payload.equals(String("OFF"))) {
      digitalWrite(relayPin, HIGH);
      delay(200);
      state = HIGH;
      client.publish(relay_state_channel, "OFF");
    }
  }

  if (topicStr == led_set_channel) {
    Serial.print("led color: ");
    Serial.println(payload);

  //convert string payload to char array
    int n = payload.length(); 
    char payload_char_array[n+1]; 
    strcpy(payload_char_array, payload.c_str()); 
    
    long long number = strtol(payload_char_array, NULL, 16);
    long long r = number >> 16;
    long long g = number >> 8 & 0xFF;
    long long b = number & 0xFF;
    delay(200);
    ledColor(r, g, b);
    client.publish(led_state_channel, payload_char_array);
  }

}

void ledColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(0, r, g, b);
  strip.show();
}


void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection

    while (WiFi.status() != WL_CONNECTED && firstWifiloop < 20 ) {
      delay(500);
      Serial.print(".");
      firstWifiloop++;
      ledColor(255, 0, 0);
    }
    if (WiFi.status() == WL_CONNECTED) {
      //print out some more debug once connected
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      ledColor(0, 255, 0);
      delay(1000);
    }
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      //EJ: Delete "mqtt_username", and "mqtt_password" here if you are not using any
      if (client.connect((char*) clientName.c_str())) { //EJ: Update accordingly with your MQTT account
        Serial.println("\tMQTT Connected");
        ledColor(0, 0, 0);
        client.subscribe(relay_set_channel);
        client.subscribe(led_set_channel);

        //EJ: Do not forget to replicate the above line if you will have more than the above number of relay switches
      }

      //otherwise print failed for debugging
      else {
        Serial.println("\tFailed.");
        ledColor(255, 0, 255);
        break;
      }
    }
  }

}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac) {

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5) {
      result += ':';
    }
  }

  return result;
}
