#ifndef ARTISTREPOSITORY_H
#define ARTISTREPOSITORY_H

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "AccountRepository.h"
#include "../entities/Artist.h"

class ArtistRepository : public AccountRepository<Artist> {
private:
    std::string filePath;
    std::vector<Artist> artists;
    int nextId;

    // فيلدهاي متني (fullName, userName, biography, password) ممكن است شامل
    // فاصله باشند اما نبايد شامل كاراكتر '|' باشند چون از آن به‌عنوان جداكننده استفاده مي‌شود.
    void load() {
        artists.clear();
        nextId = 1;
        std::ifstream in(filePath);
        if (!in.is_open()) return;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string idStr, fullName, userName, bio, password;
            std::getline(ss, idStr, '|');
            std::getline(ss, fullName, '|');
            std::getline(ss, userName, '|');
            std::getline(ss, bio, '|');
            std::getline(ss, password, '|');
            Artist a(std::stoi(idStr), fullName, userName, bio, password);
            artists.push_back(a);
            if (a.getId() >= nextId) nextId = a.getId() + 1;
        }
    }

    void persist() {
        std::ofstream out(filePath, std::ios::trunc);
        for (auto &a : artists)
            out << a.getId() << '|' << a.getFullName() << '|' << a.getUserName() << '|'
                << a.getBiography() << '|' << a.getPassword() << '\n';
    }

public:
    explicit ArtistRepository(const std::string &path = "data/artists.txt") : filePath(path), nextId(1) {
        load();
    }

    int save(Artist &entity) override {
        auto it = std::find_if(artists.begin(), artists.end(),
                                [&](const Artist &a) { return a.getId() == entity.getId() && entity.getId() != 0; });
        if (it != artists.end()) {
            *it = entity;
            persist();
            return entity.getId();
        }
        entity.setId(nextId++);
        artists.push_back(entity);
        persist();
        return entity.getId();
    }

    bool remove(int id) override {
        auto it = std::find_if(artists.begin(), artists.end(),
                                [&](const Artist &a) { return a.getId() == id; });
        if (it == artists.end()) return false;
        artists.erase(it);
        persist();
        return true;
    }

    std::optional<Artist> search(int id) override {
        auto it = std::find_if(artists.begin(), artists.end(),
                                [&](const Artist &a) { return a.getId() == id; });
        if (it == artists.end()) return std::nullopt;
        return *it;
    }

    std::optional<Artist> searchByUserName(const std::string &userName) override {
        auto it = std::find_if(artists.begin(), artists.end(),
                                [&](const Artist &a) { return a.getUserName() == userName; });
        if (it == artists.end()) return std::nullopt;
        return *it;
    }

    // فهرست تمام هنرمندان (لازم براي صفحه‌ي "مشاهده فهرستي از تمام هنرمندان" شنوندگان)
    std::vector<Artist> all() const { return artists; }
};

#endif // ARTISTREPOSITORY_H
