#pragma once
/**
 * @file BindArg.h
 * Written by openlab.vn.ua
 *
 * Binder to allow use functions with pointer argument in Arduino interrupt handlers where only `void func(void)` allowed.
 * Usefull to use object instance methods as interrupt handlers.
 *
 * The library is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * The software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Developer's home:
 * https://github.com/openlab-vn-ua
 * (Open IT Laboratory from Vinnitsya, Ukraine)

@code
//Example (How use object instance method as interrupt handler):
class MyDevice
{
  bindArgVoidFunc_t interruptGate = nullptr;
  int               interruptNum;

  public:
  void interruptHandler()
  {
    // you may use "this" here
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
@endcode
*/

#ifndef BIND_ARG_H
#define BIND_ARG_H

typedef void  (*bindArgVoidFunc_t)(void); ///< Bind gate function returned after binding: `void gate(void)`

typedef void* bindArgProcArg_t; ///< Simpiest argument type to bind: `void *`
typedef void  (*bindArgProcFunc_t)(bindArgProcArg_t arg); ///< Function that is simpliest function to bind with arg: `void func(void *arg)`

/**
@brief Allocates `void gate(void)` bind gate for `void func(void *arg)` function + `void *arg`. 

The gate calls original func with argument supplied at gate allocation time (so argument "binded" with func at gate).
The function is equivalent of `gate = std::bind(func, arg)`, but returns "pure" `void gate(void)` function.
Takes `void func(void *arg)` func and `void *arg` parameter to be passed to the func when bind gate is called.

Function is useful when attaching interrupt handlers, that need object/struct-like arguments
The function may be used to attach `this` argument to method call, so object method may be used as interrupt handler, 
there is a special templated version of this function `bindArgGateThisAllocate` that may be used for this propose.
If you function has typed parameter (pointer to typed object, not just `void*`) 
there is a special templated version of this function `bindArgGateFuncAllocate` that may be used for this propose.

@param func function `void func(void *arg)` to be to bind 
@param arg `void *` parameter to be supplied to func when gate will be called
@return function that has `void gate(void)` prototype to be used as callback (or interrupt handler)
@note The number of available gates are limited (~30), so if you ever do not need the gate you should free it via `bindArgGateFree`

@code
//Example (How pass void *arg to interrupt handler):
struct MyData { }; // some data here
static MyData int1params;
static MyData int2params;
void myInterruptHandler(MyData *myParam = nullptr) { } // some code here ( = nullptr just to trick arduino compiler)
// with free
static bindArgVoidFunc_t int1gate;
attachInterrupt(digitalPinToInterrupt(1), int1gate = bindArgGateVoidAllocate((bindArgProcFunc_t)&myInterruptHandler, (bindArgProcArg_t)&int1params), CHANGE);
//... some logic
detachInterrupt(digitalPinToInterrupt(1));
bindArgGateFree(int1gate);
// if you init once in "setup", you may not need bindArgGateFree
attachInterrupt(digitalPinToInterrupt(2), bindArgGateVoidAllocate((bindArgProcFunc_t)&myInterruptHandler, (bindArgProcArg_t)&int2params), CHANGE);
@endcode
*/
extern 
bindArgVoidFunc_t bindArgGateVoidAllocate(bindArgProcFunc_t func, bindArgProcArg_t arg);

/**
@brief Allocates `void gate(void)` bind gate for typed `void func(type *arg)` function + `type *arg`. 

The gate calls original func with argument supplied at gate allocation time (so argument "binded" with func at gate).
The function is equivalent of `gate = std::bind(func, arg)`, but returns "pure" `void gate(void)` function.
Takes `void func(type *arg)` func and `type *arg` parameter to be passed to the func when bind gate is called.

Function is useful when attaching interrupt handlers, that need object/struct-like arguments

@param func function `void func(type *arg)` to be to bind 
@param arg `type *` parameter to be supplied to func when gate will be called
@return function that has `void gate(void)` prototype to be used as callback (or interrupt handler)
@note The number of available gates are limited (~30), so if you ever do not need the gate you should free it via `bindArgGateFree`

@code
//example:
struct MyData { }; // some data here
static MyData int1params;
static MyData int2params;
void myInterruptHandler(MyData *myParam) { } // some code here ( = nullptr just to trick arduino compiler)
// with free
static bindArgVoidFunc_t int1gate;
attachInterrupt(1, int1gate = bindArgGateFuncAllocate(&myInterruptHandler, &int1params), CHANGE);
//... some logic
detachInterrupt(1);
bindArgGateFree(int1gate);
// if you init once in "setup", you may not need bindArgGateFree
attachInterrupt(2, bindArgGateFuncAllocate(&myInterruptHandler, &int2params), CHANGE);
@endcode
*/
template<typename T>
bindArgVoidFunc_t bindArgGateFuncAllocate(void (*func)(T*), T* arg)
{
  return bindArgGateFuncAllocate(*((bindArgProcFunc_t*)((void*)(&func))), (bindArgProcArg_t)(arg)); // bit ugly way of passing to prevent useless warning
};

/**
@brief Allocates `void gate(void)` bind gate for object method call.

The gate calls original func with object pointer argument supplied at gate allocation time (so argument "binded" with func at gate).
The function is equivalent of `gate = std::bind(&MyClass::method, this)` or `gate = std::bind(&MyClass::method, &myObject)`, 
but returns "pure" `void gate(void)` function. Takes an object as argument to be passed to the method (usually `this`).

Function is useful when attaching interrupt handlers to object methods.

@param method method function pointer of object that takes no arguments and returns void
@param objPtr object pointer to be supplied to method at this pointer
@return function that has `void gate(void)` prototype to be used as callback (or interrupt handler)
@note The number of available gates are limited (~30), so if you ever do not need the gate you should free it via `bindArgGateFree`

@code
//Example (How use object instance method as interrupt handler):
class MyDevice
{
  bindArgVoidFunc_t interruptGate = nullptr;
  int               interruptNum;

  public:
  void interruptHandler()
  {
    // you may use "this" here
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
@endcode
*/
template<typename T>
bindArgVoidFunc_t bindArgGateThisAllocate(void (T::*method)(), T* objPtr)
{ 
  return bindArgGateFuncAllocate(*((bindArgProcFunc_t*)((void*)(&method))), (bindArgProcArg_t)(objPtr)); // bit ugly way of passing to prevent useless warning
};

/**
@brief Frees `void gate(void)` bind gate allocated by one of allocation `bindArgGate....Allocate` functions.

@note The number of available gates are limited (~30), so if you ever do not need the gate you should free it via bindArgGateFree.

@param gate object allocates by one of `bindArgGate....Allocate` functions
*/
extern 
void bindArgGateFree(bindArgVoidFunc_t gate);

#endif

