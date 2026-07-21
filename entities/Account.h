#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

// نقش كاربر: هنرمند يا شنونده
enum class Role {
    Artist,
    Listener
};

// كلاس پايه‌ي حساب كاربري (موجوديت Account در سند پروژه).
// اين كلاس به‌صورت انتزاعي طراحي شده (داراي تابع مجازي محض) تا نتوان
// مستقيما از آن شيء ساخت؛ كلاس‌هاي Artist و Listener از آن ارث‌بري مي‌كنند.
class Account {
protected:
    int id;                 // شناسه كاربر (منحصربه‌فرد)
    std::string fullName;   // نام اصلي
    std::string userName;   // نام كاربري (منحصربه‌فرد)
    std::string biography;  // شرح حال
    std::string password;   // رمز عبور
    std::string profilePhoto; // عكس پروفايل (امتيازي - فعلا خالي مي‌ماند)

public:
    Account()
        : id(0), fullName(""), userName(""), biography(""), password(""), profilePhoto("") {}

    Account(int id, const std::string &fullName, const std::string &userName,
            const std::string &biography, const std::string &password)
        : id(id), fullName(fullName), userName(userName), biography(biography),
          password(password), profilePhoto("") {}

    virtual ~Account() = default;

    // تابع مجازي محض: هر كلاس فرزند نقش خودش را برمي‌گرداند.
    // وجود همين تابع، Account را انتزاعي مي‌كند.
    virtual Role getRole() const = 0;

    // ---- getters ----
    int getId() const { return id; }
    std::string getFullName() const { return fullName; }
    std::string getUserName() const { return userName; }
    std::string getBiography() const { return biography; }
    std::string getPassword() const { return password; }
    std::string getProfilePhoto() const { return profilePhoto; }

    // ---- setters ----
    void setId(int newId) { id = newId; }
    void setFullName(const std::string &v) { fullName = v; }
    void setUserName(const std::string &v) { userName = v; }
    void setBiography(const std::string &v) { biography = v; }
    void setPassword(const std::string &v) { password = v; }
    void setProfilePhoto(const std::string &v) { profilePhoto = v; }
};

#endif // ACCOUNT_H
