#include "BindArg.h"

// Simple xample on how to use bindArgGateFuncAllocate to call object methods via void func(type *) function

struct MyType
{
  const char *text;
};

static MyType foo = { text:"My name is foo" };
static MyType bar = { text:"My name is bar" };

void sayHello(void *argVoid)
{
  MyType *arg = static_cast<MyType*>(argVoid);
  Serial.print("Hello:");
  Serial.print(arg->text);
  Serial.println();
}

bindArgVoidFunc_t fooCall; // function will be void func(void)
bindArgVoidFunc_t barCall; // function will be void func(void)

void setup()
{
  Serial.begin(115200); while(!Serial) { };

  fooCall = bindArgGateVoidAllocate(&sayHello, &foo); // Will call foo.sayHello()
  barCall = bindArgGateVoidAllocate(&sayHello, &bar); // Will call bar.sayHello()

  Serial.println("Hi!");
}

void loop()
{
  Serial.println("Hello from classes:");

  fooCall(); // No params supplied here, but actual pointer to `foo` struct will be passed (as void*)
  barCall(); // No params supplied here, but actual pointer to `bar` struct will be passed (as void*)

  delay(1000);
}
