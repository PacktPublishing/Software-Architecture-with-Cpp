/*
 * Copyright (c) 2019 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JAEGERTRACING_COMPILERS_H
#define JAEGERTRACING_COMPILERS_H

#ifdef _MSC_VER

#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

// Define NOMINMAX to inhibit definition of Macros min(a,b) and max(a,b) in
// windows.h
#ifndef NOMINMAX
#define NOMINMAX
#endif

#endif  // _MSC_VER

#endif  // JAEGERTRACING_COMPILERS_H
