/*
 * CheckString.h
 *
 *  Created on: Oct 28, 2014
 *      Author: Nicolas Morisset
 */

#ifndef CHECKSTRING_H_
#define CHECKSTRING_H_

#include <boost/regex.hpp>

// The ApplyRegex function applies a provided regex (regex_string) to a
// string (string_to_parse) in order to detect any not wanted character(s)
// Params:
// regex_string    : string, the regex to be applied
// option          : string, the name of the parameter to be checked
// string_to_parse : string, the string to apply the regex
// throw:
// invalid character(s) found
inline void ApplyRegex(const std::string& regex_string,
                       const std::string& option,
                       const std::string& string_to_parse)
{

  boost::regex expression(regex_string);
  if (!boost::regex_match(string_to_parse, expression)) {
    throw Elements::Exception() <<"Invalid character(s) found for the \""
                                << option << "\" option : \""
                                << string_to_parse
                                << "\"";
  }
}


// The checkRangeString function applies a provided regex (regex_string) to a
// string (string_to_parse) in order to detect any not wanted character(s) for
// a range option type and throws an exception if any
// Params:
// option          : string, the name of the parameter to be checked
// string_to_parse : string, the string to apply the regex
inline void checkRangeString(const std::string& option,
                             const std::string& string_to_parse)
{
  // Regex for the range option where the string expected is as follows:
  // "min max step"
  //  where min max and step are doubles so only digits and/or dot are expected
  const std::string regex_string{"(((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}"};
  ApplyRegex(regex_string, option, string_to_parse);
}

// The checkValueString function applies a provided regex (regex_string) to a
// string (string_to_parse) in order to detect any not wanted character(s) for
// a value type option and throws an exception if any
// Params:
// option          : string, the name of the parameter to be checked
// string_to_parse : string, the string to apply the regex
inline void checkValueString( const std::string& option,
                              const std::string& string_to_parse)
{
  // Regex for the value option where the string expected is as follows:
  // "value"  where value is a double so only digits and/or dot characters
  // are expected
  const std::string regex_string{"((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)"};
  ApplyRegex(regex_string, option, string_to_parse);
}



#endif /* CHECKSTRING_H_ */
