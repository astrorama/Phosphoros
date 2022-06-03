#ifndef SED_PARAM_UTILS_H
#define SED_PARAM_UTILS_H

#include <string>
#include <set>
#include <map>
#include <QObject>
#include "PhzQtUI/ModelSet.h"

/**
 * @brief The SedParamUtils class
 */

namespace Euclid {
namespace PhzQtUI {

class SedParamUtils : public QObject {
	Q_OBJECT
public:
	SedParamUtils();

	virtual ~SedParamUtils() = default;

	void listAvailableParam(const ModelSet& model);

	std::set<std::string> getList();


	static bool createPPConfig(const ModelSet& model, std::set<std::string> params, std::string out_path);

	static std::string getFile(const XYDataset::QualifiedName& sed);

	static std::map<std::string, std::string> getParameterList(const std::string& file);

	static std::string getParameter(const std::string& file, const std::string& key_word);

	static std::string getName(const std::string& file);

signals:
    void progress(size_t, size_t);


private:
  size_t m_progress = 0;
  size_t m_total = 0;
  std::set<std::string> m_list{};

};

}
}

#endif // SED_PARAM_UTILS_H
