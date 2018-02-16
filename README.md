# ResMgr
Safe Defer for C++

# Reasons
Dealing with complex C libraries using handles and pointers in C++ has two simple alternatives:
- Use RAII and write (hopefully automate) huge amount of boilerplate to encapsulate concepts into classes.
- Use complex C-style low-level memory management that usually ends having ugly `goto` to circumvent resource lacks on errors.

GoLang introduces an interesting concept to address those problematics: **`defer`**.

This project provides an easy definition of **`defer`** in C++11 style using lambda capture and static object declaration to insure correct resource handling even when dealing with exceptions.

# Usage

> A line of code is worth a milion descriptions

With this in mind let's see this library in action.

```C++
RegMgr rmgr; // Declare staticly the resource manager

EXT_HANDLE h = CreateExternalResource(); // External C library resource
if(h == nullptr) {
  cerr << "Cannot create external resource\n";
  return -1; // Exit from the current function
}
rmgr.Defer([&h] {
  DestroyExternalResource(h);
  h = nullptr;
}); // Delegate resource destruction to rmgr

A* a = new A(); // Dynamic object allocation
rmgr.Defer([&a] {
  delete a;
  a = nullptr;
}); // Delegate destroy of allocated object

// At the end of rmgr scope all the defered operations are executed in the inverse order
```

Now when you are checking for errors and returning you shouldn't worry about some resources being aready allocated,\
**just `return`**!

ResMgr destructor will handle all the work for you, even when an exception is trown!

# Class constructors

What if I need to create a class and allocate resources that can fail during construction?

Here is a solution for you:
```C++
class A {
  private:
    ResMgr rmgr;
    EXT_HANDLE h = nullptr;
  public:
    A(bool do_throw) {
      ResMgr _rmgr; // Temporary resource manager to handle construction exceptions
      
      this->h = CreateExternalResource();
      if(this->h == nullptr) {
        cerr << "Cannot create external resource\n";
        throw A_Exception("External resource acquisition failed"); // Abort constructor
      }
      _rmgr.Defer([this] {
        DestroyExternalResource(this->h);
        this->h = nullptr;
      }); // Delegate resource destruction to rmgr

      if(do_throw) {
        throw A_Exception("Throw requested"); // Abort constructor
      }
      
      this->rmgr.Swap(_rmgr); // Handle resorces to internal manager leaving _rmgr empty
      // So at the end of the constructor _rmgr destruction has no side effects
    }
};
```

Now if an exceptions occours in the constructor and that expetion is catched no memory leaks occour!

# Extra functions

C++ users want controll, for this purpose two extra funciton are available:
- `rmgr.Clear();`\
executes all deferred actions added leaving `rmgr` empty.
- `rmgr.Release();`\
release all the deferred actions without executing them (*DANGEROUS* but might be useful in the right hands).