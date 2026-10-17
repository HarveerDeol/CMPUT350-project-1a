#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CMPUT350 {

class GameObject; 

class NotificationManager {
public:
    /// @brief Registers an object to receive a notification. @param object The listener (std::weak_ptr<GameObject>). @param key The notification name (const std::string&).
    void Register(std::weak_ptr<GameObject> object, const std::string& key);
    /// @brief Removes an object from a notification's listeners. @param object The listener (std::weak_ptr<GameObject>). @param key The notification name (const std::string&).
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);
    /// @brief Sends a notification to its registered listeners. @param message The notification name (const std::string&).
    void Notify(const std::string& message);
private:
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
};

}

#endif