#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#include <string>
#include "threadpool.h"

typedef struct Node
{
	size_t start_pos;
	size_t end_pos;
	char *ptr; //mmap_ptr
	std::string url;
}Node;

class downloader {
public:
    downloader(const std::string& url, const std::string& path, const std::string& filename)
        : url_(url), path_(path), filename_(filename)
    {
    }

    virtual ~downloader() = default;

    char* mmap_data_to_file(off_t& mmap_offset, int& fd, size_t file_size);
    bool run();
    virtual void* worker(Node *node) = 0;
    virtual size_t parse_data_length() = 0;

protected:
    std::string url_;   /* 待下载的url */
    std::string path_;  /* 输出路径 */
    std::string filename_;  /* 输出文件名 */
};

#endif
