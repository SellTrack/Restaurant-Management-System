#include "singleton.hpp"
#include "Entity.hpp"

// Queue Manager
Customer* QueueManager::fetchCustomer(){
    m.lock(); // mutex kontrolu
    Customer *tmp = *customers.begin();
    customers.erase(customers.begin());
    m.unlock(); // mutex kontrolu
    return tmp;
}
Customer* QueueManager::fetchLeavingCustomer(){

    m.lock(); // mutex kontrolu
    Customer *tmp = *leavingCustomers.begin();
    leavingCustomers.erase(leavingCustomers.begin());
    m.unlock(); // mutex kontrolu
    return tmp;
}
Customer* QueueManager::fetchSitCustomer(){
    m.lock(); // mutex kontrolu
    Customer *tmp = *sitCustomers.begin();
    sitCustomers.erase(sitCustomers.begin());
    m.unlock(); // mutex kontrolu
    return tmp;
}
void QueueManager::enter(Customer* tmp, int priority){
    m.lock();
    if(std::find(customers.begin(), customers.end(), tmp) != customers.end()) // zaten var mi
    {
        m.unlock(); // mutex ac return den once
        return ; // already added
    }
    if(priority == 1){ // oncelikli ise ilk siraya ekle
        customers.insert(customers.begin(), tmp);
    }
    else if(priority == 0){ // degilse sona
        customers.push_back(tmp);
    }
    m.unlock(); // mutex kontrolu
}
void QueueManager::IWannaPay(Customer* tmp){
    m.lock();
    leavingCustomers.push_back(tmp);
    m.unlock();
}



//Table Manager
TableManager::TableManager(){
    for (int i = 0; i < 6; i++){
        tableVec.push_back(Table(i));
    }
    i = 6;
}

int TableManager::sit(Customer *tmp){
    m.lock();
    for (int i = 0; i < 6; i++){
        if(tableVec.at(i).isEmpty == EMPTY)
        {
            tableVec.at(i).isEmpty = FULL;
            tmp->setTableId(i);
            tmp->setCurrentState(SIT);
            QueueManager::instance.sitCustomers.push_back(tmp);
            m.unlock();
            return i;
        }
    }
    m.unlock();
    return -1;
}