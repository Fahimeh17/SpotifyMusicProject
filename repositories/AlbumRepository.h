#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "AbstractRepository.h"
#include "../entities/Album.h"

class AlbumRepository : public AbstractRepository<Album> {
private:
    std::string filePath;
    std::vector<Album> albums_;
    int nextId;

    void load() {
        albums_.clear();
        nextId = 1;
        std::ifstream in(filePath);
        if (!in.is_open()) return;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string idStr, name, artistIdStr;
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, artistIdStr, '|');
            Album a(std::stoi(idStr), name, std::stoi(artistIdStr));
            albums_.push_back(a);
            if (a.getId() >= nextId) nextId = a.getId() + 1;
        }
    }

    void persist() {
        std::ofstream out(filePath, std::ios::trunc);
        for (auto &a : albums_)
            out << a.getId() << '|' << a.getName() << '|' << a.getArtistId() << '\n';
    }

public:
    explicit AlbumRepository(const std::string &path = "data/albums.txt") : filePath(path), nextId(1) {
        load();
    }

    int save(Album &entity) override {
        auto it = std::find_if(albums_.begin(), albums_.end(),
                                [&](const Album &a) { return a.getId() == entity.getId() && entity.getId() != 0; });
        if (it != albums_.end()) {
            *it = entity;
            persist();
            return entity.getId();
        }
        entity.setId(nextId++);
        albums_.push_back(entity);
        persist();
        return entity.getId();
    }

    bool remove(int id) override {
        auto it = std::find_if(albums_.begin(), albums_.end(),
                                [&](const Album &a) { return a.getId() == id; });
        if (it == albums_.end()) return false;
        albums_.erase(it);
        persist();
        return true;
    }

    std::optional<Album> search(int id) override {
        auto it = std::find_if(albums_.begin(), albums_.end(),
                                [&](const Album &a) { return a.getId() == id; });
        if (it == albums_.end()) return std::nullopt;
        return *it;
    }

    // تمام آلبوم‌هاي يك هنرمند
    std::vector<Album> albums(int artistId) {
        std::vector<Album> result;
        for (auto &a : albums_)
            if (a.getArtistId() == artistId)
                result.push_back(a);
        return result;
    }
};

#endif // ALBUMREPOSITORY_H
