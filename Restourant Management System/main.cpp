#include "singleton.hpp"
#include "Entity.hpp"
#include <iostream>

// g++ -std=c++20 -o prog -Wall -Wextra -Werror Entity.cpp main.cpp singleton.cpp

QueueManager QueueManager::instance;
TableManager TableManager::instance;
chefManager chefManager::instance;

#define WAITER_COUNT 3
#define CHEF_COUNT 2

// struct GENERAL{
//     std::vector<Waiter> waiterVec;
//     std::vector<Chef*> chefVec;
//     Kasa kasiyer = Kasa(0, 1);

//     void init(){
//         for (int l = 0; l < WAITER_COUNT; l++) {
//             waiterVec.push_back(Waiter(l, 1));
//         }
//         for (int i = 0; i < CHEF_COUNT; i++){
//             chefVec.push_back(new Chef(i, 1));
//         }
//     }

//     void allJoin(){
//         for (size_t i = 0; i < waiterVec.size(); i++) {
//             waiterVec.at(i).EndOfTheWorld();
//             waiterVec.at(i).join();
//         }
//         std::cout << "waiterlar cikmiyor" << std::endl;
//         for (size_t i = 0; i < chefVec.size(); i++) {
//             chefVec.at(i)->EndOfTheWorld();
//             chefVec.at(i)->join();
//             delete chefVec.at(i);
//         }
//         std::cout << "sefelr cikmiyor" << std::endl;
//         kasiyer.EndOfTheWorld();
//         kasiyer.join();
//         std::cout << "kasiyer cikmiyor" << std::endl;
//     }
// };


int main(){
    std::vector<Waiter> waiterVec;
    std::vector<Chef*> chefVec;
    Kasa kasiyer = Kasa(0, 1);

    for (int l = 0; l < WAITER_COUNT; l++) {
        waiterVec.push_back(Waiter(l, 1));
    }
    for (int x = 0; x < CHEF_COUNT; x++){
        chefVec.push_back(new Chef(x, 1));
    }
    std::string temp;
    int num = 0;
    int numPriority = 0;
    int i = 0;

    // genel.init();
    while(true){
        getline(std::cin, temp);
        if(!temp.compare("add")){
            std::cout << "oncelikli : "; std::cin >> numPriority;
            std::cout << "normal : "; std::cin >> num;
            for (int a = 0; a < num + numPriority; a++){
                if(a < numPriority)
                    QueueManager::instance.enter(new Customer(i, 1), 1);
                else{
                    QueueManager::instance.enter(new Customer(i, 0), 0);
                }
                i++;
            }
        }
        else if(!temp.compare("exit")){
            for (Customer *tmp : QueueManager::instance.customers){
                tmp->join();
                delete tmp;
                // QueueManager::instance.customers[l]->join();
                // delete QueueManager::instance.customers[l];
            }
            break;
        }
    }
        for (size_t i = 0; i < waiterVec.size(); i++) {
            waiterVec.at(i).EndOfTheWorld();
            waiterVec.at(i).join();
        }
        std::cout << "waiterlar cikmiyor" << std::endl;
        for (size_t i = 0; i < chefVec.size(); i++) {
            chefVec.at(i)->EndOfTheWorld();
            chefVec.at(i)->join();
            // delete chefVec.at(i);
        }
        std::cout << "sefelr cikmiyor" << std::endl;
        kasiyer.EndOfTheWorld();
        kasiyer.join();
        std::cout << "kasiyer cikmiyor" << std::endl;
    // genel.allJoin();
    return (0);
}