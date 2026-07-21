#ifndef LISTENER_H
#define LISTENER_H

#include "Account.h"

// موجوديت شنونده. تمام ويژگي‌هاي حساب كاربري را از Account به ارث مي‌برد
// و فقط نقش خودش (Role::Listener) را مشخص مي‌كند (چندريختي).
class Listener : public Account {
public:
    Listener() : Account() {}

    Listener(int id, const std::string &fullName, const std::string &userName,
              const std::string &biography, const std::string &password)
        : Account(id, fullName, userName, biography, password) {}

    Role getRole() const override { return Role::Listener; }
};

#endif // LISTENER_H
