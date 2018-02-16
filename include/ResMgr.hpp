#ifndef DAF_RESMGR_HPP
#define DAF_RESMGR_HPP

/*

  ResMgr (c) by Dario Fagotto
  under CC BY 4.0 license (see LICENSE.md)
  Version 1:
    Basic version with Swap capabilities

*/

#include <functional>
#include <deque>

typedef std::function<void (void)> deferred_f;

class ResMgr {
  public:
    void Defer(deferred_f d);
    void Clear();
    void Release();
    void Swap(ResMgr& rmgr);
    ResMgr();
    virtual ~ResMgr();
  protected:
    std::deque<deferred_f>& getInternalStack();
  private:
    std::deque<deferred_f> fs;
};

#endif
