#ifndef FORMUTILS_H
#define FORMUTILS_H
#include <QString>
#include <QLocale>


namespace Euclid {
namespace PhzQtUI {

class FormUtils
{
public:
  FormUtils();


    static double parseToDouble(QString string){
      QLocale locale{};
      bool ok;
      double d = locale.toDouble(string, &ok );
      if(ok) {
         return d;
      }

      return 0.;
    }


};

}
}
#endif // FORMUTILS_H
