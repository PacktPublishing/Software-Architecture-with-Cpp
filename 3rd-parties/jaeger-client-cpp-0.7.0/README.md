[![Build Status][ci-img]][ci] [![Coverage Status][cov-img]][cov] [![Appveyor Build][appveyor]][appveyor] [![OpenTracing 1.0 Enabled][ot-img]][ot-url]

![Ubuntu](https://github.com/jaegertracing/jaeger-client-cpp/workflows/Ubuntu/badge.svg)
![MacOS](https://github.com/jaegertracing/jaeger-client-cpp/workflows/MacOS/badge.svg)
![Windows](https://github.com/jaegertracing/jaeger-client-cpp/workflows/Windows/badge.svg)


# jaeger-client-cpp
[Jaeger](https://www.jaegertracing.io/) SDK with [OpenTracing API for C++](https://github.com/opentracing/opentracing-cpp) binding.

## Contributing

Please see [CONTRIBUTING.md](CONTRIBUTING.md).

## Building

jaeger-client-cpp is built using CMake. It will automatically download
needed dependencies using [Hunter](https://docs.hunter.sh/en/latest/).

To build:

```bash
    mkdir build
    cd build
    cmake ..
    make
```

After building, the [example](./examples/App.cpp) program can be run
with:

```bash
    ./app ../examples/config.yml
```

To run tests:

```bash
    make test
```

To install the library:

```bash
    make install
```

### Generated files

This project uses Apache Thrift for wire-format protocol support code
generation. It currently requires Thrift 0.11.0.

The code can be re-generated with

```bash
    $ git submodule update --init
    $ find idl/thrift/ -type f -name \*.thrift -exec thrift -gen cpp -out src/jaegertracing/thrift-gen {} \;
    $ git apply scripts/thrift-gen.patch
```

### Updating Agent Host and Port

The default host and port for Jaeger Agent is `127.0.0.1:6831`. When the application and Jaeger Agent are running in different containers on the same host, the application's notion of `localhost`/127.0.0.1 it restriced to its own container, so in this case it's usually necessary to override the Agent's host/port by updating your reporter configuration:

YAML configuration:

```yml
reporter:
  localAgentHostPort: jaeger-agent:6831
```

NOTE: It is not recommended to use a remote host for UDP connections.

### Connecting directly to the Collector

In case the client should connect directly to the collector instead of going through an agent, it's necessary update the reporter configuration

```yml
reporter:
  endpoint: http://${collectorhost}:${collectorport}/api/traces
```

Note that if both `localAgentHostPort` and `endpoint` are specified, the `endpoint` will be used.

### Updating Sampling Server URL

The default sampling collector URL is `http://127.0.0.1:5778/sampling`. Similar to UDP address above, you can use a different URL by updating the sampler configuration.

```yml
sampler:
  samplingServerURL: http://jaeger-agent.local:5778/sampling
```

### Configuration via Environment

It's possible to populate the tracer configuration from the environement variables by calling `jaegertracing::Config::fromEnv`.

The following property names are currently available:

Property | Description
--- | ---
JAEGER_SERVICE_NAME | The service name
JAEGER_DISABLED _(not recommended)_ | Instructs the Configuration to return a no-op tracer
JAEGER_AGENT_HOST | The hostname for communicating with agent via UDP
JAEGER_AGENT_PORT | The port for communicating with agent via UDP
JAEGER_ENDPOINT | The traces endpoint, in case the client should connect directly to the Collector, like http://jaeger-collector:14268/api/traces
JAEGER_PROPAGATION | The propagation format used by the tracer. Supported values are jaeger and w3c
JAEGER_REPORTER_LOG_SPANS | Whether the reporter should also log the spans
JAEGER_REPORTER_MAX_QUEUE_SIZE | The reporter's maximum queue size
JAEGER_REPORTER_FLUSH_INTERVAL | The reporter's flush interval (ms)
JAEGER_SAMPLER_TYPE | The [sampler type](https://www.jaegertracing.io/docs/latest/sampling/#client-sampling-configuration)
JAEGER_SAMPLER_PARAM | The sampler parameter (double)
JAEGER_SAMPLING_ENDPOINT | The url for the remote sampling conf when using sampler type remote. Default is http://127.0.0.1:5778/sampling
JAEGER_TAGS | A comma separated list of `name = value` tracer level tags, which get added to all reported spans. The value can also refer to an environment variable using the format `${envVarName:default}`, where the `:default` is optional, and identifies a value to be used if the environment variable cannot be found

### SelfRef
Jaeger Tracer supports an additional reference type call 'SelfRef'.
It returns an opentracing::SpanReference which can be passed to Tracer::StartSpan
to influence the SpanContext of the newly created span. Specifically, the new span inherits the traceID
and spanID from the passed SELF reference. It can be used to pass externally generated IDs to the tracer,
with the purpose of recording spans from data generated elsewhere (e.g. from logs), or by augmenting the
data of the existing span (Jaeger backend will merge multiple instances of the spans with the same IDs).
Must be the lone reference, can be used only for root spans.

Usage example:
```
  jaegertracing::SpanContext customCtx { {1, 2}, 3, 0, 0, jaegertracing::SpanContext::StrMap() }; // TraceId and SpanID must be != 0
  auto span = tracer->StartSpan("spanName", { jaegertracing::SelfRef(&customCtx) });
```

## License

[Apache 2.0 License](./LICENSE).

[ci-img]: https://travis-ci.org/jaegertracing/jaeger-client-cpp.svg?branch=master
[ci]: https://travis-ci.org/jaegertracing/jaeger-client-cpp
[appveyor]: https://ci.appveyor.com/api/projects/status/bu992qd3y9bpwe7u?svg=true
[cov-img]: https://codecov.io/gh/jaegertracing/jaeger-client-cpp/branch/master/graph/badge.svg
[cov]: https://codecov.io/gh/jaegertracing/jaeger-client-cpp
[ot-img]: https://img.shields.io/badge/OpenTracing--1.0-enabled-blue.svg
[ot-url]: http://opentracing.io
