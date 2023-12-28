#include "Entity.hpp"
#include <sstream>

std::vector<int> orderMeals;
std::mutex mu;
std::mutex muuuutex;
std::condition_variable waiterconv;
std::condition_variable cashier_conv;
std::condition_variable chefConv;
std::mutex m;
std::mutex lockl;
std::mutex printLock;
std::mutex mutexCook;
std::condition_variable cvCook;


// ----------- ENTITY ----------
Entity::Entity(){
    outputFile = std::ofstream("logs.json");
}
Entity::Entity(const Entity& entityCopy){
    (void)entityCopy;
}

//print with lock musteri mesajlarini guvenli sekilde yazdirmak icin
void Entity::printWithLock_customer(const int &cID, const int &tableNum, customerStates st, int p){
    printLock.lock();
    if(st == WAITING){std::cout << "\033[43;31m" << ((p==1)?"*oncelikli* ":"")<<"\033[0m\033[47;34mcustomer:"<< cID << " sanırım şu an yer yok? :(\033[0m" << std::endl;}
    else if(st == SIT){std::cout << "\033[43;31m" << ((p==1)?"*oncelikli* ":"")<<"\033[0m\033[47;32mcustomer:"<< cID << " masa:" << tableNum << "    'oturduk!'\033[0m" << std::endl;}
    else if(st == ORDERED){std::cout << "\033[43;31m" << ((p==1)?"*oncelikli* ":"")<<"\033[0m\033[1;35mcustomer:"<< cID << " masa:" << tableNum << "    :'yemegi soyledik!'\033[0m" << std::endl;}
    else if(st == COOKED){std::cout << "\033[43;31m" << ((p==1)?"*oncelikli* ":"")<< "\033[0m\033[1;35mcustomer:"<< cID << " masa:" << tableNum << "    :'yemek masaya geldi thx!'\033[0m" << std::endl;}
    else if(st == LEAVE){std::cout << "\033[43;31m" << ((p==1)?"*oncelikli* ":"")<< "\033[0m\033[1;15mcustomer:"<< cID << " masa:" << tableNum << "    :'odemeyi yaptik gidiyoruz!'\033[0m" << std::endl;}
    else if(st == QUIT){std::cout << "\033[43;31m" << ((p==1)?"*oncelikli* ":"")<< "\033[0m\033[47;31mcustomer:"<< cID << " çok zaman geçti çıkıyorum ben :(\033[0m" << std::endl;}
    printLock.unlock();
}
void Entity::printWithLock_waiter(const int &waiterId, const int &cID, const int &tableNum, waiterStates s){
    printLock.lock();
    if(s == AVAILABLE){std::cout << "\033[40;32mWaiter: " << waiterId << " is ready !\033[0m" << std::endl;}
    else if(s == BUSY){std::cout << "\033[40;32mWaiter: " << waiterId << " 's taking order of customer:" << cID << " masa:" << tableNum << "\033[0m" << std::endl;}
    printLock.unlock();
}

void Entity::printWithLock_chef(const int &chefId, const int &cID, const int &tableNum, chefStates s){
    printLock.lock();
    if (s == CAVAILABLE){std::cout << "\033[44;30mChef:"<< chefId << " 's ready to cook!\033[0m" << std::endl;}
    else if(s == COOKING){std::cout << "\033[44;30mChef:"<< chefId << " is cooking the meal of customer:"<< cID << " masa: " << tableNum<< "\033[0m" << std::endl;}
    printLock.unlock();
}
void Entity::printWithLock_cashier(const int &kasaId, const int &cID, const int &tableNum, cashierState s){
    printLock.lock();
    if (s == READY){std::cout << "Kasiyer:" << kasaId << "'s ready to get payments!"<<std::endl;}
    else if(s == GETTING_PAY){std::cout << "\033[40;31mKasiyer:"<< kasaId << " odemesi aliniyor customer:"<< cID << " masa: "<< tableNum << "\033[0m" << std::endl;}
    else if(s == ERROR){std::cout << "\033[40;31mKasiyer:"<< kasaId << " hata var !!!!! \033[0m" << std::endl;}
    printLock.unlock();
}


// ---------- TABLE ---------- 
Table::Table(int i) {
    isEmpty = EMPTY;
    tableNo = i + 1;
}



// ---------- CUSTOMER ----------
Customer::Customer(int i, int p){
    priority = p;
    TableId = -1;
    id = i + 1;
    customerThread = std::thread(&Customer::customerLoop, this);
    customerId  = customerThread.get_id();
}
int Customer::sit(){
    // oturma islemleri
    std::chrono::steady_clock sc;
    auto start = sc.now();

    TableManager::instance.sit(this);
    while(this->state == WAITING){
        TableManager::instance.sit(this);
        auto end = sc.now();       // end timer (starting & ending is done by measuring the time at the moment the process started & ended respectively)
        if(static_cast<std::chrono::duration<double>>(end - start).count() > 20){
            printWithLock_customer(id, 0, QUIT, priority);
            return -1;
        }
        printWithLock_customer(id, 0, WAITING, priority);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    printWithLock_customer(id, getTableId(), SIT,  priority);
    return 1;
}
void Customer::takeOrder(){
    while (state != ORDERED){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printWithLock_customer(id, getTableId(), ORDERED, priority);
}
void Customer::getYourMealAndEat(){
    while (state != COOKED){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printWithLock_customer(id, getTableId(), COOKED, priority);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
void Customer::Pay(){
    setCurrentState(READYTOPAY); // odemeye hazir hale geldigimizi durum guncellemesi yaparak sagliyoruz
    QueueManager::instance.IWannaPay(this);
    cashier_conv.notify_one();
}
void Customer::leave(){
    while(state != LEAVE){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    printWithLock_customer(id, getTableId(), LEAVE, priority);
}
void Customer::customerLoop() {
    int res = sit();

    if (res == -1)
    {
        return;
    }
    
    waiterconv.notify_one(); // garsona siparis verdigimizi bildiren kisim
    takeOrder(); //taking order
    getYourMealAndEat(); // yeme
    Pay(); //odeme
    leave(); //cikacak
}
void Customer::join(){
    customerThread.join();
}
void Customer::setTableId(int t){
    std::lock_guard<std::mutex> m(kilit);
    TableId = t;
    state = SIT;
}
void Customer::setCustomerId(std::thread::id tID){  // hic kullanilmayacak cunku constructor da atandi
    customerId = tID;
}
void Customer::setCurrentState(customerStates c){
    kilit.lock();
    state = c;
    kilit.unlock();
}
customerStates Customer::getCurrentState(){
    kilit.lock();
    return state;
    kilit.unlock();
}

int Customer::getTableId(){
    std::lock_guard<std::mutex> m(kilit);
    return TableId;
}
int Customer::getCustomerId(){
    std::lock_guard<std::mutex> m(kilit);
    return id;
}



//Waiter
Waiter::Waiter(int i, int check){
    _check = check;
    waiterId = i;
    isBusy = AVAILABLE;
    waiterThread = std::thread(&Waiter::waiterLoop, this, i + 1);
}
std::mutex kilit;
void Waiter::waiterLoop(int waiterId) {
    Customer *tmp;
    printWithLock_waiter(waiterId, 0, 0, AVAILABLE);
    while(_check){
        std::unique_lock<std::mutex> ml(kilit); // ayni anda musteri alma ihtimaline karsi kilitle
        waiterconv.wait(ml, [](){return QueueManager::instance.sitCustomers.size() > 0;});
        
        // queue manager dan musteri al
        tmp = QueueManager::instance.fetchSitCustomer();
        
        ml.unlock();
        printWithLock_waiter(waiterId, tmp->getCustomerId(), tmp->getTableId(), BUSY); // garson musteri ile ilgileniyor mesahi
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        tmp->setCurrentState(ORDERED);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // garsonun asciya siparis verdigi  
        chefManager::instance.giveOrder(tmp);
        cvCook.notify_one();
    }
}



//----- CHEF -----

Chef::Chef(int id, int check) : chefId(id) {
    _check = check;
    cooker1 = std::thread(&Chef::cookMeal, this);
    cooker2 = std::thread(&Chef::cookMeal, this);
}
Chef::Chef(const Chef &other){
    //copy constructor for chef class
    (void)other;
}
void Chef::join() {
    cooker1.detach();
    cooker2.detach();
}
void Chef::cookMeal() {
    // printWithLock_chef(chefId, 0, 0, CAVAILABLE); 
    while (_check)
    {
        std::unique_lock<std::mutex> lock(mutexCook);
        cvCook.wait(lock, [](){ return  chefManager::instance.orderedMeals.size() > 0;});
        
        Customer *meal = chefManager::instance.orderedMeals.back();
        chefManager::instance.orderedMeals.pop_back();
        lock.unlock(); // Releasing the lock while cooking to allow other threads to access the queue
        
        // Simulate cooking time 
        printWithLock_chef(chefId, meal->getCustomerId(), meal->getTableId(), COOKING);
        std::this_thread::sleep_for(std::chrono::seconds(4));
        printLock.lock();
        meal->setCurrentState(COOKED);
        printLock.unlock();
    }
    
}




// function for cashier class
Kasa::Kasa(int num, int check){
    _check = check;
    cashierID = num;
    kasaThread = std::thread(&Kasa::CashierLoop, this, num + 1);
}

void Kasa::CashierLoop(int cashierID){
    printWithLock_cashier(cashierID, 0, 0, READY);
    while(_check){
        std::unique_lock<std::mutex> kitledimm(kasakiliti);
        cashier_conv.wait(kitledimm, [](){return QueueManager::instance.leavingCustomers.size() > 0;});
        Customer *leaving = QueueManager::instance.fetchLeavingCustomer();
        if(leaving == nullptr)
        {
            printWithLock_cashier(cashierID, 0, 0, ERROR);
            continue;
        }
        printWithLock_cashier(cashierID, leaving->getCustomerId(), leaving->getTableId(), GETTING_PAY);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        leaving->setCurrentState(LEAVE);
        TableManager::instance.tableVec.at(leaving->getTableId()).isEmpty = EMPTY;
    }
}