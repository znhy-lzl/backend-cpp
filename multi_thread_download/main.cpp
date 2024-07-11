#include "download.h"
#include "http_download.h"
#include <curl/curl.h>
#include<iostream>
#include <string>
#include <unistd.h>

void show_usage()
{
    std::string info = "usage: \n" \
    "-m [http | ftp]: 下载模式选择\n" \
    "-h:              帮助\n";
    std::cout << info << std::endl;
}

int main(int argc, char *argv[]) {

    if (argc <= 4) {
        std::cout << "usage: " << argv[0] << " <url> <output dir> <file> -m mode[http | ftp]"  << std::endl;
        return -1;
    }

    std::string url = argv[1];
    std::string path = argv[2];
    std::string filename = argv[3];

    int opt = 0;
    std::string mode;

    while ((opt = getopt(argc, argv, "m:h")) != -1) {
        switch (opt) {
            case 'm':
                mode = optarg;
                break;
            case 'h':
                show_usage();
                return 0;
        }

    }

    CURLcode res;
    res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        return -1;
    }

    downloader *download = nullptr;
    if (mode == "http") {
        /* 使用http下载文件 */
        download = new http_downloader(url, path, filename);
        
    } else if (mode == "ftp") {
        std::cout << "note: ftp is todolist" << std::endl;
    }

    std::cout << "download " << filename << "..." << std::endl;
    if (download) {
        try {
            download->run();
            std::cout << "download success." << std::endl;
        } catch(const std::string& msg) {
            std::cerr << "download failure, error is: " << msg << std::endl;
        }
        delete download;
    }
    
    curl_global_cleanup();

    return 0;
}
