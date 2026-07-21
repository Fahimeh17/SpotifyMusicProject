#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

#include <optional>
#include <string>
#include "AbstractRepository.h"

// كلاس الگو كه از AbstractRepository ارث‌بري مي‌كند و تابع searchByUserName
// را (بدون پياده‌سازي) اضافه مي‌كند. ArtistRepository و ListenerRepository
// از اين كلاس ارث‌بري مي‌كنند و همه‌ي توابع از جمله همين تابع را پياده‌سازي مي‌كنند.
template <typename T>
class AccountRepository : public AbstractRepository<T> {
public:
    virtual std::optional<T> searchByUserName(const std::string &userName) = 0;
};

#endif // ACCOUNTREPOSITORY_H
