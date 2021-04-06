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

#include <random>

#include <gtest/gtest.h>

#include "jaegertracing/Constants.h"
#include "jaegertracing/Tag.h"
#include "jaegertracing/samplers/AdaptiveSampler.h"
#include "jaegertracing/samplers/Config.h"
#include "jaegertracing/samplers/ConstSampler.h"
#include "jaegertracing/samplers/GuaranteedThroughputProbabilisticSampler.h"
#include "jaegertracing/samplers/ProbabilisticSampler.h"
#include "jaegertracing/samplers/RateLimitingSampler.h"
#include "jaegertracing/samplers/RemotelyControlledSampler.h"
#include "jaegertracing/samplers/Sampler.h"
#include "jaegertracing/samplers/SamplingStatus.h"
#include "jaegertracing/testutils/MockAgent.h"
#include "jaegertracing/testutils/TUDPTransport.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"

namespace jaegertracing {
namespace samplers {
namespace {

constexpr auto kTestOperationName = "op";
constexpr auto kTestFirstTimeOperationName = "firstTimeOp";
constexpr auto kTestDefaultSamplingProbability = 0.5;
constexpr auto kTestMaxID = std::numeric_limits<uint64_t>::max() / 2 + 1;
constexpr auto kTestDefaultMaxOperations = 10;

const Tag testProbablisticExpectedTags[] = {
    { "sampler.type", "probabilistic" }, { "sampler.param", 0.5 }
};

const Tag testLowerBoundExpectedTags[] = { { "sampler.type", "lowerbound" },
                                           { "sampler.param", 0.5 } };

#define CMP_TAGS(tagArr, tagVec)                                               \
    {                                                                          \
        ASSERT_EQ(sizeof(tagArr) / sizeof(Tag), (tagVec).size());              \
        for (auto i = static_cast<size_t>(0); i < (tagVec).size(); ++i) {      \
            jaegertracing::thrift::Tag thriftTagArr;                           \
            jaegertracing::thrift::Tag thriftTagVec;                           \
            (tagArr)[i].thrift(thriftTagArr);                                  \
            (tagVec)[i].thrift(thriftTagVec);                                  \
            ASSERT_EQ(thriftTagArr, thriftTagVec);                             \
        }                                                                      \
    }

}  // anonymous namespace

TEST(Sampler, testSamplerTags)
{
    ConstSampler constTrue(true);
    ConstSampler constFalse(false);
    ProbabilisticSampler prob(0.1);
    RateLimitingSampler rate(0.1);

    const struct {
        Sampler& _sampler;
        std::string _samplerType;
        Tag::ValueType _samplerParam;
    } tests[] = { { constTrue, "const", true },
                  { constFalse, "const", false },
                  { prob, "probabilistic", 0.1 },
                  { rate, "ratelimiting", 0.1 } };

    for (auto&& test : tests) {
        const auto tags =
            test._sampler.isSampled(TraceID(), kTestOperationName).tags();
        auto count = 0;
        for (auto&& tag : tags) {
            if (tag.key() == kSamplerTypeTagKey) {
                ASSERT_TRUE(tag.value().is<const char*>());
                ASSERT_EQ(test._samplerType, tag.value().get<const char*>());
                ++count;
            }
            else if (tag.key() == kSamplerParamTagKey) {
                ASSERT_EQ(test._samplerParam, tag.value());
                ++count;
            }
        }
        ASSERT_EQ(2, count);
    }
}

TEST(Sampler, testProbabilisticSamplerErrors)
{
    ProbabilisticSampler sampler(-0.1);
    ASSERT_LE(0, sampler.samplingRate());
    ASSERT_GE(1, sampler.samplingRate());
    sampler = ProbabilisticSampler(1.1);
    ASSERT_LE(0, sampler.samplingRate());
    ASSERT_GE(1, sampler.samplingRate());
}

TEST(Sampler, testProbabilisticSampler)
{
    {
        ProbabilisticSampler sampler(0.5);
        auto result =
            sampler.isSampled(TraceID(0, kTestMaxID + 10), kTestOperationName);
        ASSERT_FALSE(result.isSampled());
        CMP_TAGS(testProbablisticExpectedTags, result.tags());

        result =
            sampler.isSampled(TraceID(0, kTestMaxID - 20), kTestOperationName);
        ASSERT_TRUE(result.isSampled());
        CMP_TAGS(testProbablisticExpectedTags, result.tags());
    }
    {
        ProbabilisticSampler sampler(1.0);
        auto result =
            sampler.isSampled(TraceID(0, kTestMaxID), kTestOperationName);
        ASSERT_TRUE(result.isSampled());

        result =
            sampler.isSampled(TraceID(0, kTestMaxID - 20), kTestOperationName);
        ASSERT_TRUE(result.isSampled());
    }
}

TEST(Sampler, testProbabilisticSamplerPerformance)
{
    constexpr auto kNumSamples = static_cast<uint64_t>(10000);

    ProbabilisticSampler sampler(0.001);
    std::random_device randomDevice;
    std::default_random_engine randomGenerator(randomDevice());
    std::uniform_int_distribution<uint64_t> distribution;
    auto count = static_cast<uint64_t>(0);
    for (auto i = static_cast<uint64_t>(0); i < kNumSamples; ++i) {
        TraceID id(0, distribution(randomGenerator));
        if (sampler.isSampled(id, kTestOperationName).isSampled()) {
            ++count;
        }
    }
    const auto rate = static_cast<double>(count) / kNumSamples;
    std::cout << "Sampled: " << count << " rate=" << rate << '\n';
}

TEST(Sampler, testProbabilisticSamplerInvalidRate)
{
    Config samplerConfig1(kSamplerTypeProbabilistic,
                          1.1,
                          "",
                          0,
                          samplers::Config::Clock::duration());
    Config samplerConfig2(kSamplerTypeProbabilistic,
                          -0.1,
                          "",
                          0,
                          samplers::Config::Clock::duration());
    auto logger = logging::nullLogger();
    auto metrics = metrics::Metrics::makeNullMetrics();
    ASSERT_THROW(samplerConfig1.makeSampler("test-service", *logger, *metrics),
                 std::invalid_argument);
    ASSERT_THROW(samplerConfig2.makeSampler("test-service", *logger, *metrics),
                 std::invalid_argument);
}

TEST(Sampler, testRateLimitingSampler)
{
    {
        RateLimitingSampler sampler(2);
        auto result = sampler.isSampled(TraceID(), kTestOperationName);
        ASSERT_TRUE(result.isSampled());
        result = sampler.isSampled(TraceID(), kTestOperationName);
        ASSERT_TRUE(result.isSampled());
        result = sampler.isSampled(TraceID(), kTestOperationName);
        ASSERT_FALSE(result.isSampled());
    }

    {
        RateLimitingSampler sampler(0.1);
        auto result = sampler.isSampled(TraceID(), kTestOperationName);
        ASSERT_TRUE(result.isSampled());
        result = sampler.isSampled(TraceID(), kTestOperationName);
        ASSERT_FALSE(result.isSampled());
    }
}

TEST(Sampler, testGuaranteedThroughputProbabilisticSamplerUpdate)
{
    auto lowerBound = 2.0;
    auto samplingRate = 0.5;
    GuaranteedThroughputProbabilisticSampler sampler(lowerBound, samplingRate);
    ASSERT_EQ(lowerBound, sampler.lowerBound());
    ASSERT_EQ(samplingRate, sampler.samplingRate());

    auto newLowerBound = 1.0;
    auto newSamplingRate = 0.6;
    sampler.update(newLowerBound, newSamplingRate);
    ASSERT_EQ(newLowerBound, sampler.lowerBound());
    ASSERT_EQ(newSamplingRate, sampler.samplingRate());

    newSamplingRate = 1.1;
    sampler.update(newLowerBound, newSamplingRate);
    ASSERT_EQ(1.0, sampler.samplingRate());
}

TEST(Sampler, testAdaptiveSampler)
{
    namespace thriftgen = sampling_manager::thrift;

    thriftgen::OperationSamplingStrategy strategy;
    strategy.__set_operation(kTestOperationName);
    thriftgen::ProbabilisticSamplingStrategy probabilisticSampling;
    probabilisticSampling.__set_samplingRate(kTestDefaultSamplingProbability);
    strategy.__set_probabilisticSampling(probabilisticSampling);

    thriftgen::PerOperationSamplingStrategies strategies;
    strategies.__set_defaultSamplingProbability(
        kTestDefaultSamplingProbability);
    strategies.__set_defaultLowerBoundTracesPerSecond(1.0);
    strategies.__set_perOperationStrategies({ strategy });

    AdaptiveSampler sampler(strategies, kTestDefaultMaxOperations);
    auto result =
        sampler.isSampled(TraceID(0, kTestMaxID + 10), kTestOperationName);
    ASSERT_TRUE(result.isSampled());
    CMP_TAGS(testLowerBoundExpectedTags, result.tags());

    result = sampler.isSampled(TraceID(0, kTestMaxID - 20), kTestOperationName);
    ASSERT_TRUE(result.isSampled());
    CMP_TAGS(testProbablisticExpectedTags, result.tags());

    result = sampler.isSampled(TraceID(0, kTestMaxID + 10), kTestOperationName);
    ASSERT_FALSE(result.isSampled());

    result = sampler.isSampled(TraceID(0, kTestMaxID - 20),
                               kTestFirstTimeOperationName);
    ASSERT_TRUE(result.isSampled());
    CMP_TAGS(testProbablisticExpectedTags, result.tags());
}

TEST(Sampler, testAdaptiveSamplerErrors)
{
    namespace thriftgen = sampling_manager::thrift;

    thriftgen::OperationSamplingStrategy strategy;
    strategy.__set_operation(kTestOperationName);
    thriftgen::ProbabilisticSamplingStrategy probabilisticSampling;
    probabilisticSampling.__set_samplingRate(-0.1);
    strategy.__set_probabilisticSampling(probabilisticSampling);

    thriftgen::PerOperationSamplingStrategies strategies;
    strategies.__set_defaultSamplingProbability(
        kTestDefaultSamplingProbability);
    strategies.__set_defaultLowerBoundTracesPerSecond(2.0);
    strategies.__set_perOperationStrategies({ strategy });

    {
        AdaptiveSampler sampler(strategies, kTestDefaultMaxOperations);
    }

    {
        strategies.perOperationStrategies.at(0)
            .probabilisticSampling.__set_samplingRate(1.1);
        AdaptiveSampler sampler(strategies, kTestDefaultMaxOperations);
    }
}

TEST(Sampler, testAdaptiveSamplerUpdate)
{
    namespace thriftgen = sampling_manager::thrift;

    constexpr auto kSamplingRate = 0.1;
    constexpr auto kLowerBound = 2.0;

    thriftgen::OperationSamplingStrategy strategy;
    strategy.__set_operation(kTestOperationName);
    thriftgen::ProbabilisticSamplingStrategy probabilisticSampling;
    probabilisticSampling.__set_samplingRate(kSamplingRate);
    strategy.__set_probabilisticSampling(probabilisticSampling);

    thriftgen::PerOperationSamplingStrategies strategies;
    strategies.__set_defaultSamplingProbability(
        kTestDefaultSamplingProbability);
    strategies.__set_defaultLowerBoundTracesPerSecond(kLowerBound);
    strategies.__set_perOperationStrategies({ strategy });

    AdaptiveSampler sampler(strategies, kTestDefaultMaxOperations);

    constexpr auto kNewSamplingRate = 0.2;
    constexpr auto kNewLowerBound = 3.0;
    constexpr auto kNewDefaultSamplingProbability = 0.1;

    // Updated kTestOperationName strategy.
    thriftgen::OperationSamplingStrategy updatedStrategy;
    updatedStrategy.__set_operation(kTestOperationName);
    thriftgen::ProbabilisticSamplingStrategy updatedProbabilisticSampling;
    updatedProbabilisticSampling.__set_samplingRate(kNewSamplingRate);
    updatedStrategy.__set_probabilisticSampling(updatedProbabilisticSampling);

    // New kTestFirstTimeOperationName strategy.
    thriftgen::OperationSamplingStrategy newStrategy;
    newStrategy.__set_operation(kTestFirstTimeOperationName);
    thriftgen::ProbabilisticSamplingStrategy newProbabilisticSampling;
    newProbabilisticSampling.__set_samplingRate(kNewSamplingRate);
    newStrategy.__set_probabilisticSampling(newProbabilisticSampling);

    thriftgen::PerOperationSamplingStrategies newStrategies;
    newStrategies.__set_defaultSamplingProbability(
        kNewDefaultSamplingProbability);
    newStrategies.__set_defaultLowerBoundTracesPerSecond(kNewLowerBound);
    newStrategies.__set_perOperationStrategies(
        { updatedStrategy, newStrategy });

    sampler.update(newStrategies);
}

TEST(Sampler, testRemotelyControlledSampler)
{
    const auto mockAgent = testutils::MockAgent::make();
    mockAgent->start();
    const auto logger = logging::nullLogger();
    const auto metrics = metrics::Metrics::makeNullMetrics();
            
    // Make sure remote sampling probability is 1
    sampling_manager::thrift::SamplingStrategyResponse config;
    config.__set_strategyType(
        sampling_manager::thrift::SamplingStrategyType::PROBABILISTIC);
    sampling_manager::thrift::ProbabilisticSamplingStrategy probaStrategy;
    probaStrategy.__set_samplingRate(1.0);
    config.__set_probabilisticSampling(probaStrategy);
    mockAgent->addSamplingStrategy("test-service", config);

    // Default probability of 0.5, switches to 1 when downloaded
    RemotelyControlledSampler sampler(
        "test-service",
        "http://" + mockAgent->samplingServerAddress().authority(),
        std::make_shared<ProbabilisticSampler>(kTestDefaultSamplingProbability),
        kTestDefaultMaxOperations,
        std::chrono::milliseconds(100),
        *logger,
        *metrics);
    
    // Wait a bit for remote config download to be done
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::random_device device;
    std::mt19937_64 rng;
    rng.seed(device());
    for (auto startTime = RemotelyControlledSampler::Clock::now();
         std::chrono::duration_cast<std::chrono::seconds>(
             RemotelyControlledSampler::Clock::now() - startTime)
             .count() < 1;) {
        TraceID traceID(rng(), rng());
        // If probability was 0.5 we could reasonnably assume one of 50 samples fail
        ASSERT_TRUE(sampler.isSampled(traceID, kTestOperationName).isSampled());
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    sampler.close();
}

}  // namespace samplers
}  // namespace jaegertracing
