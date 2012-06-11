/*!
 * \file src/exe/tests/test_task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Test suite for task manager
 */
#define BOOST_TEST_DYN_LINK
#include <vector>
#include <string>
#include "boost/test/unit_test.hpp"
#include "include/flame.h"
#include "../task_manager.hpp"

BOOST_AUTO_TEST_SUITE(TaskManager)

namespace exe = flame::exe;
namespace mem = flame::mem;

// dummy function
FLAME_AGENT_FUNC(func1) { return 0; }

BOOST_AUTO_TEST_CASE(taskmgr_initialise_and_test_singleton) {
  std::string task_name = "test_task";
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.RegisterAgent("Circle");

  exe::TaskManager& tm1 = exe::TaskManager::GetInstance();
  tm1.CreateTask(task_name, "Circle", &func1);

  exe::TaskManager& tm2 = exe::TaskManager::GetInstance();
  exe::Task& task = tm2.GetTask(task_name);
  BOOST_CHECK_EQUAL(task.get_task_name(), task_name);


  tm2.Reset();  // TEST BUILD only method
  mgr.Reset();
  BOOST_CHECK_THROW(tm2.GetTask(task_name),
                    flame::exceptions::invalid_argument);
}

BOOST_AUTO_TEST_CASE(initialise_memory_manager) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.RegisterAgent("Circle");
  mgr.RegisterAgentVar<int>("Circle", "x_int");
  mgr.RegisterAgentVar<double>("Circle", "y_dbl");
  mgr.HintPopulationSize("Circle", (size_t)10);
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)1);

  mgr.GetVector<int>("Circle", "x_int")->push_back(1);
  mgr.GetVector<double>("Circle", "y_dbl")->push_back(0.1);
}

BOOST_AUTO_TEST_CASE(test_create_task) {
  exe::TaskManager& tm = exe::TaskManager::GetInstance();
  BOOST_CHECK_THROW(tm.CreateTask("t1", "NotAnAgent", &func1),
                    flame::exceptions::invalid_agent);
  BOOST_CHECK_THROW(tm.CreateTask("t1", "Circle", NULL),
                    flame::exceptions::invalid_argument);

  exe::Task& task = tm.CreateTask("t1", "Circle", &func1);
  task.AllowAccess("x_int");
  task.AllowAccess("y_dbl", true);
  mem::MemoryIteratorPtr m_iter = task.GetMemoryIterator();
  BOOST_CHECK_THROW(task.AllowAccess("NotAVar"),
                    flame::exceptions::invalid_variable);

  BOOST_CHECK_THROW(tm.GetTask("NotATask"),
                    flame::exceptions::invalid_argument);
  exe::Task& t1 = tm.GetTask("t1");
  BOOST_CHECK_EQUAL(t1.get_task_name(), "t1");

  // get by id
  exe::Task& t2 = tm.GetTask(t1.get_task_id());
  BOOST_CHECK_EQUAL(t2.get_task_name(), t1.get_task_name());
  BOOST_CHECK_THROW(tm.GetTask(1000), flame::exceptions::invalid_argument);

  // reset
  tm.Reset();
}

BOOST_AUTO_TEST_CASE(test_dep_management) {
  exe::TaskManager& tm = exe::TaskManager::GetInstance();

  // check initial data structure
  BOOST_CHECK_EQUAL(tm.get_nodeps_size(), 0);
  BOOST_CHECK_EQUAL(tm.get_task_count(), 0);

  // add task
  tm.CreateTask("t1", "Circle", &func1);
  tm.CreateTask("t2", "Circle", &func1);
  tm.CreateTask("t3", "Circle", &func1);
  tm.CreateTask("t4", "Circle", &func1);
  BOOST_CHECK_EQUAL(tm.get_nodeps_size(), 4);
  BOOST_CHECK_EQUAL(tm.get_node_count(), 4);

  // reset
  tm.Reset();
}

BOOST_AUTO_TEST_CASE(reset_memory_manager) {
  mem::MemoryManager& mgr = mem::MemoryManager::GetInstance();
  mgr.Reset();  // reset again so as not to affect next test suite
  BOOST_CHECK_EQUAL(mgr.GetAgentCount(), (size_t)0);
}

BOOST_AUTO_TEST_SUITE_END()