#include "ICURL.h"

#include <curl/curl.h>

string ICURL::buffer;

static size_t curl_write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	ICURL::buffer += string((const char*)ptr, size * nmemb);
  return size * nmemb;
}

int ICURL::fetchPage(const string& url, string& content)
{
	CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();
  if(curl)
	{
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
 
		buffer = "";
    res = curl_easy_perform(curl);
		content = buffer;

    if(res != CURLE_OK)
		{
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			return 0;
		}
		else
		{
			curl_easy_cleanup(curl);
			return 1;
		}
  }
}
