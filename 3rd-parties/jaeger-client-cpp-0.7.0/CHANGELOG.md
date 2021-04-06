Changes by Version
==================

0.7.1 (unreleased)
------------------

-- Nothing yet.


0.7.0 (2021-02-28)
------------------

- Add support for W3C traceparent/tracestate propagation (#255)
- Add support for generating 128 bit trace ids (#254)
- Do not strip leading zeros from trace IDs and span IDs (#259)


0.6.0 (2020-06-08)
------------------
- Support plugin configuration via environment variables (#192)
- An extension of enum opentracing::SpanReferenceType, for a new Span. (#206)
- Update OpenTracing to 1.6.0 (#209)
- Fix loading of sampling endpoint address from JAEGER_SAMPLING_ENDPOINT environment variable (#200)
- Fix sampling endpoint URL
- Fix typo in README.md (#194)
- Add jaeger-debug-id as a tag (#190)
- Reuse TMemoryBuffer when calculating size of ThriftType (#185)
- Support Tracer tags and  configuration via environment variables (#181)
- Add an HTTP Sender (#165) (#171)
- Remove Thrift headers from Jaeger public headers (#172)
- Update default samplingServerURL to include /sampling path (#158)


0.5.0 (2019-09-07)
------------------
- Improve C++ flag handling in CMake (#128)
- localIP: try resolving local hostname first (#130)
- Fix deadlock if steady_clock::now() returns the same value twice (#132)
- Implement SpanContext::Clone (#138)
- Change sampler param sentinel value from YAML parser (#145)
- Fix RemoteReporter test race condition (#135)
- Add details on how to set agent & collector endpoint (#153)
- Clarify agent/sampler address overrides in README
- Upgrade to OpenTracing 1.5.0; Support build on Windows (#115)
- Throw exception on invalid sampling rate (#168)


0.4.2 (2018-08-14)
------------------
- Fix `tracer.Inject(..., HTTPHeadersWriter&)` (#107)
- Upgrade dynamic loading API (#120)


0.4.1 (2018-05-16)
------------------
- Add example application (#101)
- Improve CMake installation to allow use of lib64 directory (#102)
- Fix CMake config for OpenTracing dependency (#103)
- Fix tracer inject for HTTP headers (#107)


0.4.0 (2018-05-07)
------------------
- Build shared plugin for Linux amd64 using Docker (#82)
- Fix UDP test compilation error (#88)
- Fix usage of propagation headers config (#91)
- Fix dynamic load build error (#92)
- Use Thrift 0.11.0 (#94)


0.3.0 (2018-04-17)
------------------
- Use LogRecord in Span::FinishWithOptions (#58)
- Flush pending spans in RemoteReporter destructor (#59)
- Add support for dynamic loading (#64)
- Fix unhandled exception when Jaeger agent unavailable (#80)
- Fix potential race condition in concurrent baggage access (#83)


0.2.0 (2018-01-25)
------------------
- Fix bug in localIP and revert change in TR1 tuple definition (#31)
- Add language prefix to Jaeger client version tag (#35)
- Fix yaml-cpp issues (#39)


0.1.0 (2017-11-29)
------------------
- Don't use forwarding reference (#11)
- Fix overflow bug (#13)
- Set default-constructed timestamp to current time in StartSpan (#18)


0.0.8 (2017-11-20)
------------------
- Fix host IP formatting and improve local IP API (#4)
- Use JSON instead of Thrift's TJSONProtocol (#12)
