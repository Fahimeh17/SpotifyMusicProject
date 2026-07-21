#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// كلاس پايه استثناهاي برنامه. تمام استثناهاي پروژه از اين كلاس ارث‌بري مي‌كنند
// تا در سطح بالاتر (مثلا لايه UI) بشود همه‌ي آنها را با يك catch گرفت.
class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string &message) : std::runtime_error(message) {}
};

// وقتي نام كاربري در سيستم پيدا نشود (مثلا هنگام لاگين)
class UserNotFoundException : public AppException {
public:
    explicit UserNotFoundException(const std::string &userName)
        : AppException("کاربری با نام کاربری «" + userName + "» یافت نشد.") {}
};

// وقتي رمز عبور وارد شده اشتباه باشد
class InvalidPasswordException : public AppException {
public:
    InvalidPasswordException() : AppException("رمز عبور اشتباه است.") {}
};

// وقتي در زمان ثبت‌نام، نام كاربري از قبل وجود داشته باشد
class DuplicateUserNameException : public AppException {
public:
    explicit DuplicateUserNameException(const std::string &userName)
        : AppException("نام کاربری «" + userName + "» قبلاً استفاده شده است.") {}
};

// وقتي موجوديتي (آهنگ، آلبوم، ليست پخش، حساب) با شناسه‌ي داده شده پيدا نشود
class EntityNotFoundException : public AppException {
public:
    explicit EntityNotFoundException(const std::string &entityName, int id)
        : AppException(entityName + " با شناسه " + std::to_string(id) + " یافت نشد.") {}
};

// وقتي مقدار وروديِ داده شده (مثلا شناسه‌ي هنرمند) نامعتبر يا مجاز نباشد
class InvalidArgumentException : public AppException {
public:
    explicit InvalidArgumentException(const std::string &message) : AppException(message) {}
};

#endif // EXCEPTIONS_H
