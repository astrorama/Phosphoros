/** 
 * @file ConfigManager_fixture.h
 * @date Nov 6, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE
#define _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE

#include <chrono>
#include "Configuration/ConfigManager.h"

struct ConfigManager_fixture {
  
  long timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::system_clock::now().time_since_epoch()).count();
  
  Euclid::Configuration::ConfigManager& config_manager = 
                      Euclid::Configuration::ConfigManager::getInstance(timestamp);
 
};

#endif // _PHZCONFIGURATION_CONFIGMANAGER_FIXTURE

