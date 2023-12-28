#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <semaphore>
#include <fstream>

#include "singleton.hpp"



enum waiterStates{
    BUSY,
    AVAILABLE
};

enum cashierState{
    READY,
    ERROR,
    GETTING_PAY
};

enum tableStates{
    EMPTY,
    FULL
};
enum chefStates{
    COOKING,
    CAVAILABLE
};

enum customerStates{
    WAITING,
    SIT,
    ORDERED,
    COOKED,
    READYTOPAY,
    QUIT,
    LEAVE
};


class Entity{
public:
    Entity();
    Entity(const Entity& entityCopy);

    void printWithLock_waiter(const int &waiterId, const int &cID, const int &tableNum, waiterStates s);
    void printWithLock_customer(const int &cID, const int &tableNum, customerStates st, int p);
    void printWithLock_chef(const int &chefId, const int &cID, const int &tableNum, chefStates s);
    void printWithLock_cashier(const int &kasaId, const int &cID, const int &tableNum, cashierState s);

    virtual ~Entity(){};
    std::ofstream outputFile;
    int _check;
    void EndOfTheWorld(){
        _check = 0;
    }
private:
};

class Table{
public:
    tableStates isEmpty;
    Table(int i);
private:
    int tableNo;
};


class Customer : public Entity{
public:
    Customer(int i, int p);
    void customerLoop();
    void join();

    int sit();
    void takeOrder();
    void getYourMealAndEat();
    void Pay();
    void leave();

    //setter and getter
    void setTableId(int t);
    void setCustomerId(std::thread::id tID);
    void setCurrentState(customerStates c);
    customerStates getCurrentState(void);
    int getTableId();
    int getCustomerId();

private:
    std::thread customerThread;
    std::thread::id customerId;
    std::mutex kilit;
    customerStates state;
    int id;
    int priority;
    int TableId;
};




class Waiter : public Entity{
public:
    Waiter(int i, int check);
    void waiterLoop(int waiterId);
    void join(){waiterThread.detach();};

private:
    int waiterId;
    waiterStates isBusy;
    std::thread waiterThread;
};



class Chef : public Entity{
public:
    // constructor
    Chef(int id, int check);
    Chef(const Chef &other);

    // member functions
    void cookMeal();
    void join();
    ~Chef(){
        delete this;
    }
private:
    std::thread cooker1;
    std::thread cooker2;
    int chefId;
};


class Kasa: public Entity{
public:
    Kasa(int num, int check);
    void CashierLoop(int cashierID);
    void join(){kasaThread.detach();};

private:
    std::thread kasaThread;
    std::mutex kasakiliti;
    int cashierID;
};


#endif