#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <chrono>
#include <ctime>
#include <mutex>
#include <map>
#include <thread>
#include <string>


struct customer {
    std::chrono::time_point<std::chrono::system_clock> startLine;
    int itemsNum;
};

class shop {

public:

    shop(size_t lineSize, size_t checkoutSize, size_t itemTime);
    ~shop();
    void startWork();
    void stopWork();
    void addToLine(std::shared_ptr<customer> cust);
    long getWorkTime();
    std::map<std::string, float> getStats(); 

private:
    struct checkout {
        std::shared_ptr<std::thread> thread;
        bool free = true;
        std::shared_ptr<customer> cust;
        std::chrono::time_point<std::chrono::system_clock> wait;
        bool needJoin = false;
        void _checkoutWork(size_t itemTime);
    };

    size_t _checkoutSize;
    size_t _lineSize;
    std::vector<checkout> _checkouts;
    size_t _servedCust = 0;
    size_t _rejectedCust = 0;
    size_t _totalLineWait = 0;
    size_t _totalServTime = 0;
    size_t _timeWasted = 0;
    size_t _chTimeWorking = 0;
    size_t _totalLineSize = 0;
    std::mutex lineLock;
    std::queue<std::shared_ptr<customer>> line;
    size_t _itemTime;
    bool _work = false;
    std::shared_ptr<std::thread> _workThread;
    std::shared_ptr<std::thread> _countLineSize;
    std::chrono::time_point<std::chrono::system_clock> _startWork;
    size_t _timeWorked = 0;
    void _servCustemers();
    void _addLineSize();
};

