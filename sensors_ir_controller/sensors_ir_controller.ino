// IR
const int IRdPin = 7;
int isFalling = 0;


void setup() {
  pinMode(IRdPin, INPUT);
  // Start serial communication
  Serial.begin(9600);
}

void loop() {
  isFalling = digitalRead(IRdPin);
  Serial.println(String(isFalling));
  delay(0.1); 
}
