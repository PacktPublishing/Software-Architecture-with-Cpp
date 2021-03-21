#pragma once

struct Resource;

// C API
Resource *acquireResource() {}
void releaseResource(Resource *resource) {}

// C++ API
using ResourceRaii = std::unique_ptr<Resource, decltype(&releaseResource)>;
ResourceRaii acquireResourceRaii() {}
