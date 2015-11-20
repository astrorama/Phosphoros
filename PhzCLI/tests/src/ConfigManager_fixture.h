/** 
 * @file ConfigManager_fixture.h
 * @date Nov 6, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE
#define _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE

#include <chrono>
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"

struct ConfigManager_fixture {
  
  long timestamp = Euclid::Configuration::getUniqueManagerId();
  
  Euclid::Configuration::ConfigManager& config_manager = 
                      Euclid::Configuration::ConfigManager::getInstance(timestamp);
 
};

#endif // _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE

