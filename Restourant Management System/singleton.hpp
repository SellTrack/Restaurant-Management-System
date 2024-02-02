#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <iostream>
class Table;
class Customer;

//colorful text
//          foreground background
// black        30         40
// red          31         41
// green        32         42
// yellow       33         43
// blue         34         44
// magenta      35         45
// cyan         36         46
// white        37         47

// std::cout << "\033[1;31mbold red text\033[0m\n";

// reset             0  (everything back to normal)
// bold/bright       1  (often a brighter shade of the same colour)
// underline         4
// inverse           7  (swap foreground and background colours)
// bold/bright off  21
// underline off    24
// inverse off      27

// customer islemleri beyaz arka plan -> 47
// waiter islemleri siyah arka plan -> 40
// chef islemleri mavi arka plan -> 40
// kasiyer islemleri kirmizi arka plan -> 40


// musteri gelince queue manager a ekle
// table manager kontrol ediyor queue dan olup oturtturacak
// table manager oturan musteri dondurecek
        


class TableManager{
public:
    TableManager();
    int sit(Customer* tmp);

    // std::binary_semaphore sem{6};
    int i;
    std::mutex m;
    static TableManager instance;
    std::vector<Table> tableVec;
private:
};

 
class QueueManager{
public:
    Customer* fetchCustomer();
    Customer* fetchSitCustomer();
    Customer* fetchLeavingCustomer();
    void enter(Customer* tmp, int priority);
    void IWannaPay(Customer* tmp);
    
    static QueueManager instance;
    std::condition_variable sit;
    std::vector<Customer*> customers;
    std::vector<Customer*> sitCustomers;
    std::vector<Customer*> leavingCustomers;
    std::mutex m;
    std::mutex lock;
private:
};

// 
 

class chefManager{
public:
    static chefManager instance;
    std::vector<Customer*> orderedMeals;

    void giveOrder(Customer *p){
        // state degistirmek icin fonksiyon
        std::unique_lock<std::mutex> kitledim(sel);    
        orderedMeals.push_back(p);
    }
    Customer* getOrder(){
        // yemegi siparis listesinden silmek icin
        std::unique_lock<std::mutex> kitledim(sel);
        Customer *tmp = *orderedMeals.begin();
        orderedMeals.erase(orderedMeals.begin());
        return tmp;
    }
private:
    std::mutex sel;
};

#endif