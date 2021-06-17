#pragma once

#include <set>
#include <string>

namespace akaifat::fat {
class AkaiNameGenerator {
private:
   std::set<std::string> usedNames;

  std::string tidyString(std::string dirty)
  {
//    std::stringBuilder result = new std::stringBuilder();
//
//    /* epurate it from alien characters */
//    for (int src = 0; src < dirty.length(); src++) {
//      char toTest = Character.toUpperCase(dirty.charAt(src));
//      if (isSkipChar(toTest)) continue;
//
//      if (validChar(toTest)) {
//        result.append(toTest);
//      } else {
//        result.append('_');
//      }
//    }
//
//    return result.tostd::string();
      return "";
  }

  bool cleanString(std::string& s)
  {
    for (int i = 0; i < s.length(); i++)
    {
      if (isSkipChar(s[i])) return false;
      if (!validChar(s[i])) return false;
    }

    return true;
  }

  std::string stripLeadingPeriods(std::string& str)
  {
//    std::stringBuilder sb = new std::stringBuilder(str.length());
//
//    for (int i = 0; i < str.length(); i++) {
//      if (str.charAt(i) != '.') {
//        sb.append(str.substring(i));
//        break;
//      }
//    }
//
//    return sb.tostd::string();
      return "";
  }

public:
	AkaiNameGenerator(std::set<std::string>& _usedNames)
  : usedNames (_usedNames)
  {
	}

	static bool validChar(char toTest)
  {
		if (toTest >= 'A' && toTest <= 'Z') return true;
		if (toTest >= '0' && toTest <= '9') return true;

		return (toTest == '_' || toTest == '^' || toTest == '$' || toTest == '~' || toTest == '!' || toTest == '#'
				|| toTest == '%' || toTest == '&' || toTest == '-' || toTest == '{' || toTest == '}' || toTest == '('
				|| toTest == ')' || toTest == '@' || toTest == '\'' || toTest == '`');
	}

	static bool isSkipChar(char c)
  {
		return (c == '.') || (c == ' ');
	}

	std::string generateAkaiName(std::string longFullName)
  {
//		longFullName = stripLeadingPeriods(longFullName).toUpperCase(Locale.ROOT);
//
//		std::string longName;
//		std::string longExt;
//		int dotIdx = longFullName.lastIndexOf('.');
//
//		if (dotIdx == -1)
//    {
//			longName = tidystd::string(longFullName);
//			longExt = "";
//		}
//    else
//    {
//			cleanString(longFullName.substring(0, dotIdx));
//			longName = tidystd::string(longFullName.substring(0, dotIdx));
//			longExt = tidystd::string(longFullName.substring(dotIdx + 1));
//		}
//
//		std::string shortExt = (longExt.length() > 3) ? longExt.substring(0, 3) : longExt;
//
//		if ((longName.length() > 16 && usedNames.contains((longName.substring(0, 16) + "." + shortExt).toLowerCase(Locale.ROOT)))
//					|| usedNames.contains((longName + "." + shortExt).toLowerCase(Locale.ROOT)))
//    {
//			int maxLongIdx = Math.min(longName.length(), 16);
//
//			for (int i = 1; i < 99999; i++) {
//				std::string serial = "~" + i;
//				int serialLen = serial.length();
//				std::string shortName = longName.substring(0, Math.min(maxLongIdx, 16 - serialLen)) + serial;
//
//				if (!usedNames.contains((shortName + "." + shortExt).toLowerCase(Locale.ROOT))) {
//
//					return shortName + "." + shortExt;
//				}
//			}
//
//			throw "could not generate short name for \"" + longFullName + "\"";
//		}
//
//		if (longName.length() > 16) longName = longName.substring(0, 16);
//
//		return longName + "." + shortExt;
        return "";
	}

};
}
