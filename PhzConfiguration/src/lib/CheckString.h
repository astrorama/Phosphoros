/*
 * CheckString.h
 *
 *  Created on: Oct 28, 2014
 *      Author: Nicolas Morisset
 */

#ifndef CHECKSTRING_H_
#define CHECKSTRING_H_


// This function applies a provided regex (regex_string) to a string
// (string_to_parse) in order to detect any not wanted character(s) and
// throws an exception if any
// Params:
// regex_string    : string, the regex to be used
// option          : string, the name of the parameter to be checked
// string_to_parse : string, the string to apply the regex
inline void checkString(const std::string& regex_string,
                        const std::string& option,
                        const std::string& string_to_parse)
{
  boost::regex expression(regex_string);
  if (!boost::regex_match(string_to_parse, expression)) {
    throw Elements::Exception() <<"Invalid character(s) found for the option: "
                                << option << " = " << string_to_parse;
  }
}



#endif /* CHECKSTRING_H_ */
