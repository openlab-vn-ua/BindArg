#include "BindArg.h"

// Simple xample on how to use bindArgGateThisAllocate to call object methods via void func(void) function

class MyObject
{
  public:
  const char *text;

  void begin(const char *helloText)
  {
    this->text = helloText;
  }

  void sayHello(void)
  {
    // object method
    Serial.print("Hello:");
    Serial.print(this->text);
    Serial.println();
  }
};

MyObject foo;
MyObject bar;

bindArgVoidFunc_t fooCall; // function will be void func(void)
bindArgVoidFunc_t barCall; // function will be void func(void)

void setup()
{
  Serial.begin(115200); while(!Serial) { };

  foo.begin("My name is foo");
  bar.begin("My name is bar");

  fooCall = bindArgGateThisAllocate(&MyObject::sayHello, &foo); // Will call foo.sayHello()
  barCall = bindArgGateThisAllocate(&MyObject::sayHello, &bar); // Will call bar.sayHello()

  Serial.println("Hi!");
}

void loop()
{
  Serial.println("Hello from classes:");

  fooCall(); // No params supplied here, but actual `this` for `foo` object will be passed
  barCall(); // No params supplied here, but actual `this` for `bar` object will be passed

  delay(1000);
}
