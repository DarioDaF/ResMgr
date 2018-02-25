/*

  ResMgr (c) by Dario Fagotto
  under CC BY 4.0 license (see LICENSE.md)
  Version 1.1:
    Swap replaced by move assignment/construction
  Version 1.0:
    Basic version with Swap capabilities

*/

#include "ResMgr.hpp"

void ResMgr::Defer(deferred_f d) {
  this->fs.push_back(d);
}

ResMgr::ResMgr() {}

ResMgr::ResMgr(ResMgr&& other) : fs(std::move(other.fs)) {}

ResMgr& ResMgr::operator=(ResMgr&& other) {
  this->fs = std::move(other.fs);
  return *this;
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
