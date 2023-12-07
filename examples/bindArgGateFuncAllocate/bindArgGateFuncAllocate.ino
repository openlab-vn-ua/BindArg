#include "BindArg.h"

// Simple xample on how to use bindArgGateFuncAllocate to call object methods via void func(type *) function

struct MyType
{
  const char *text;
};

static MyType foo = { text:"My name is foo" };
static MyType bar = { text:"My name is bar" };

void sayHello(MyType *arg)
{
  Serial.print("Hello:");
  Serial.print(arg->text);
  Serial.println();
}

bindArgVoidFunc_t fooCall; // function will be void func(void)
bindArgVoidFunc_t barCall; // function will be void func(void)

void setup()
{
  Serial.begin(115200); while(!Serial) { };

  fooCall = bindArgGateFuncAllocate(&sayHello, &foo); // Will call foo.sayHello()
  barCall = bindArgGateFuncAllocate(&sayHello, &bar); // Will call bar.sayHello()

  Serial.println("Hi!");
}

void loop()
{
  Serial.println("Hello from classes:");

  fooCall(); // No params supplied here, but actual `this` for `foo` object will be passed
  barCall(); // No params supplied here, but actual `this` for `bar` object will be passed

  delay(1000);
}
