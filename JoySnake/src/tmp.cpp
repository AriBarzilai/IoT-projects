


// #define CLK_PIN 25
// #define DT_PIN 26            
// #define SW_PIN 27

// #define DIRECTION_CW 0   // clockwise direction
// #define DIRECTION_CCW 1  // counter-clockwise direction

// volatile int counter = 0;
// volatile int direction = DIRECTION_CW;
// volatile unsigned long last_time;  // for debouncing
// int prev_counter;



// void setup() {
//   Serial.begin(9600);

//   // configure encoder pins as inputs
//   pinMode(CLK_PIN, INPUT);
//   pinMode(DT_PIN, INPUT);
  

//   // use interrupt for CLK pin is enough
//   // call ISR_encoderChange() when CLK pin changes from LOW to HIGH
//   attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_encoderChange, RISING);
//   attachInterrupt(digitalPinToInterrupt(SW_PIN), handleButtonInterrupt, RISING);

// }

// void loop() {
//   button.loop();  // MUST call the loop() function first

//   if (prev_counter != counter) {
//     Serial.print("DIRECTION: ");
//     if (direction == DIRECTION_CW)
//       Serial.print("Clockwise");
//     else
//       Serial.print("Counter-clockwise");

//     Serial.print(" | COUNTER: ");
//     Serial.println(counter);

//     prev_counter = counter;
//   }

//   if (button.isPressed()) {
//     Serial.println("The button is pressed");
//   }

//   // TO DO: your other work here
// }

// void ISR_encoderChange() {
//   if ((millis() - last_time) < 50)  // debounce time is 50ms
//     return;

//   if (digitalRead(DT_PIN) == HIGH) {
//     // the encoder is rotating in counter-clockwise direction => decrease the counter
//     counter--;
//     direction = DIRECTION_CCW;
//   } else {
//     // the encoder is rotating in clockwise direction => increase the counter
//     counter++;
//     direction = DIRECTION_CW;
//   }

//   last_time = millis();
// }

// void handleButtonInterrupt()
// {
//   if ((millis() - last_time) < 50)  // debounce time is 50ms
//     return;
//   last_time = millis();
//   Serial.println("Button pressed");
// }