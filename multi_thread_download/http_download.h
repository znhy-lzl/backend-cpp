#ifndef __HTTP_DOWNLOAD__
#define __HTTP_DOWNLOAD__

#include <string>

#include "download.h"

using std::string;

class http_downloader : public downloader{
public:
    http_downloader(const string &url, const string &path, const string &filename)
        : downloader(url, path, filename)
    {
    }

    ~http_downloader() = default;

    void* worker(Node *node) override;
    size_t parse_data_length() override;
};

#endif
