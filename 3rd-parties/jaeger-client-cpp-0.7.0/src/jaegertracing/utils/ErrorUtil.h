/*
 * Copyright (c) 2017 Uber Technologies, Inc.
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

#ifndef JAEGERTRACING_UTILS_ERRORUTIL_H
#define JAEGERTRACING_UTILS_ERRORUTIL_H

#include "jaegertracing/Logging.h"
#include "jaegertracing/Sender.h"
#include <sstream>
#include <string>
#include <system_error>

namespace jaegertracing {
namespace utils {
namespace ErrorUtil {

inline void logError(logging::Logger& logger, const std::string& message)
{
    try {
        throw;
    } catch (const Sender::Exception& ex) {
        std::ostringstream oss;
        oss << message << ": " << ex.what() << ", numFailed=" << ex.numFailed();
        logger.error(oss.str());
    } catch (const std::system_error& ex) {
        std::ostringstream oss;
        oss << message << ": " << ex.what() << ", code=" << ex.code().value();
        logger.error(oss.str());
    } catch (const std::exception& ex) {
        std::ostringstream oss;
        oss << message << ": " << ex.what();
        logger.error(oss.str());
    } catch (...) {
        logger.error(message);
    }
}

}  // namespace ErrorUtil
}  // namespace utils
}  // namespace jaegertracing

#endif  // JAEGERTRACING_UTILS_ERRORUTIL_H
