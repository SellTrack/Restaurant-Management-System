// g++ -Wall -Werror -Wextra main.cpp -std=c++20 -o yazlab
#include <iostream>
#include <thread>
#include <semaphore>
#include <unistd.h>
#include "Waiter.hpp"

std::mutex m;
std::mutex mPrint;
std::binary_semaphore semTable{6};
std::vector<Waiter> waiters(3);

int findAvailableWaiter(){
    m.lock();
    for (size_t i = 0; i < waiters.size(); i++)
    {
        if(waiters[i].isBusy() == false)
        {
            waiters[i].busy = true;
            m.unlock();
            return i;
        }
    }
    m.unlock();
    return (-1);
}

// namespace std{
//     class myThread : public std::thread{
//         public:
//         myThread(){
//             std::thread();
//         }
//     };
// }

void getMealOrder(Waiter &tmpWaiter){
    sleep(1);
    tmpWaiter.busy = false;
}


void acceptCustomer(){
    int num = 0;
    TRY:
    if(semTable.try_acquire()){
        num = findAvailableWaiter();
        if(num == -1){
            mPrint.lock();        
            std::cout << "there are no available waiters please wait: " << std::this_thread::get_id() << std::endl;
            mPrint.unlock();
            sleep(4);
            goto TRY;
        }
        else{
            mPrint.lock();        
            std::cout << "Waiter " << num << "'s dealing with the customer: "<< std::this_thread::get_id()<< std::endl;
            getMealOrder(waiters[num]);
            // sleep(2); // time for eating
            semTable.release();
            mPrint.unlock();        
        }
    }
    else{
        mPrint.lock();        
        std::cout << "there are no available table to sit please wait: " << std::endl;
        mPrint.unlock();
        sleep(4);
        goto TRY;
    }
}

int main(){
    for (int i = 0; i < 3; i++){
        waiters[i] = Waiter();
    }
    std::vector<std::thread> vec;
    std::string temp;
    int num = 0;
    while(1){
        std::getline(std::cin, temp);
        if(!temp.compare("add")){
            std::cout << "how many customer : ";
            std::cin >> num;
            for(int i = 0; i < num; i++){
                vec.push_back(std::thread(acceptCustomer));
            }
        }
        else if(!temp.compare("exit")){
            std::cout << "exiting" << std::endl;
            break;
        }
    }
    for(size_t i = 0; i < vec.size(); i++){
        vec[i].join();
    }
    return(0);
}