#ifndef PHOTOMETRICCORRECTIONHANDLER_H
#define PHOTOMETRICCORRECTIONHANDLER_H

#include <string>
#include <list>
#include <map>

namespace Euclid {
namespace PhosphorosUiDm {

class PhotometricCorrectionHandler
{
public:
    PhotometricCorrectionHandler();

   static std::list<std::string> getCompatibleCorrectionFiles(std::map<std::string,bool> selected_filters);
};

}
}

#endif // PHOTOMETRICCORRECTIONHANDLER_H


