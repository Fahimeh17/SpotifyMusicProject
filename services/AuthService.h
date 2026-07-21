#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <string>
#include "../entities/Artist.h"
#include "../entities/Listener.h"
#include "../repositories/ArtistRepository.h"
#include "../repositories/ListenerRepository.h"
#include "../exceptions/Exceptions.h"

// نتيجه‌ي يك لاگين موفق: نقش كاربر و شناسه‌ي حسابش (طبق نقش، UI صفحه‌ي مناسب را باز مي‌كند)
struct LoginResult {
    Role role;
    int accountId;
};

// اين سرويس مسئول قوانين ثبت‌نام/ورود/ويرايش/حذف حساب است (چيزهايي كه سند
// آنها را "قابليت‌هاي مشترك" و "روند اجراي برنامه" ناميده و نبايد در Repository باشند).
class AuthService {
private:
    ArtistRepository &artistRepo;
    ListenerRepository &listenerRepo;

    // نام كاربري بايد در كل سيستم (بين هنرمندان و شنوندگان) منحصربه‌فرد باشد
    bool userNameTaken(const std::string &userName) {
        return artistRepo.searchByUserName(userName).has_value() ||
               listenerRepo.searchByUserName(userName).has_value();
    }

public:
    AuthService(ArtistRepository &artistRepo, ListenerRepository &listenerRepo)
        : artistRepo(artistRepo), listenerRepo(listenerRepo) {}

    // پرتاب DuplicateUserNameException در صورت تكراري بودن نام كاربري
    Artist registerArtist(const std::string &fullName, const std::string &userName,
                           const std::string &password, const std::string &biography) {
        if (userNameTaken(userName)) throw DuplicateUserNameException(userName);
        Artist a(0, fullName, userName, biography, password);
        artistRepo.save(a);
        return a;
    }

    Listener registerListener(const std::string &fullName, const std::string &userName,
                               const std::string &password, const std::string &biography) {
        if (userNameTaken(userName)) throw DuplicateUserNameException(userName);
        Listener l(0, fullName, userName, biography, password);
        listenerRepo.save(l);
        return l;
    }

    // پرتاب UserNotFoundException اگر نام كاربري وجود نداشته باشد،
    // پرتاب InvalidPasswordException اگر رمز اشتباه باشد.
    LoginResult login(const std::string &userName, const std::string &password) {
        auto artist = artistRepo.searchByUserName(userName);
        if (artist.has_value()) {
            if (artist->getPassword() != password) throw InvalidPasswordException();
            return {Role::Artist, artist->getId()};
        }
        auto listener = listenerRepo.searchByUserName(userName);
        if (listener.has_value()) {
            if (listener->getPassword() != password) throw InvalidPasswordException();
            return {Role::Listener, listener->getId()};
        }
        throw UserNotFoundException(userName);
    }

    // ويرايش حساب كاربري (مشترك بين هنرمند و شنونده)
    void editAccount(Role role, int accountId, const std::string &newFullName,
                      const std::string &newUserName, const std::string &newPassword,
                      const std::string &newBiography) {
        if (role == Role::Artist) {
            auto current = artistRepo.search(accountId);
            if (!current.has_value()) throw EntityNotFoundException("حساب هنرمند", accountId);
            if (newUserName != current->getUserName() && userNameTaken(newUserName))
                throw DuplicateUserNameException(newUserName);
            Artist updated(accountId, newFullName, newUserName, newBiography, newPassword);
            artistRepo.save(updated);
        } else {
            auto current = listenerRepo.search(accountId);
            if (!current.has_value()) throw EntityNotFoundException("حساب شنونده", accountId);
            if (newUserName != current->getUserName() && userNameTaken(newUserName))
                throw DuplicateUserNameException(newUserName);
            Listener updated(accountId, newFullName, newUserName, newBiography, newPassword);
            listenerRepo.save(updated);
        }
    }
};

#endif // AUTHSERVICE_H
