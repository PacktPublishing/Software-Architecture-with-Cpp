#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Sum.h>

using cucumber::ScenarioScope;

struct SumCtx {
  Sum sum;
  int a;
  int b;
  int result;
};

GIVEN("^I have entered (\\d+) and (\\d+) as parameters$",
      (const int a, const int b)) {
  ScenarioScope<SumCtx> context;

  context->a = a;
  context->b = b;
}

WHEN("^I add them") {
  ScenarioScope<SumCtx> context;

  context->result = context->sum.sum(context->a, context->b);
}

THEN("^the result should be (.*)$", (const int expected)) {
  ScenarioScope<SumCtx> context;

  EXPECT_EQ(expected, context->result);
}
