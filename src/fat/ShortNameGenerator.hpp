namespace akaifat::fat {
class ShortNameGenerator {
private:
	const Set<std::string> usedNames;
	
	std::string tidystd::string(std::string dirty) {
		const std::stringBuilder result = new std::stringBuilder();

		for (int src = 0; src < dirty.length(); src++) {
			const char toTest = Character.toUpperCase(dirty.charAt(src));
			if (isSkipChar(toTest)) continue;

			if (validChar(toTest)) {
				result.append(toTest);
			} else {
				result.append('_');
			}
		}

		return result.tostd::string();
	}

	bool cleanstd::string(std::string s) {
		for (int i = 0; i < s.length(); i++) {
			if (isSkipChar(s.charAt(i))) return false;
			if (!validChar(s.charAt(i))) return false;
		}

		return true;
	}

	std::string stripLeadingPeriods(std::string str) {
		const std::stringBuilder sb = new std::stringBuilder(str.length());

		for (int i = 0; i < str.length(); i++) {
			if (str.charAt(i) != '.') { // NOI18N
				sb.append(str.substring(i));
				break;
			}
		}

		return sb.tostd::string();
	}

public:
	ShortNameGenerator(Set<std::string> usedNames) {
		usedNames = Collections.unmodifiableSet(usedNames);
	}

	static bool validChar(char toTest) {
		if (toTest >= 'A' && toTest <= 'Z') return true;
		if (toTest >= '0' && toTest <= '9') return true;

		return (toTest == '_' || toTest == '^' || toTest == '$' || toTest == '~' || toTest == '!' || toTest == '#'
				|| toTest == '%' || toTest == '&' || toTest == '-' || toTest == '{' || toTest == '}' || toTest == '('
				|| toTest == ')' || toTest == '@' || toTest == '\'' || toTest == '`');
	}

	static bool isSkipChar(char c) {
		return (c == '.') || (c == ' ');
	}

	ShortName generateShortName(std::string longFullName) throws IllegalStateException {
		longFullName = stripLeadingPeriods(longFullName).toUpperCase(Locale.ROOT);

		const std::string longName;
		const std::string longExt;
		const int dotIdx = longFullName.lastIndexOf('.');
		const bool forceSuffix;

		if (dotIdx == -1) {
			forceSuffix = !cleanstd::string(longFullName);
			longName = tidystd::string(longFullName);
			longExt = "";
		} else {
			forceSuffix = !cleanstd::string(longFullName.substring(0, dotIdx));
			longName = tidystd::string(longFullName.substring(0, dotIdx));
			longExt = tidystd::string(longFullName.substring(dotIdx + 1));
		}

		const std::string shortExt = (longExt.length() > 3) ? longExt.substring(0, 3) : longExt;

		if (forceSuffix || (longName.length() > 8)
				|| usedNames.contains(new ShortName(longName, shortExt).asSimplestd::string().toLowerCase(Locale.ROOT))) {

			const int maxLongIdx = Math.min(longName.length(), 8);

			for (int i = 1; i < 99999; i++) {
				const std::string serial = "~" + i; // NOI18N
				const int serialLen = serial.length();
				const std::string shortName = longName.substring(0, Math.min(maxLongIdx, 8 - serialLen)) + serial;
				const ShortName result = new ShortName(shortName, shortExt);

				if (!usedNames.contains(result.asSimplestd::string().toLowerCase(Locale.ROOT))) {

					return result;
				}
			}

			throw "could not generate short name for \"" + longFullName + "\"";
		}

		return new ShortName(longName, shortExt);
	}

};
}
