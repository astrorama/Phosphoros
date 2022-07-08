/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file PhzCLI/LsAuxDirConfig.h
 * @date 11/16/15
 * @author morisset
 */

#ifndef _PHZCLI_LSAUXDIRCONFIG_H
#define _PHZCLI_LSAUXDIRCONFIG_H

#include <string>

#include "Configuration/Configuration.h"
#include "XYDataset/XYDatasetProvider.h"

namespace Euclid {
namespace PhzCLI {

/**
 * @class LsAuxDirConfig
 * @brief Configuration class for the Auxiliary data used by
 * the LsAux executable.
 *
 */
class LsAuxDirConfig : public Configuration::Configuration {

public:
  /// Constructor
  LsAuxDirConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~LsAuxDirConfig() = default;

  /**
   * @brief
   * Returns the program options defined by the LsAuxDirConfig
   *
   * @details
   *
   *
   * @return The map with the option descriptions
   */
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  /// Initializes
  void initialize(const UserValues& args) override;

  std::shared_ptr<XYDataset::XYDatasetProvider> getDatasetProvider() {
    return m_provider;
  }

  const std::string& getGroup() const {
    return m_group;
  }

  const std::string& getDatasetToShow() const {
    return m_dataset_to_show;
  }

  const bool& showData() const {
    return m_show_data;
  }

private:
  std::shared_ptr<XYDataset::XYDatasetProvider> m_provider;
  std::string                                   m_group;
  std::string                                   m_dataset_to_show;
  bool                                          m_show_data{false};

}; /* End of LsAuxDirConfig class */

} /* namespace PhzCLI */
} /* namespace Euclid */

#endif
