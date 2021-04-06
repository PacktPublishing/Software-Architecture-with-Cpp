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

#ifndef JAEGERTRACING_BAGGAGE_BAGGAGESETTER_H
#define JAEGERTRACING_BAGGAGE_BAGGAGESETTER_H

#include "jaegertracing/LogRecord.h"
#include "jaegertracing/Span.h"
#include "jaegertracing/baggage/RestrictionManager.h"
#include "jaegertracing/metrics/Metrics.h"

namespace jaegertracing {
namespace baggage {

class BaggageSetter {
  public:
    BaggageSetter(RestrictionManager& restrictionManager,
                  metrics::Metrics& metrics)
        : _restrictionManager(restrictionManager)
        , _metrics(metrics)
    {
    }

    template <typename LoggingFunction>
    void setBaggage(Span& span,
                    SpanContext::StrMap& baggage,
                    const std::string& key,
                    std::string value,
                    LoggingFunction logFn) const
    {
        auto truncated = false;
        const auto restriction =
            _restrictionManager.getRestriction(span.serviceNameNoLock(), key);
        if (!restriction.keyAllowed()) {
            logFields(span,
                      key,
                      value,
                      std::string(),
                      truncated,
                      restriction.keyAllowed(),
                      logFn);
            _metrics.baggageUpdateFailure().inc(1);
            return;
        }

        if (static_cast<int>(value.size()) > restriction.maxValueLength()) {
            truncated = true;
            value = value.substr(0, restriction.maxValueLength());
            _metrics.baggageTruncate().inc(1);
        }

        auto itr = baggage.find(key);
        const auto prevItem =
            (itr == std::end(baggage) ? std::string() : itr->second);
        if (itr == std::end(baggage)) {
            baggage[key] = value;
        }
        else {
            itr->second = value;
        }
        logFields(span,
                  key,
                  value,
                  prevItem,
                  truncated,
                  restriction.keyAllowed(),
                  logFn);
        _metrics.baggageUpdateSuccess().inc(1);
    }

  private:
    template <typename LoggingFunction>
    void logFields(const Span& span,
                   const std::string& key,
                   const std::string& value,
                   const std::string& prevItem,
                   bool truncated,
                   bool valid,
                   LoggingFunction logFn) const
    {
        if (!span.contextNoLock().isSampled()) {
            return;
        }

        std::vector<Tag> fields(
            { { "event", "baggage" }, { "key", key }, { "value", value } });
        if (!prevItem.empty()) {
            fields.push_back(Tag("override", "true"));
        }
        if (truncated) {
            fields.push_back(Tag("truncated", "true"));
        }
        if (!valid) {
            fields.push_back(Tag("invalid", "true"));
        }

        logFn(std::begin(fields), std::end(fields));
    }

    RestrictionManager& _restrictionManager;
    metrics::Metrics& _metrics;
};

}  // namespace baggage
}  // namespace jaegertracing

#endif  // JAEGERTRACING_BAGGAGE_BAGGAGESETTER_H
