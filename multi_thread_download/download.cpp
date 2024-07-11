#include "download.h"
#include "threadpool.h"
#include <functional>
#include <future>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

char* downloader::mmap_data_to_file(off_t& mmap_offset, int& fd, size_t file_size)
{
    /* 目录不存在，则创建 */
    if (access(path_.c_str(), F_OK) != 0)
    {
        mkdir(path_.c_str(), 0766);
    }
    const std::string file = path_ + "/" + filename_;

	fd = open(file.c_str (), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        throw std::string("failed to open file.");
    }
    
    mmap_offset = lseek(fd, file_size, SEEK_SET);
	if (lseek(fd, file_size - 1, SEEK_SET) == -1) {
        close(fd);
        throw std::string("faile to set file size.");
    }

	if (write(fd, "", 1) != 1) {
        close(fd);
        throw std::string("failed to write data.");
    }

	char *mmap_ptr = static_cast<char*>(mmap(nullptr, mmap_offset,
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
	if (mmap_ptr == MAP_FAILED) {
        close(fd);
        throw std::string ("failed to mmap file.");
    }

    return mmap_ptr;
        
}

bool downloader::run()
{
    size_t file_size = parse_data_length();

    off_t mmap_offset = 0;
    int fd = 0;
    char* mmap_ptr = mmap_data_to_file(mmap_offset, fd, file_size);
    if (mmap_ptr == nullptr) {
        close(fd);
        throw std::string("failed to mmap to file.");
    }

    auto &thread_pool = ThreadPool::instance();
    int thread_num = thread_pool.idle_thread_count();
    
    /* 为线程分配下载范围 */
    long part_size = file_size / thread_num;
    std::vector<Node*> data_node(thread_num + 1, nullptr);
    for (int i = 0; i <= thread_num; ++i) {
        data_node[i] = new Node();
        data_node[i]->start_pos = i * part_size;
        if (i < thread_num) {
            data_node[i]->end_pos = (i + 1) * part_size - 1;
        } else {
            data_node[i]->end_pos = file_size - 1;
        }

        data_node[i]->ptr = mmap_ptr;
        data_node[i]->url = url_;
    }

    std::vector<std::future<void*>> ret_vec;

    for (int i = 0; i <= thread_num; ++i) {
        auto worker_func = std::bind(&downloader::worker, this, std::placeholders::_1);
        auto ret = thread_pool.commit(worker_func, data_node[i]);
        ret_vec.emplace_back(std::move(ret));
    }

    /* 用于等待任务完成 */
    for (auto& ret : ret_vec) {
        ret.get();
    }

    for (int i = 0; i <= thread_num; ++i) {
        delete data_node[i];
        data_node[i] = nullptr;
    }


    munmap(mmap_ptr, file_size);
	close (fd);
    return true;
}
