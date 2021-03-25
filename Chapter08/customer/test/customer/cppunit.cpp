#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include "customer/customer.h"

using namespace CppUnit;
using namespace std;

class TestBasicResponses : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestBasicResponses);
  CPPUNIT_TEST(testBob);
  CPPUNIT_TEST_SUITE_END();

 protected:
  void testBob();
};

void TestBasicResponses::testBob() {
  auto name = "Bob";
  auto code_and_string = responder{}.prepare_response(name);
  CPPUNIT_ASSERT(code_and_string.first == web::http::status_codes::OK);
  CPPUNIT_ASSERT(code_and_string.second == web::json::value("Hello, Bob!"));
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestBasicResponses);

int main() {
  CPPUNIT_NS::TestResult testresult;

  CPPUNIT_NS::TestResultCollector collectedresults;
  testresult.addListener(&collectedresults);

  CPPUNIT_NS::BriefTestProgressListener progress;
  testresult.addListener(&progress);

  CPPUNIT_NS::TestRunner testrunner;
  testrunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
  testrunner.run(testresult);

  CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
  compileroutputter.write();

  ofstream xmlFileOut("cppTestBasicResponsesResults.xml");
  XmlOutputter xmlOut(&collectedresults, xmlFileOut);
  xmlOut.write();

  return collectedresults.wasSuccessful() ? 0 : 1;
}
