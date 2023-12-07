#include <BindArg.h>

// Pin watcher: will proccess pin change state
// There may be mutiple instance on different pins (and interrupts)
class PinWatcher 
{
  protected:

  bindArgVoidFunc_t interruptGate = nullptr;
  int               interruptPin = -1;

  public:

  // this instance method will be called on interrupt
  void onInterrupt()
  {
    // You may use `this` here
    Serial.print("Pin ");
    Serial.print(this->interruptPin); // example: with explicit `this`
    Serial.print(" now ");
    Serial.print(digitalRead(interruptPin)); // example: with implicit `this`
    Serial.println();
  }

  // initialize
  void begin(int deviceInterruptPin)
  {
    if (deviceInterruptPin < 0) { return; } // just in case wrong params supplied
    pinMode(deviceInterruptPin, INPUT_PULLUP);
    interruptPin = deviceInterruptPin;
    interruptGate = bindArgGateThisAllocate(&PinWatcher::onInterrupt, this);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interruptGate, CHANGE);
  }

  // stop operation
  void end()
  {
    if (interruptPin < 0) { return; } // just in case someone call `end` without `begin`
    detachInterrupt(digitalPinToInterrupt(interruptPin));
    bindArgGateFree(interruptGate);
    interruptGate = nullptr;
  }

  // return true if dvice is initalized with `begin`
  bool isActive() { return interruptGate != nullptr; }
};

#define PIN_A 1
#define PIN_B 2

PinWatcher pinWatchA;
PinWatcher pinWatchB;

void setup()
{
  Serial.begin(115200); while(!Serial) { };
  Serial.println("Hi!");

  pinWatchA.begin(PIN_A);
  pinWatchB.begin(PIN_B);
}

void loop()
{
  delay(1000);
  Serial.println("[pulse]");
}
