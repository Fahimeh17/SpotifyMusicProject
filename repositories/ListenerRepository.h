#ifndef LISTENERREPOSITORY_H
#define LISTENERREPOSITORY_H

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "AccountRepository.h"
#include "../entities/Listener.h"

class ListenerRepository : public AccountRepository<Listener> {
private:
    std::string filePath;
    std::string likedFilePath; // فايل مشترك (listenerId,songId) كه SongRepository هم مي‌خواند
    std::vector<Listener> listeners;
    int nextId;

    void load() {
        listeners.clear();
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
            Listener l(std::stoi(idStr), fullName, userName, bio, password);
            listeners.push_back(l);
            if (l.getId() >= nextId) nextId = l.getId() + 1;
        }
    }

    void persist() {
        std::ofstream out(filePath, std::ios::trunc);
        for (auto &l : listeners)
            out << l.getId() << '|' << l.getFullName() << '|' << l.getUserName() << '|'
                << l.getBiography() << '|' << l.getPassword() << '\n';
    }

    std::vector<std::pair<int, int>> loadLiked() {
        std::vector<std::pair<int, int>> liked;
        std::ifstream in(likedFilePath);
        if (!in.is_open()) return liked;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string lIdStr, sIdStr;
            std::getline(ss, lIdStr, ',');
            std::getline(ss, sIdStr, ',');
            liked.push_back({std::stoi(lIdStr), std::stoi(sIdStr)});
        }
        return liked;
    }

    void persistLiked(const std::vector<std::pair<int, int>> &liked) {
        std::ofstream out(likedFilePath, std::ios::trunc);
        for (auto &l : liked)
            out << l.first << ',' << l.second << '\n';
    }

public:
    explicit ListenerRepository(const std::string &path = "data/listeners.txt",
                                 const std::string &likedPath = "data/liked_songs.csv")
        : filePath(path), likedFilePath(likedPath), nextId(1) {
        load();
    }

    int save(Listener &entity) override {
        auto it = std::find_if(listeners.begin(), listeners.end(),
                                [&](const Listener &l) { return l.getId() == entity.getId() && entity.getId() != 0; });
        if (it != listeners.end()) {
            *it = entity;
            persist();
            return entity.getId();
        }
        entity.setId(nextId++);
        listeners.push_back(entity);
        persist();
        return entity.getId();
    }

    bool remove(int id) override {
        auto it = std::find_if(listeners.begin(), listeners.end(),
                                [&](const Listener &l) { return l.getId() == id; });
        if (it == listeners.end()) return false;
        listeners.erase(it);
        persist();

        // با حذف حساب شنونده، رد لايك‌هاي او هم پاك مي‌شود
        auto liked = loadLiked();
        liked.erase(std::remove_if(liked.begin(), liked.end(),
                                    [&](const std::pair<int, int> &p) { return p.first == id; }),
                    liked.end());
        persistLiked(liked);
        return true;
    }

    std::optional<Listener> search(int id) override {
        auto it = std::find_if(listeners.begin(), listeners.end(),
                                [&](const Listener &l) { return l.getId() == id; });
        if (it == listeners.end()) return std::nullopt;
        return *it;
    }

    std::optional<Listener> searchByUserName(const std::string &userName) override {
        auto it = std::find_if(listeners.begin(), listeners.end(),
                                [&](const Listener &l) { return l.getUserName() == userName; });
        if (it == listeners.end()) return std::nullopt;
        return *it;
    }

    // ---- توابع اختصاصي ListenerRepository ----

    // به‌روزرساني وضعيت لايك يك آهنگ توسط يك شنونده
    void updateLiked(int listenerId, int songId, bool liked) {
        auto all = loadLiked();
        bool exists = std::any_of(all.begin(), all.end(), [&](const std::pair<int, int> &p) {
            return p.first == listenerId && p.second == songId;
        });
        if (liked && !exists) {
            all.push_back({listenerId, songId});
            persistLiked(all);
        } else if (!liked && exists) {
            all.erase(std::remove_if(all.begin(), all.end(),
                                      [&](const std::pair<int, int> &p) {
                                          return p.first == listenerId && p.second == songId;
                                      }),
                      all.end());
            persistLiked(all);
        }
    }

    bool isLiked(int listenerId, int songId) {
        auto all = loadLiked();
        return std::any_of(all.begin(), all.end(), [&](const std::pair<int, int> &p) {
            return p.first == listenerId && p.second == songId;
        });
    }
};

#endif // LISTENERREPOSITORY_H
