#include "bot_chatgpt.hpp"
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

Bot_ChatGPT::Bot_ChatGPT(const std::string &host,
						int port,
						const std::string &password,
						const std::string &user,
						const std::string &realName)
	: Bot(host, port, password, user, realName)
{
}

std::string Bot_ChatGPT::ApiCall(const std::string &prompt)
{
	CURL* curl = curl_easy_init();
	if (!curl) 
		return "Error: unable to init cURL";

	std::string response;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	struct curl_slist* headers = NULL;
	
	curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "Authorization: Bearer <weneedtheapikeyhere>");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string jsonBody =
		"{"
			"\"model\":\"gpt-3.5-turbo\","
			"\"messages\":[{\"role\":\"user\",\"content\":\"" + prompt + "\"}]"
		"}";

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		return "cURL request failed: " + std::string(curl_easy_strerror(res));

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	size_t contentPos = response.find("\"content\":\"");
	if (contentPos != std::string::npos) {
		contentPos += 11; // Length of "content":"
		size_t endPos = response.find("\"", contentPos);
		if (endPos != std::string::npos) {
			return response.substr(contentPos, endPos - contentPos);
		}
	}
	return "Error parsing response";
}