#ifndef __CURL_H__
#define __CURL_H__

#include <string>
using namespace std;

class ICURL 
{
public:
	static int fetchPage(const string& url, string& content);

// private:
	static string buffer;
};

#endif
