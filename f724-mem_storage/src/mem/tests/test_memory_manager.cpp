/*!
 * \file src/mem/tests/test_agent_memory.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for the memory module
 */
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include <stdexcept>
#include "../memory_manager.hpp"

namespace m = flame::mem;


BOOST_AUTO_TEST_SUITE(MemModule)

BOOST_AUTO_TEST_CASE(test_register_agent) {
  size_t s1 = 100, s2 = 123;
  m::MemoryManager mgr;
  /*
  mgr.RegisterAgent("Circle", s1);
  mgr.RegisterAgent("Square", s2);

  // Registering a single agent variable
  mgr.RegisterAgentVar<int>("Circle", "val");

  // Registering multiple agent variavles
  std::vector<std::string> var_names;
  var_names.push_back("x");
  var_names.push_back("y");
  var_names.push_back("z");
  mgr.RegisterAgentVar<double>("Circle", var_names);

  // Register var to the other agent
  mgr.RegisterAgentVar<int>("Square", "val");

  // retrieve vector from non-existent agent
  BOOST_CHECK_THROW((mgr.GetReader<int>("Triangle", "val")),
                    std::invalid_argument);
  // retrieve unregistered vector from agent
  BOOST_CHECK_THROW((mgr.GetReader<int>("Circle", "dummy")),
                    std::invalid_argument);
  // retrieve valid vector with wrong type
  BOOST_CHECK_THROW((mgr.GetReader<int>("Circle", "x")), std::domain_error);
  // retrieve valid vector
  BOOST_CHECK_NO_THROW((mgr.GetReader<int>("Circle", "val")));
  BOOST_CHECK_NO_THROW((mgr.GetReader<int>("Square", "val")));
  BOOST_CHECK_NO_THROW((mgr.GetReader<double>("Circle", "y")));
}

BOOST_AUTO_TEST_CASE(test_vector_access_empty) {
  m::MemoryManager mgr;
  mgr.RegisterAgent("Circle", 10);
  mgr.RegisterAgentVar<int>("Circle", "val");

  m::VectorReader<int> ro = mgr.GetReader<int>("Circle", "val");
  BOOST_CHECK(ro.begin() == ro.end());
  BOOST_CHECK(ro.empty());
  BOOST_CHECK_EQUAL(ro.size(), (size_t)0);
  */
}
/*

struct F {  // test fixture
  F() {
    mgr.RegisterAgent("Circle", 10);
    mgr.RegisterAgentVar<int>("Circle", "val");

    // populate vector by accessing hook into private method (test build only)
    std::vector<int> &vec = mgr.GetMemoryVector_test<int>("Circle", "val");
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
  }

  m::MemoryManager mgr;
};

BOOST_FIXTURE_TEST_CASE(test_vector_access_readonly, F) {
  m::VectorReader<int> ro = mgr.GetReader<int>("Circle", "val");
  // check iteration using RO method
  BOOST_CHECK(ro.begin() != ro.end());
  BOOST_CHECK(!ro.empty());
  BOOST_CHECK_EQUAL(ro.size(), (size_t)3);

  int expected[] = {1, 2, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected+3, ro.begin(), ro.end());
  BOOST_CHECK(typeid(ro.begin()) == typeid(std::vector<int>::const_iterator));
}

BOOST_FIXTURE_TEST_CASE(test_vector_access_readwrite, F) {
  // check iteration using RW method
  int expected[] = {1, 2, 3};
  m::VectorWriter<int> rw = mgr.GetWriter<int>("Circle", "val");
  BOOST_CHECK_EQUAL_COLLECTIONS(expected, expected+3, rw.begin(), rw.end());
  BOOST_CHECK(typeid(rw.begin()) == typeid(std::vector<int>::iterator));

  // update values using RW method
  m::VectorWriter<int>::iterator it;
  for (it = rw.begin(); it < rw.end(); ++it) {
    *it *= 2;
  }
  // check updated values
  int expected2[] = {2, 4, 6};
  BOOST_CHECK_EQUAL_COLLECTIONS(expected2, expected2+3, rw.begin(), rw.end());

  // test internal vector using hook into private method (test build only)
  std::vector<int> &vec = mgr.GetMemoryVector_test<int>("Circle", "val");
  BOOST_CHECK_EQUAL_COLLECTIONS(expected2, expected2+3, vec.begin(), vec.end());
}
*/
BOOST_AUTO_TEST_SUITE_END()