#include "kutils.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

#ifndef WIN32
#include <grp.h>
#endif

using namespace std;

uint32_t getTicks ()
{
	timeval tv;
	gettimeofday (&tv, 0);
	uint32_t val = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return val;
}
uint32_t getTicksUS ()
{
	timeval tv;
	gettimeofday (&tv, 0);
	uint32_t val = tv.tv_sec * 1000000 + tv.tv_usec;
	return val;
}
std::string getErrnoString ()
{
  char buf[256];
  char *str = strerror_r (errno, (char*)&buf, 256);
  return str;
}

vector<std::string> explode (const std::string& str, const std::string& delim, size_t maxCount, size_t start)
{
  vector<std::string> parts;
  size_t idx = start, delimIdx;

  delimIdx = str.find (delim, idx);
  if (delimIdx == string::npos) { parts.push_back (str); return parts; }
  do
  {
    if (parts.size () == maxCount - 1)
    {
      string part = str.substr (idx);
      parts.push_back (part);
      idx = str.size ();
      break;
    }
    string part = str.substr (idx, delimIdx - idx);
    parts.push_back (part);
    idx = delimIdx + delim.size ();
    delimIdx = str.find (delim, idx);
  } while (delimIdx != string::npos && idx < str.size ());

  if (idx < str.size ())
  {
    string part = str.substr (idx);
    parts.push_back (part);
  }

  return parts;
}
std::string replaceAll (const string& str, const string& what, const string& replacement)
{
	string newStr = "";
	size_t idx = 0;
	size_t pos;
	while (idx < str.size ())
	{
		// printf ("idx: %d\n", idx);
		pos = str.find (what, idx);
		// printf ("pos: %d\n", pos);
		// printf ("p1os: %d\n", what.size ());
		if (pos == string::npos)
		{
			newStr += str.substr (idx);
			break;
		}
		else
		{
			// 012345
			// abcdcd
			newStr += str.substr (idx, pos - idx) + replacement;
			idx = pos + what.size ();
		}
	}
	return newStr;
}
std::string removeChars (const std::string& str, const std::string& chars)
{
	string newStr = "";
	for (size_t i = 0; i < str.size (); i++)
	{
		if (chars.find_first_of (str[i]) == string::npos)
			newStr += str[i];
	}
	return newStr;
}
std::string joinStrings (const std::string& sep, const std::vector<std::string>& strings, size_t start, int count)
{
	string str = "";
	for (size_t i = start; i < strings.size () && count--; i++)
	{
		if (i != start)
			str += sep;
		str += strings[i];
	}
	return str;
}
std::string ltrim (const std::string& str, const std::string& chars)
{
	size_t idx = 0;
	for (; idx < str.size () && chars.find_first_of (str[idx]) != string::npos; idx++);
	return str.substr (idx);
}
std::string rtrim (const std::string& str, const std::string& chars)
{
	int idx = str.size () - 1;
	for (; idx >= 0 && chars.find_first_of (str[idx]) != string::npos; idx--);
	return str.substr (0, idx + 1);
}
std::string trim (const std::string& str, const std::string& chars)
{
	return ltrim (rtrim (str, chars), chars);
}
std::string xorStrings (const std::string& text, const std::string& key)
{
	std::string str = "";
	for (size_t i = 0; i < text.size (); i++)
		str += text[i] ^ key[i % key.size ()];
	return str;
}

#ifndef WIN32
int changeUidGid (int uid, gid_t gid)
{
	if (setgid (gid) == -1)
	{
		return 1;
	}
	gid_t gidl[1] = { gid };
	if (setgroups (1, gidl) == -1)
	{
		return 1;
	}
	if (setuid (uid) == -1)
	{
		return 1;
	}

	return 0;
}
#endif

std::vector<std::string> parseArgs (std::string str, int count)
{
	vector<std::string> args;
	str = trim (str);
	str = replaceAll (str, "\\\"", "\1");

	if (str.size () == 0) return args;

	if (count == 1)
	{
		args.push_back (str);
		return args;
	}

	vector<string> parts = explode (str, " ");
	vector<string> newParts;

	string tmpArg = "";
	bool capt = false;
	size_t i;
	for (i = 0; i < parts.size (); i++)
	{
		string p = parts[i];

		if (capt)
		{
			if (p.size () == 0)
			{
				tmpArg += " ";
			}
			else if (p[p.size () - 1] == '"')
			{
				tmpArg += " " + p.substr (0, p.size () - 1);
				newParts.push_back (tmpArg);
				tmpArg = "";
				capt = false;
			}
			else
			{
				tmpArg += " " + p;
			}
		}
		else
		{
			if (p.size () == 0)
			{
				// newParts.push_back (" ");
			}
			else if (p.size () > 1 && p[0] == '"' && p[p.size () - 1] == '"')
			{
				newParts.push_back (p.substr (1, p.size () - 2));
			}
			else if (p[0] == '"')
			{
				if (p.size () != 1)
					tmpArg = p.substr (1);
				else
					tmpArg = "";
				capt = true;
			}
			else
			{
				newParts.push_back (p);
			}
		}

		if (count != -1 && (int)newParts.size () == count - 1)
			break;
	}

	// for (int j = 0; j < parts.size (); j++)
	// {
		// cout << "p: |" << parts[j] << "|\n";
	// }
	// cout << endl;

	if (i + 1 < parts.size ())
	{
		tmpArg = joinStrings (" ", parts, i + 1);
		newParts.push_back (tmpArg);
	}

	for (size_t j = 0; j < newParts.size (); j++)
	{
		newParts[j] = replaceAll (newParts[j], "\1", "\\\"");
		// cout << "np: |" << newParts[j] << "|\n";
	}

	// cout << "|"<<str<<"|\n";

	return newParts;
}
