#include <iostream>

#include <yaml-cpp/yaml.h>

#include <jaegertracing/Tracer.h>

namespace {

void setUpTracer(const char* configFilePath)
{
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
        "example-service", config, jaegertracing::logging::consoleLogger());
    opentracing::Tracer::InitGlobal(
        std::static_pointer_cast<opentracing::Tracer>(tracer));
}

void tracedSubroutine(const std::unique_ptr<opentracing::Span>& parentSpan)
{
    auto span = opentracing::Tracer::Global()->StartSpan(
        "tracedSubroutine", { opentracing::ChildOf(&parentSpan->context()) });
}

void tracedFunction()
{
    auto span = opentracing::Tracer::Global()->StartSpan("tracedFunction");
    tracedSubroutine(span);
}

}  // anonymous namespace

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <config-yaml-path>\n";
        return 1;
    }
    setUpTracer(argv[1]);
    tracedFunction();
    // Not stricly necessary to close tracer, but might flush any buffered
    // spans. See more details in opentracing::Tracer::Close() documentation.
    opentracing::Tracer::Global()->Close();
    return 0;
}
