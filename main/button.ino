int buttonPin = D3;
int buttonPrevious = LOW;    // the previous reading from the input pin
int buttonReading;

void buttonLoop() {

  buttonReading = digitalRead(buttonPin);

  //Serial.print("buttonReading:   ");
  //Serial.println(buttonReading);

  //Serial.print("buttonPrevious:   ");
  //Serial.println(buttonPrevious);

  if (buttonReading != buttonPrevious) {
    state = !state;
    digitalWrite(relayPin, state);
    client.publish("/home/relay/state/", (state == LOW ? "ON" : "OFF"));
  }

    buttonPrevious = buttonReading;

}

bool buttonSetup() {
  Serial.println("enter buttonSetup()");
  pinMode(buttonPin, INPUT);
  delay(100);
  Serial.println("button INPUT done \n end of buttonSetup() ");
  return true;
}

