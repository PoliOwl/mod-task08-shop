#include "task.h"

shop::shop(size_t lineSize, size_t checkoutSize, size_t itemTime) :
    _checkoutSize(checkoutSize),
    _lineSize(lineSize),
    _itemTime(itemTime),
    _checkouts(std::vector<checkout>(_checkoutSize))
{}

void shop::_servCustemers() {
    while (_work) {
        for (auto& el : _checkouts) {
            if (el.free && line.size()) {
                lineLock.lock();
                el.free = false;
                if (el.needJoin) {
                    el.thread->join();
                }
                auto endLienTime = std::chrono::system_clock::now();
                el.cust = line.front();
                line.pop();
                _totalLineWait +=
                std::chrono::duration_cast<std::chrono::milliseconds>(endLienTime - el.cust->startLine).count();
                _chTimeWorking += el.cust->itemsNum* _itemTime;
                _totalServTime += el.cust->itemsNum* _itemTime;
                _timeWasted +=
                std::chrono::duration_cast<std::chrono::milliseconds>(endLienTime - el.wait).count();
                size_t sleep = el.cust->itemsNum * _itemTime;
                el.needJoin = true;
                el.thread = std::make_shared<std::thread>([&] {el._checkoutWork(_itemTime); });
                lineLock.unlock();
            }
        }
    }
    while (line.size()) {
        for (auto& el : _checkouts) {
            if(line.size() == 0) {
                break;
            }
            if (el.free && line.size()) {
                el.free = false;
                if (el.needJoin) {
                    el.thread->join();
                }
                auto endLienTime = std::chrono::system_clock::now();
                el.cust = line.front();
                line.pop();
                _chTimeWorking += el.cust->itemsNum* _itemTime;
                _totalServTime += el.cust->itemsNum* _itemTime;
                _timeWasted += std::chrono::duration_cast<std::chrono::milliseconds>(endLienTime - el.wait).count();
                _totalLineWait += std::chrono::duration_cast<std::chrono::milliseconds>(endLienTime - el.cust->startLine).count();
                el.thread = std::make_shared<std::thread>([&] {el._checkoutWork(_itemTime); });
            }
        }
    }
}

void shop::addToLine(std::shared_ptr<customer> cust) {
    if (_work) {
        lineLock.lock();
        if (line.size() < _lineSize) {
            cust->startLine = std::chrono::system_clock::now();
            line.push(cust);
            ++_servedCust;
        }
        else {
            ++_rejectedCust;
        }
        lineLock.unlock();
    }
}

void shop::startWork() {
    _work = true;
    _startWork = std::chrono::system_clock::now();
    for (auto& el : _checkouts) {
        el.wait = std::chrono::system_clock::now();
    }
    _workThread = std::make_shared<std::thread>([&] {_servCustemers(); });
    _countLineSize = std::make_shared<std::thread>([&] {_addLineSize(); });
}

void shop::stopWork() {
    _work = false;
    _countLineSize->join();
    _workThread->join();
    for (auto& el : _checkouts) {
        if (el.needJoin) {
            el.thread->join();
        }
    }
    auto endTime = std::chrono::system_clock::now();
    _timeWorked = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - _startWork).count();
}

long shop::getWorkTime() {
    if (_work) {
        auto endTime = std::chrono::system_clock::now();
        auto workTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - _startWork).count();
        return workTime;
    } else {
        return _timeWorked;
    }
}

std::map<std::string, float> shop::getStats() {
    auto time = getWorkTime();
    std::map<std::string, float> ans = {
        {"served custemers", _servedCust},
        {"rejected custemers", _rejectedCust},
        {"average custemer time", float((_totalServTime +  _totalLineWait) / _servedCust)},
        {"average checkout work time", float(_chTimeWorking) / _checkoutSize},
        {"average checkout wasted time", float(_timeWasted) / _checkoutSize},
        {"average line size", float(_totalLineSize) / (int(time / 100))},
        {"total shop work time", time}
    };
    return ans;
}

void shop::checkout::_checkoutWork(size_t itemTime) {
    auto sleepTime = cust->itemsNum * itemTime;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    wait = std::chrono::system_clock::now();
    free = true;
}

void shop::_addLineSize() {
    while (_work) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        _totalLineSize += line.size();
    }
}

shop::~shop() {
    if (_work) {
        this->stopWork();
    }
}