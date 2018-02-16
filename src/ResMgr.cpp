/*

  ResMgr (c) by Dario Fagotto
  under CC BY 4.0 license (see LICENSE.md)
  Version 1.0:
    Basic version with Swap capabilities

*/

#include "ResMgr.hpp"

void ResMgr::Defer(deferred_f d) {
  this->fs.push_back(d);
}

ResMgr::ResMgr() {}

void ResMgr::Swap(ResMgr& rmgr) {
  this->fs.swap(rmgr.getInternalStack());
}

void ResMgr::Clear() {
  while(!this->fs.empty()) {
    (this->fs.back())();
    this->fs.pop_back();
  }
}

void ResMgr::Release() {
  this->fs.clear();
}

ResMgr::~ResMgr() {
  this->Clear();
}

std::deque<deferred_f>& ResMgr::getInternalStack() {
  return this->fs;
}
