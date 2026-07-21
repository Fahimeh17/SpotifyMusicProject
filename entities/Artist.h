#ifndef ARTIST_H
#define ARTIST_H

#include "Account.h"

// موجوديت هنرمند. تمام ويژگي‌هاي حساب كاربري را از Account به ارث مي‌برد
// و فقط نقش خودش (Role::Artist) را مشخص مي‌كند (چندريختي).
class Artist : public Account {
public:
    Artist() : Account() {}

    Artist(int id, const std::string &fullName, const std::string &userName,
           const std::string &biography, const std::string &password)
        : Account(id, fullName, userName, biography, password) {}

    Role getRole() const override { return Role::Artist; }
};

#endif // ARTIST_H
