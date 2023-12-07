# BindArg
![Build CI Status](https://github.com/openlab-vn-ua/BindArg/actions/workflows/githubci.yml/badge.svg)
![Lib Lint Status](https://github.com/openlab-vn-ua/BindArg/actions/workflows/arduino-lint-lib.yml/badge.svg)

Binder to allow use functions with pointer argument in Arduino interrupt handlers where only void func(void) allowed.

Useful to use object instance methods as interrupt handlers.

The library is free software: you can redistribute it and/or modify it under the terms of the MIT License.

# Workflow

Allocate bind arg gate via `bindArgGateThisAllocate` or `bindArgGateFuncAllocate`, and supply function pointer returned to `attachInterrupt` or alike.

Free bind arg gate when you no longer need it via `bindArgGateFree` after `detachInterrupt`

# Example
```cpp
//Example (How to use object instance method as interrupt handler):
class MyDevice
{
  bindArgVoidFunc_t interruptGate = nullptr;
  int               interruptNum;

  public:
  void interruptHandler()
  {
    //You may use "this" here
  }

  void begin(int deviceInterruptPin)
  {
    //...
    interruptNum = digitalPinToInterrupt(deviceInterruptPin);
    //...
    interruptGate = bindArgGateThisAllocate(&MyDevice::interruptHandler, this);
    attachInterrupt(interruptNum, interruptGate, CHANGE);
    //...
  }

  void end()
  {
    //...
    detachInterrupt(interruptNum);
    bindArgGateFree(interruptGate);
    interruptGate = nullptr;
    //...
  }
};
```

# API Reference

## bindArgGateThisAllocate
```cpp
bindArgVoidFunc_t bindArgGateThisAllocate(void (T::*method)(), T* objPtr);
```

Allocates `void gate(void)` bind gate for the object method call.

The gate calls the original func with object pointer argument supplied at gate allocation time (so argument "bound" with func at gate).
The function is equivalent of `gate = std::bind(&MyClass::method, this)` or `gate = std::bind(&MyClass::method, &myObject)`, 
but returns "pure" `void gate(void)` function. Takes an object as an argument to be passed to the method (usually `this`).

The function is useful when attaching interrupt handlers to object methods.

**param** method function pointer of an object that takes no arguments and returns void

**param** objPtr object pointer to be supplied to method at this pointer

**return** function that has `void gate(void)` prototype to be used as a callback (or interrupt handler) or nullptr if no more gates are available

**note** The number of available gates is limited (~30), so if you ever do not need the gate you should free it via `bindArgGateFree`

## bindArgGateFuncAllocate
```cpp
bindArgVoidFunc_t bindArgGateFuncAllocate(void (*func)(T*), T* arg);
```
Allocates `void gate(void)` bind gate for typed `void func(type *arg)` function + `type *arg`. 

The gate calls original func with the argument supplied at gate allocation time (so argument "bound" with func at gate).
The function is equivalent of `gate = std::bind(func, arg)`, but returns "pure" `void gate(void)` function.
Takes `void func(type *arg)` func and `type *arg` parameter to be passed to the func when bind gate is called.

Function is useful when attaching interrupt handlers, that need object/struct-like arguments

**param** func function `void func(type *arg)` to be to bind 

**param** arg `type *` parameter to be supplied to func when gate will be called

**return** function that has `void gate(void)` prototype to be used as a callback (or interrupt handler) or nullptr if no more gates are available

**note** The number of available gates is limited (~30), so if you ever do not need the gate you should free it via `bindArgGateFree`

## bindArgGateVoidAllocate
```cpp
bindArgVoidFunc_t bindArgGateVoidAllocate(void (*func)(void *), void* arg);
```
Allocates `void gate(void)` bind gate for untyped `void func(void *arg)` function + `void *arg`. 

The gate calls original func with the argument supplied at gate allocation time (so argument "bound" with func at gate).
The function is equivalent of `gate = std::bind(func, arg)`, but returns "pure" `void gate(void)` function.
Takes `void func(void *arg)` func and `void *arg` parameter to be passed to the func when bind gate is called.

*This is a more expert and old-school C-like style of passing user-defined arguments, but in most cases, the use of `bindArgGateFuncAllocate` is preferred as more type-safe.*

Function is useful when attaching interrupt handlers, that need object/struct-like arguments

**param** func function `void func(void *arg)` to be to bind 

**param** arg `void *` parameter to be supplied to func when gate will be called

**return** function that has `void gate(void)` prototype to be used as a callback (or interrupt handler) or nullptr if no more gates are available

**note** The number of available gates is limited (~30), so if you ever do not need the gate you should free it via `bindArgGateFree`

## bindArgGateFree
```cpp
void bindArgGateFree(bindArgVoidFunc_t gate);
```

Frees `void gate(void)` bind gate allocated by one of allocation `bindArgGate....Allocate` functions.

**note** The number of available gates is limited (~30), so if you ever do not need the gate you should free it via `bindArgGateFree`

**param** gate object allocates by one of `bindArgGate....Allocate` functions (if nullptr, the request is safely ignored).


