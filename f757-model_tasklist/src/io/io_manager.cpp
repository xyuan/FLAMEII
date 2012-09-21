/*!
 * \file src/io/io_manager.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief IOManager: management for I/O Backend
 */
#include <string>
#include "exceptions/io.hpp"
#include "./io_manager.hpp"

namespace flame { namespace io {

int IOManager::loadModel(std::string const& file,
        flame::model::XModel * model) {
    int rc;

    /* Read model */
    rc = ioxmlmodel.readXMLModel(file, model);
    if (rc != 0) return rc;

    return rc;
}

int IOManager::readPop(std::string file_name,
            model::XModel * model,
            FileType fileType) {
    int rc;
    std::string xmlpopxsd;
    flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();

    if (fileType == xml) {
        /* Set path to xml pop location */
        ioxmlpop.setXmlPopPath(file_name);
        /* Validate xml first */
        xmlpopxsd = ioxmlpop.xmlPopPath();
        xmlpopxsd.append("xmlpop.xsd");
        /* Create data schema */
        rc = ioxmlpop.createDataSchema(xmlpopxsd, model);
        if (rc != 0) {
            fprintf(stderr, "Error: Could not create data schema\n");
            return rc;
        }
        /* Validate data using schema */
        rc = ioxmlpop.validateData(file_name, xmlpopxsd);
        if (rc != 0) {
            fprintf(stderr, "Error: Could not validate data with schema\n");
            return rc;
        }
        /* Read validated pop xml */
        rc = ioxmlpop.readXMLPop(file_name, model, &memoryManager);
        return rc;
    } else {
        throw flame::exceptions::flame_io_exception("unknown file type");
    }
}

int IOManager::writePop(std::string file_name,
            int iterationNo,
            model::XModel * model,
            FileType fileType) {
    int rc;
    flame::mem::MemoryManager& memoryManager =
                flame::mem::MemoryManager::GetInstance();

    /* If file type selected is xml */
    if (fileType == xml) {
        /* Check a path has been set */
        if (!ioxmlpop.xmlPopPathIsSet()) {
            fprintf(stderr, "Error: Path not set by reading pop data\n");
            return -1;
        }
        /* Write out pop to xml */
        rc = ioxmlpop.writeXMLPop(file_name,
                iterationNo, model, &memoryManager);
        return rc;
    /* Through exception for unknown file types */
    } else {
        throw flame::exceptions::flame_io_exception("unknown file type");
    }
}

}}  // namespace flame::io
