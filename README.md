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

If you want just the function summary for this simple class look at the end!

With this in mind let's see this library in action.

```C++
ResMgr rmgr; // Declare staticly the resource manager

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
      
      this->rmgr = std::move(_rmgr); // Handle resorces to internal manager leaving _rmgr empty
      // So at the end of the constructor _rmgr destruction has no side effects
    }
};
```

Now if an exceptions occours in the constructor and that expetion is catched no memory leaks occour!

# Cascading managers

What if I need to revert some changes but keep the main manager alive?

```C++
ResMgr glob_mgr;

/* Stuff with glob_mgr */

ResMgr* sub_mgr = new ResMgr(); // Dynamicly allocated because it can be deleted before glob_mgr!
glob_mgr.Defer([&sub_mgr] {
  if(sub_mgr != nullptr) {
    sub_mgr->Clear(); // Empty the manager
    delete sub_mgr;
    sub_mgr = nullptr;
  }
});

/* Do stuff with sub_mgr */

sub_mgr->Clear(); // Revert sub changes

/* Here sub_mgr exists so you can use it to try again the change, but remember that the order of destruction cannot be changed anymore! */
```

# Safe class constructor is good, but I need my deletion logic...

No problem, C++ users want controll, so an extra function for this is being providen:

```C++
class A {
  private:
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
      
      _rmgr.Release(); // Release manager control, now all the resources are again fully under your control
      // So at the end of the constructor _rmgr destruction has no side effects
    }
    virtual ~A() {
      /* Do your destruction logic without fearing double frees! */
      DestroyExternalResource(this->h);
    }
};
```

# Function summary

- `rmgr.Defer(std::function<void (void)>);`\
Adds a functional (function pointer with context informations for captures) to be defered at destruction.
- `rmgr.Clear();`\
executes all deferred actions added leaving `rmgr` empty.
- `rmgr.Release();`\
release all the deferred actions without executing them (*DANGEROUS* but might be useful in the right hands).

The object is not copiable but can be `std::move`d!
