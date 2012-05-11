/*!
 * \file src/mem/memory_manager.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief MemoryManager: management and storage class for agent data
 */
#ifndef MEM__MEMORY_MANAGER_HPP_
#define MEM__MEMORY_MANAGER_HPP_
#include <map>
#include <string>
#include <vector>
#include "agent_memory.hpp"
#include "vector_wrapper.hpp"

namespace flame { namespace mem {

//! Map to store collection of AgentMemory
typedef std::map<std::string, AgentMemory> AgentMap;


//! Singleton Memory Manager
//! Apart from the Get* methods, all others should be called during the
//! initialisation stage before threads are spawned or guarded by mutexes
class MemoryManager {
  public:
    //! Returns instance of singleton object
    //!  When used in a multithreaded environment, this should be called
    //!  at lease once before threads are spawned.
    static MemoryManager& GetInstance() {
      static MemoryManager instance;
      return instance;
    }

    void RegisterAgent(std::string agent_name);

    template <typename T>
    void RegisterAgentVar(std::string agent_name, std::string var_name) {
      GetAgentMemory(agent_name).RegisterVar<T>(var_name);
    }

    template <typename T>
    void RegisterAgentVar(std::string agent_name,
                          std::vector<std::string> var_names) {
      AgentMemory& am = GetAgentMemory(agent_name);
      std::vector<std::string>::iterator it;
      for (it = var_names.begin(); it != var_names.end(); ++it) {
        am.RegisterVar<T>(*it);
      }
    }

    //! Returns typeless pointer to associated vector wrapper
    VectorWrapperBase* GetVectorWrapper(std::string agent_name,
                                        std::string var_name);

    //! Returns pointer to std::vector<T> for given agent variable
    template <typename T>
    std::vector<T>* GetVector(std::string agent_name, std::string var_name) {
      return GetAgentMemory(agent_name).GetVector<T>(var_name);
    }

    void HintPopulationSize(std::string agent_name, unsigned int size_hint);
    size_t GetAgentCount();

#ifdef TESTBUILD
    void Reset();  //! Delete all agents and vars

#endif

  private:
    // This is a singleton. Disallow instantiation, copy and assignment.
    MemoryManager() {}
    MemoryManager(const MemoryManager&);
    void operator=(const MemoryManager&);

    AgentMap agent_map_;
    AgentMemory& GetAgentMemory(std::string agent_name);
};
}}  // namespace flame::mem
#endif  // MEM__MEMORY_MANAGER_HPP_
