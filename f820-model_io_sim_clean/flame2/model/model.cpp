/*!
 * \file flame2/model/model.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Model: model object for users to use
 */
#include <cstdio>
#include <string>
#include "flame2/config.hpp"
#include "flame2/io/io_manager.hpp"
#include "model.hpp"

namespace flame {
namespace model {

Model::Model(std::string path_to_model)
    : modelLoaded_(false) {
    flame::io::IOManager& ioManager = flame::io::IOManager::GetInstance();
    int rc = 0;

    // Load model
    try {
        // Call ioManager to load model
        ioManager.loadModel(path_to_model, &model_);
    }
    // Catch exception
    catch(const flame::exceptions::flame_io_exception& E) {
        std::fprintf(stderr, "Error: %s\n", E.what());
        model_.clear();
        return;
    }

    // Validate model
    rc = model_.validate();
    if (rc != 0) {
std::fprintf(stderr, "Error: Model from XML file could not be validated.\n");
        model_.clear();
        return;
    }

    modelLoaded_ = true;
}

Model::~Model() {
}

flame::model::XModel * Model::getXModel() {
    return &model_;
}

void Model::registerAgentFunction(std::string name,
        flame::exe::TaskFunction f_ptr) {
    model_.registerAgentFunction(name, f_ptr);
}

}}  // namespace flame::model