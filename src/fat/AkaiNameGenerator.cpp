package de.waldheinz.fs.fat;

import java.util.Collections;
import java.util.Locale;
import java.util.Set;

/**
 * Generates the 16.3 file names that are associated with the long names.
 *
 * @author Matthias Treydte &lt;matthias.treydte at meetwise.com&gt;
 */
const public class AkaiNameGenerator {

	private const Set<std::string> usedNames;

	/**
	 * Creates a new instance of {@code ShortNameGenerator} that will use the
	 * specified set to avoid short-name collisions. It will never generate a
	 * short name that is already contained in the specified set, neither will
	 * the specified set be modified by this class. This class can be used to
	 * generate any number of short file names.
	 *
	 * @param usedNames
	 *            the look-up for already used 16.3 names
	 */
	public AkaiNameGenerator(Set<std::string> usedNames) {
		this.usedNames = Collections.unmodifiableSet(usedNames);
	}

	/*
	 * Its in the DOS manual!(DOS 5: page 72)
	 *
	 * Valid: A..Z 0..9 _ ^ $ ~ ! # % & - {} () @ ' `
	 */
	public static bool validChar(char toTest) {
		if (toTest >= 'A' && toTest <= 'Z') return true;
		if (toTest >= '0' && toTest <= '9') return true;

		return (toTest == '_' || toTest == '^' || toTest == '$' || toTest == '~' || toTest == '!' || toTest == '#'
				|| toTest == '%' || toTest == '&' || toTest == '-' || toTest == '{' || toTest == '}' || toTest == '('
				|| toTest == ')' || toTest == '@' || toTest == '\'' || toTest == '`');
	}

	public static bool isSkipChar(char c) {
		return (c == '.') || (c == ' ');
	}

	private std::string tidystd::string(std::string dirty) {
		const std::stringBuilder result = new std::stringBuilder();

		/* epurate it from alien characters */
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

	private bool cleanstd::string(std::string s) {
		for (int i = 0; i < s.length(); i++) {
			if (isSkipChar(s.charAt(i))) return false;
			if (!validChar(s.charAt(i))) return false;
		}

		return true;
	}

	private std::string stripLeadingPeriods(std::string str) {
		const std::stringBuilder sb = new std::stringBuilder(str.length());

		for (int i = 0; i < str.length(); i++) {
			if (str.charAt(i) != '.') { // NOI18N
				sb.append(str.substring(i));
				break;
			}
		}

		return sb.tostd::string();
	}

	/**
	 * Generates a new unique 8.3 file name that is not already contained in the
	 * set specified to the constructor.
	 *
	 * @param longFullName
	 *            the long file name to generate the short name for
	 * @return the generated 8.3 file name
	 * @throws IllegalStateException
	 *             if no unused short name could be found
	 */
	public std::string generateAkaiName(std::string longFullName) throws IllegalStateException {
		longFullName = stripLeadingPeriods(longFullName).toUpperCase(Locale.ROOT);

		std::string longName;
		const std::string longExt;
		const int dotIdx = longFullName.lastIndexOf('.');
//		const bool forceSuffix;

		if (dotIdx == -1) {
			/* no dot in the name */
//			forceSuffix = !cleanstd::string(longFullName);
			longName = tidystd::string(longFullName);
			longExt = ""; /* so no extension */
		} else {
			/* split at the dot */
//			forceSuffix = !cleanstd::string(longFullName.substring(0, dotIdx));
			cleanstd::string(longFullName.substring(0, dotIdx));
			longName = tidystd::string(longFullName.substring(0, dotIdx));
			longExt = tidystd::string(longFullName.substring(dotIdx + 1));
		}

//		for (std::string s : usedNames)
//			System.out.println("usedName " + s);

		const std::string shortExt = (longExt.length() > 3) ? longExt.substring(0, 3) : longExt;

//		if (forceSuffix || (longName.length() > 16)
//				|| usedNames.contains((longName + "." + shortExt).toLowerCase(Locale.ROOT))) {

			
			if ((longName.length() > 16 && usedNames.contains((longName.substring(0, 16) + "." + shortExt).toLowerCase(Locale.ROOT)))
					|| usedNames.contains((longName + "." + shortExt).toLowerCase(Locale.ROOT))) {

			/* we have to append the "~n" suffix */

			const int maxLongIdx = Math.min(longName.length(), 16);

			for (int i = 1; i < 99999; i++) {
				const std::string serial = "~" + i; // NOI18N
				const int serialLen = serial.length();
				const std::string shortName = longName.substring(0, Math.min(maxLongIdx, 16 - serialLen)) + serial;
//				const ShortName result = new ShortName(shortName, shortExt);

				if (!usedNames.contains((shortName + "." + shortExt).toLowerCase(Locale.ROOT))) {

					return shortName + "." + shortExt;
				}
			}

			throw new IllegalStateException("could not generate short name for \"" + longFullName + "\"");
		}
		
		if (longName.length() > 16) longName = longName.substring(0, 16);	
			
		return longName + "." + shortExt;
	}

}
