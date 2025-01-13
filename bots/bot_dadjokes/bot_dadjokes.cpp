#include "bot_dadjokes.hpp"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

Bot_DadJokes::Bot_DadJokes(
	const std::string& host,
	int port,
	const std::string& password,
	const std::string& user,
	const std::string& realName
) : Bot(host, port, password, user, realName)
{
}

std::string Bot_DadJokes::ApiCall()
{
	CURL* curl;
	CURLcode res;
	std::string response;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "https://icanhazdadjoke.com/");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Accept: text/plain");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			response = "cURL request failed: " + std::string(curl_easy_strerror(res));
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return response;
}