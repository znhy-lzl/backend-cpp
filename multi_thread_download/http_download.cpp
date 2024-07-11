#include <cstddef>
#include <cstdio>
#include <curl/curl.h>
#include <curl/easy.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "http_download.h"

std::mutex mtx;

static size_t write_cb (void *ptr, size_t size, size_t nitems, void *userdata);
static size_t get_content_length(char *ptr, size_t size, size_t nitems, void *userdata);

void* http_downloader::worker(Node *node) {
	std::thread::id thread_id = std::this_thread::get_id();

	CURL *curl = curl_easy_init();

	char range[64] = { 0 };
	snprintf (range, sizeof (range), "%ld-%ld", node->start_pos, node->end_pos);

	mtx.lock();
	std::cout << "thread id: " << thread_id << ", download range: " << range << std::endl;
	mtx.unlock();
	curl_easy_setopt(curl, CURLOPT_URL, node->url.c_str ());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)node);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1200L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 500L);
	curl_easy_setopt(curl, CURLOPT_RANGE, range);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, 
		"Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
		"AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36");

	CURLcode res = curl_easy_perform(curl);
 
	if (res != CURLE_OK)
	{
		throw std::string(curl_easy_strerror(res) + std::string(". curl code: ") + std::to_string(res));
	}
	curl_easy_cleanup(curl);

	return node;
}

size_t http_downloader::parse_data_length()
{
    long file_size = 0;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, 
		"Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
		"AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, get_content_length);
    curl_easy_setopt(curl,  CURLOPT_HEADERDATA, &file_size);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	
    CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
        throw std::string(curl_easy_strerror(res));
    }

    std::cout << "download file length: " << file_size << std::endl;
	curl_easy_cleanup(curl);
	return file_size;
}

static size_t write_cb (void *ptr, size_t size, size_t nitems, void *userdata)
{

	Node *node = (Node *) userdata;
	size_t written = 0;

	if ( (node->start_pos + size * nitems) <= node->end_pos) {
		memcpy(node->ptr + node->start_pos, ptr, size * nitems);
		node->start_pos += size * nitems;
		written = size * nitems;
	}
	else {
		if (node->start_pos + size * nitems > node->end_pos + 1) {
			written = node->end_pos - node->start_pos + 1;
		} else {
			written = size * nitems;
		}
		memcpy(node->ptr + node->start_pos, ptr, written);
	}

	return size * nitems;
}

static size_t get_content_length(char *ptr, size_t size, size_t nitems, void *userdata)
{
    long len = 0;
    int ret = sscanf(ptr, "Content-Length: %ld\n", &len);
    if (ret <= 0) {
        ret = sscanf(ptr, "content-length: %ld\n", &len);
    }
	
    if (ret) {
        *((long*)userdata) = len;
    }
    return size * nitems;
}
