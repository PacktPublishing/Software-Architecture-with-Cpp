#pragma once

#ifdef NDEBUG
inline namespace release {
#else
inline namespace debug {
#endif

struct EasilyDebuggable {
// ...
#ifndef NDEBUG
// fields helping with debugging
#endif
};

}  // end namespace
