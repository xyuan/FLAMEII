/*!
 * \file src/model/model_manager.hpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief ModelManager: management and storage class for model data
 */
#ifndef MODEL__MODEL_MANAGER_HPP_
#define MODEL__MODEL_MANAGER_HPP_
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <string>
#include <vector>
#include "./xadt.hpp"
#include "./xcondition.hpp"
#include "./xfunction.hpp"
#include "./xioput.hpp"
#include "./xmachine.hpp"
#include "./xmessage.hpp"
#include "./xmodel.hpp"
#include "./xtimeunit.hpp"
#include "./xvariable.hpp"
#include "./task.hpp"
#include "./xgraph.hpp"

namespace flame { namespace model {

class ModelManager {
  public:
    ModelManager() {}
    ~ModelManager();
    int loadModel(std::string const& file);
    int generate_task_list();
    std::vector<Task*> * get_task_list();

  private:
    int catalog_state_dependencies_functions(XModel * model, XGraph * graph);
    int catalog_state_dependencies_transitions(XModel * model, XGraph * graph);
    int catalog_state_dependencies(XModel * model, XGraph * graph);
    int catalog_communication_dependencies_syncs(
            XModel * model, XGraph * graph);
    int catalog_communication_dependencies_ioput(XModel * model,
            std::vector<XFunction*>::iterator function, XGraph * graph);
    int catalog_communication_dependencies(XModel * model,
            XGraph * graph);
    int catalog_data_dependencies_variable(
            std::vector<XMachine*>::iterator agent,
            std::vector<XVariable*>::iterator variable,
            std::vector<Task*> * tasks);
    int catalog_data_dependencies(XModel * model,
            std::vector<Task*> * tasks);
    int check_dependency_loops(XModel * model);
    int calculate_dependencies(std::vector<Task*> * tasks);
    int calculate_task_list(std::vector<Task*> * tasks);
    std::string taskTypeToString(Task::TaskType t);
    void printTaskList(std::vector<Task*> * tasks);
    XModel model_;
    std::vector<Task*> tasks_;
};

}}  // namespace flame::model
#endif  // MODEL__MODEL_MANAGER_HPP_
