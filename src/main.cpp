#include "task.h"
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
    int checkoutNum, custemerFlow, itemTime, averageCart, lineSize, totalCustemNum;
    if (argc < 7) {
        checkoutNum = 10;
        custemerFlow = 15;
        itemTime = 20;
        averageCart = 25;
        lineSize = 10;
        totalCustemNum = 10000;
    }
    else {
        checkoutNum = std::stoi(argv[1]);
        custemerFlow = std::stoi(argv[2]);
        itemTime = std::stoi(argv[3]);;
        averageCart = std::stoi(argv[4]);
        lineSize = std::stoi(argv[5]);
        totalCustemNum = std::stoi(argv[6]);
    }
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(averageCart, 7.0);
    shop mall(lineSize, checkoutNum, itemTime);
    int sleepTime = 100 / custemerFlow;
    mall.startWork();
    for (size_t i = 0; i < totalCustemNum; ++i) {
        std::cout << "sending " << i << " custemer\n";
        std::shared_ptr<customer> cust = std::make_shared<customer>();
        cust->itemsNum = abs(distribution(generator));
        mall.addToLine(cust);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
    mall.stopWork();
    std::cout << "stats:\n";
    auto sts = mall.getStats();
    for (auto el : sts) {
        std::cout << el.first << " : " << el.second << "\n";
    }
    std::cout << "\n--------calculated parameters--------\n";
    float chanceOfRej = sts["rejected custemers"] / totalCustemNum;
    float throughput = sts["served custemers"] / totalCustemNum;
    float abcThroughput = sts["served custemers"] / sts["total shop work time"];
    std::cout << "chance of rejection : " << chanceOfRej << "\n";
    std::cout << "throughput : " << throughput << "\n";
    std::cout << "absolute throughput in 1 mls: " << abcThroughput << "\n";
}
