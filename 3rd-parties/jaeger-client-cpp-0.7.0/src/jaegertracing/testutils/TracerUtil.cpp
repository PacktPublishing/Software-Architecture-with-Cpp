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

#include "jaegertracing/testutils/TracerUtil.h"
#include "jaegertracing/Config.h"
#include "jaegertracing/Logging.h"
#include "jaegertracing/Tracer.h"
#include "jaegertracing/baggage/RestrictionsConfig.h"
#include "jaegertracing/net/IPAddress.h"
#include "jaegertracing/propagation/HeadersConfig.h"
#include "jaegertracing/reporters/Config.h"
#include "jaegertracing/samplers/Config.h"
#include <memory>
#include <ostream>
#include <string>

namespace jaegertracing {
namespace testutils {
namespace TracerUtil {

std::shared_ptr<ResourceHandle> installGlobalTracer(bool traceId128Bit, propagation::Format propagationFormat)
{
    std::unique_ptr<ResourceHandle> handle(new ResourceHandle());
    handle->_mockAgent->start();
    std::ostringstream samplingServerURLStream;
    samplingServerURLStream
        << "http://" << handle->_mockAgent->samplingServerAddress().authority();
    Config config(
        false,
        traceId128Bit,
        samplers::Config("const",
                         1,
                         samplingServerURLStream.str(),
                         0,
                         samplers::Config::Clock::duration()),
        reporters::Config(0,
                          reporters::Config::Clock::duration(),
                          false,
                          handle->_mockAgent->spanServerAddress().authority()),
        propagation::HeadersConfig(),
        baggage::RestrictionsConfig(),
        "",
        std::vector<Tag>(),
        propagationFormat);

    auto tracer = Tracer::make("test-service", config, logging::nullLogger());
    opentracing::Tracer::InitGlobal(tracer);
    return std::move(handle);
}

std::shared_ptr<ResourceHandle> installGlobalTracer()
{
    return installGlobalTracer(false, propagation::Format::JAEGER);
}

std::shared_ptr<ResourceHandle> installGlobalTracer128Bit()
{
    return installGlobalTracer(true, propagation::Format::JAEGER);
}

std::shared_ptr<ResourceHandle> installGlobalTracerW3CPropagation()
{
    return installGlobalTracer(false, propagation::Format::W3C);
}

std::shared_ptr<opentracing::Tracer> buildTracer(const std::string& endpoint)
{
    std::ostringstream samplingServerURLStream;
    Config config(
        false,
        false,
        samplers::Config("const",
                         1,
                         samplingServerURLStream.str(),
                         0,
                         samplers::Config::Clock::duration()),
        reporters::Config(0,
                          std::chrono::milliseconds(100),
                          false, "", endpoint),
        propagation::HeadersConfig(),
        baggage::RestrictionsConfig());

    auto tracer = Tracer::make("test-service", config, logging::nullLogger());
    return tracer;
}

}  // namespace TracerUtil
}  // namespace testutils
}  // namespace jaegertracing
