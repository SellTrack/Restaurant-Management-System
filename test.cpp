#include <vector>
#include <iostream>

int main(){
    int man = 0;
    int i = 0;
    int man_pri = 0;
    std::string str = "start";
    std::vector<std::string> myVec;
    
    while(str.compare("exit") != 0){
        getline(std::cin, str);
        if(!str.compare("add")){
            std::cout << "oncelikli : "; std::cin >> man_pri;
            std::cout << "normal : "; std::cin >> man;
            for (int a = 0; a < man + man_pri; a++){
                if(a < man_pri){
                    myVec.insert(myVec.begin(), "oncelikli : " + std::to_string(i++));
                }
                else{
                    myVec.push_back(std::to_string(i++));
                }
            }
            for (int i = 0; i < myVec.size(); i++)
            {
                std::cout << myVec[i] << "  -   ";
            }
            std::cout << std::endl;
        }
    }
    return 0;
}