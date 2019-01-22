// You MUST download and include the IRLib2 library under your Arduino Libraries folder
// https://github.com/cyborg5/IRLib2/archive/master.zip
// See: https://learn.adafruit.com/using-an-infrared-library/overview
// The IR Transmitter module Signal "S" pin must connect to the Arduino Pin 3.
#include <IRLibAll.h>

// The following constants map to the numbers 0-9 on the TiVo HD Remote Control
// To the correct IR transmitter remote control codes. Captured directly from the// remote using the AnalysIR software from https://www.AnalysIR.com/ 
#define TivoNum0 0xA10C8C03
#define TivoNum9 0xA10C0C03
#define TivoNum8 0xA10CF40B
#define TivoNum7 0xA10C740B
#define TivoNum6 0xA10CB40B
#define TivoNum5 0xA10C340B
#define TivoNum4 0xA10CD40B
#define TivoNum3 0xA10C540B
#define TivoNum2 0xA10C940B
#define TivoNum1 0xA10C140B

int khz=38;       // Carrier Frequency For IR Transmitter.
int bits=32;      // Bits used for the NEC protocol.
IRsend mySender;  // Used to talk to the IR module on Pin 3.

// The digit extract doesn't handle unpopulated positions.
// Start at 10000 so that positions 1-4 are always populated.
// This is a hack, however it's also useful because you
// know which full cycle it is on based on the first digits.
// For example, 28759 would be the 2nd run through the full 
// set of 10K codes, and it is at number 8759.
int currentNumber=10000;  // Default Value: 10000
 
void setup() {
  //Initiate Serial communication.
  // Serial.begin(115200); // This rate is faster, but you need to set your serial window to match the speed.
  Serial.begin(9600);
}

void loop() {

  // Log progress
  Serial.print("Number: ");
  Serial.println(currentNumber);

  // Send the number to the IR Transmitter
  SendNumber(currentNumber);

  // Increment the number to process on next loop
  currentNumber++;
}

// Process a number to send each individual digit
void SendNumber(int number){  
  int digit = 0;
  
  // Send Pos 4 first as Tivo enters Left to Right on screen 
  for(int pos = 4; pos >= 1; pos--) {

    digit = ExtractDigit(number, pos);

    // Send a single digit to IR Transmitter
    SendDigit(digit);

    // Wait 500 milliseconds (0.5 seconds) between each digit. I have had this work at 300, but that might not work for you.
    delay(500); 
  } 
}

// Map a single digit from the number, to the correct TiVo 
// Remote Control code, and send the code.
void SendDigit(int digit){
  const uint32_t digitData[10] = { TivoNum0, TivoNum1, TivoNum2, TivoNum3, TivoNum4,
                                    TivoNum5, TivoNum6, TivoNum7, TivoNum8, TivoNum9 };
  Serial.print("Digit ");
  Serial.print(digit);
  Serial.print(" ");
  SendData(digitData[digit]);
}

// Send a control code via IR Transmitter
void SendData(uint32_t data) {
  Serial.println(data, HEX);
  mySender.send(NEC,data, bits);  
}

// Source for Extract Digit:
// http://forum.arduino.cc/index.php?topic=307479.0
// Note, does NOT return 0 if you choose a position that is unpopulated.
// For example: ExtractDigit(234,5) is Undefined and can return garbage values.
int ExtractDigit(int V, int P)
{
  return int(V/(pow(10,P-1))) - int(V/(pow(10,P)))*10; 
}
