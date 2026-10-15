#include "NotificationManager.h"
#include "GameObject.h"
#include <algorithm>

namespace CMPUT350 {

void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string& key) { 
    std::vector<std::weak_ptr<GameObject>>& list = mListeners[key];

    auto sp2 = object.lock();
    if (sp2 == nullptr){
        return;
    }
    for(size_t i = 0; i < list.size(); i++){
        auto sp1 = list[i].lock();
        
        if (sp1 == sp2){
            return;
        }
    }

    list.push_back(object);
    return;
 }


void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string& key) {
    auto it = mListeners.find(key);
    if (it == mListeners.end()) {
        return;                 
    }
    auto& list = it->second;   

    auto sp2 = object.lock(); 
    list.erase(std::remove_if(list.begin(), list.end(),
                          [&](const std::weak_ptr<GameObject>& entry) {
                            auto sp = entry.lock();
                            return sp == sp2 || sp == nullptr;
                          }),
           list.end());

    return;
 }

void NotificationManager::Notify(const std::string& message) { 
    auto it = mListeners.find(message);
    if (it == mListeners.end()) {
        return;                 
    }
    auto list = it->second;  


    for(size_t i = 0; i < list.size(); i++){
        auto sp1 = list[i].lock();
        if (sp1 != nullptr){
            sp1->ReceiveNotification(message);
        }
        

    }

    auto again = mListeners.find(message);
    if (again != mListeners.end()) {
        auto& live = again->second;
        live.erase(std::remove_if(live.begin(), live.end(),
                                [](const std::weak_ptr<GameObject>& e) {
                                    auto sp = e.lock();
                                    return sp == nullptr;
                                }),
                live.end());
    }

    return; 
    }

}  // namespace CMPUT350

