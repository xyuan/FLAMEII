/*!
 * \file src/exe/task_manager.cpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <stdexcept>
#include "boost/foreach.hpp"
#include "task_manager.hpp"
#include "agent_task.hpp"
#include "exceptions/all.hpp"

static inline void check_not_finalised(bool finalised) {
  if (finalised) {
    throw flame::exceptions::logic_error(
                 "Finalise() called. Operation not allowed");
  }
}

static inline void check_finalised(bool finalised) {
  if (!finalised) {
    throw flame::exceptions::logic_error("Finalise() has not been called");
  }
}

namespace flame { namespace exe {

Task& TaskManager::CreateAgentTask(std::string task_name,
                                   std::string agent_name,
                                   TaskFunction func_ptr) {
  AgentTask* task_ptr = new AgentTask(task_name, agent_name, func_ptr);

  try {  // register new task with manager
    RegisterTask(task_name, task_ptr);
  } catch(const flame::exceptions::logic_error& E) {
    delete task_ptr;  // free memory if registration failed.
    throw E;  // rethrow exception
  }

  return *task_ptr;
}

//! \brief Registers and returns a new MessageBoard Task
Task& TaskManager::CreateMessageBoardTask(std::string task_name,
                                         std::string msg_name,
                                         MessageBoardTask::Operation op) {
  MessageBoardTask* task_ptr = new MessageBoardTask(task_name, msg_name, op);
  try {  // register new task with manager
    RegisterTask(task_name, task_ptr);
  } catch(const flame::exceptions::logic_error& E) {
    delete task_ptr;  // free memory if registration failed.
    throw E;  // rethrow exception
  }

  return *task_ptr;
}

// Separate out this bit so we can handle internal Tasks differently
void TaskManager::RegisterTask(std::string task_name, Task* task_ptr) {
  if (finalised_) {
    throw flame::exceptions::logic_error("Finalise() called. No more updates");
  }
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  // Check for tasks with same name
  TaskNameMap::iterator lb = name_map_.lower_bound(task_name);
  if (lb != name_map_.end() && !(name_map_.key_comp()(task_name, lb->first))) {
    throw flame::exceptions::logic_error("task with that name already exists");
  }
  // map task name to idx of new vector entry
  Task::id_type id = tasks_.size();  // use next index as id

#ifdef DEBUG
  if (Task::IsTermTask(id)) {
    throw flame::exceptions::out_of_range("Too many tasks");
  }
#endif

  name_map_.insert(lb, TaskNameMap::value_type(task_name, id));
  task_ptr->set_task_id(id);
  tasks_.push_back(task_ptr);

  // initialise entries for dependency management
  parents_.push_back(IdSet());
  children_.push_back(IdSet());
  roots_.insert(id);
  leaves_.insert(id);
}


TaskManager::TaskId TaskManager::GetId(std::string task_name) const {
  try {
    return name_map_.at(task_name);
  }
  catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Unknown task");
  }
}


Task& TaskManager::GetTask(std::string task_name) {
  return GetTask(GetId(task_name));
}


Task& TaskManager::GetTask(TaskManager::TaskId task_id) {
  try {
    return tasks_.at(task_id);
  }
  catch(const std::exception& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}


void TaskManager::AddDependency(std::string task_name,
                                std::string dependency_name) {
  AddDependency(GetId(task_name), GetId(dependency_name));
}


void TaskManager::AddDependency(TaskManager::TaskId task_id,
                                TaskManager::TaskId dependency_id) {
  if (!IsValidID(task_id) || !IsValidID(dependency_id)) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
  if (task_id == dependency_id) {
    throw flame::exceptions::logic_error("Task cannot depend on itself");
  }
  check_not_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

#ifdef DEBUG
  if (WillCauseCyclicDependency(task_id, dependency_id)) {
    throw flame::exceptions::logic_error("This will cause cyclic dependencies");
  }
#endif

  parents_[task_id].insert(dependency_id);
  children_[dependency_id].insert(task_id);
  roots_.erase(task_id);
  leaves_.erase(dependency_id);
}


TaskManager::IdSet& TaskManager::GetDependencies(std::string task_name) {
  return GetDependencies(GetId(task_name));
}


TaskManager::IdSet& TaskManager::GetDependencies(TaskManager::TaskId id) {
  try {
    return parents_.at(id);
  }
  catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}

#ifdef DEBUG
bool TaskManager::WillCauseCyclicDependency(TaskManager::TaskId task_id,
                                        TaskManager::TaskId target) {
  if (task_id == target) return true;

  TaskManager::TaskId current;
  TaskManager::IdSet visited;
  std::stack<TaskManager::TaskId> pending;

  // traverse upwards from target. If we do meet task_id then the proposed
  // dependency would lead to a cycle
  pending.push(target);
  while (!pending.empty()) {
    current = pending.top();
    pending.pop();
    if (current == task_id) return true;  // cycle detected

    visited.insert(current);

    BOOST_FOREACH(TaskManager::TaskId parent, parents_[current]) {
      if (visited.find(parent) == visited.end()) {  // if node not yet visited
        pending.push(parent);
      }
    }
  }
  return false;
}
#endif

TaskManager::IdSet& TaskManager::GetDependents(std::string task_name) {
  return GetDependents(GetId(task_name));
}


TaskManager::IdSet& TaskManager::GetDependents(TaskManager::TaskId id) {
  try {
    return children_.at(id);
  }
  catch(const std::out_of_range& E) {
    throw flame::exceptions::invalid_argument("Invalid id");
  }
}

size_t TaskManager::GetTaskCount() const {
#ifdef DEBUG
  if (tasks_.size() != children_.size() || tasks_.size() != parents_.size()) {
    throw flame::exceptions::flame_exception("inconsistent data sizes");
  }
#endif
  return tasks_.size();
}


bool TaskManager::IsValidID(TaskManager::TaskId task_id) const {
  return (task_id < tasks_.size());
}

void TaskManager::Finalise() {
  finalised_ = true;
  IterReset();
}

bool TaskManager::IsFinalised() const {
  return finalised_;
}

void TaskManager::IterReset() {
  check_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  pending_deps_ = parents_;  // create copy of dependency tree
  assigned_tasks_.clear();
  ready_tasks_ = IdVector(roots_.begin(), roots_.end());  // tasks with no deps

  // reset and initialise
  pending_tasks_.clear();  // empty existing data
  for (size_t i = 0; i < tasks_.size(); ++i)  {
    if (roots_.find(i) == roots_.end()) {  // task not in ready queue
      pending_tasks_.insert(pending_tasks_.end(), i);
    }
  }
}

bool TaskManager::IterTaskAvailable() const {
  check_finalised(finalised_);
  return (!ready_tasks_.empty());
}

bool TaskManager::IterCompleted() const {
  check_finalised(finalised_);
  return (pending_tasks_.empty()
          && assigned_tasks_.empty()
          && ready_tasks_.empty());
}

size_t TaskManager::IterGetReadyCount() const {
  check_finalised(finalised_);
  return ready_tasks_.size();
}

size_t TaskManager::IterGetPendingCount() const {
  check_finalised(finalised_);
  return pending_tasks_.size();
}

size_t TaskManager::IterGetAssignedCount() const {
  check_finalised(finalised_);
  return assigned_tasks_.size();
}

TaskManager::TaskId TaskManager::IterTaskPop() {
  check_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  if (!IterTaskAvailable()) {
    throw flame::exceptions::none_available("No available tasks");
  }

  TaskManager::TaskId task_id = ready_tasks_.back();
  ready_tasks_.pop_back();
  assigned_tasks_.insert(task_id);
  return task_id;
}

void TaskManager::IterTaskDone(TaskManager::TaskId task_id) {
  check_finalised(finalised_);
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  IdSet::iterator it = assigned_tasks_.find(task_id);
  if (it == assigned_tasks_.end()) {
    throw flame::exceptions::invalid_argument("invalid task id");
  } else {
    assigned_tasks_.erase(it);
  }

  IdSet& dependents = children_.at(task_id);  // tasks that depend on task_id
  for (it = dependents.begin(); it != dependents.end(); ++it) {
    IdSet& d = pending_deps_.at(*it);  // get pending deps for each dependency
    d.erase(task_id);  // this dependency is now fulfilled
    if (d.empty()) {  // all dependencies met?
      ready_tasks_.push_back(*it);  // new task ready for execution
      pending_tasks_.erase(*it);
    }
  }
}


#ifdef TESTBUILD
void TaskManager::Reset() {
  boost::lock_guard<boost::mutex> lock(mutex_task_);

  tasks_.clear();
  name_map_.clear();
  roots_.clear();
  leaves_.clear();
  children_.clear();
  parents_.clear();
  finalised_ = false;
  assigned_tasks_.clear();
  ready_tasks_.clear();
  pending_tasks_.clear();
  pending_deps_.clear();
}
#endif

}}  // namespace flame::exe