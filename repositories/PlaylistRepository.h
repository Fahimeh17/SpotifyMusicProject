#ifndef PLAYLISTREPOSITORY_H
#define PLAYLISTREPOSITORY_H

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "AbstractRepository.h"
#include "../entities/Playlist.h"

class PlaylistRepository : public AbstractRepository<Playlist> {
private:
    std::string filePath;
    std::string linkFilePath; // فايل مشترك (playlistId,songId) - همان فايلي كه SongRepository مي‌خواند
    std::vector<Playlist> playlists_;
    int nextId;

    void load() {
        playlists_.clear();
        nextId = 1;
        std::ifstream in(filePath);
        if (!in.is_open()) return;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string idStr, name, listenerIdStr;
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, listenerIdStr, '|');
            Playlist p(std::stoi(idStr), name, std::stoi(listenerIdStr));
            playlists_.push_back(p);
            if (p.getId() >= nextId) nextId = p.getId() + 1;
        }
    }

    void persist() {
        std::ofstream out(filePath, std::ios::trunc);
        for (auto &p : playlists_)
            out << p.getId() << '|' << p.getName() << '|' << p.getListenerId() << '\n';
    }

    std::vector<std::pair<int, int>> loadLinks() {
        std::vector<std::pair<int, int>> links;
        std::ifstream in(linkFilePath);
        if (!in.is_open()) return links;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string pIdStr, sIdStr;
            std::getline(ss, pIdStr, ',');
            std::getline(ss, sIdStr, ',');
            links.push_back({std::stoi(pIdStr), std::stoi(sIdStr)});
        }
        return links;
    }

    void persistLinks(const std::vector<std::pair<int, int>> &links) {
        std::ofstream out(linkFilePath, std::ios::trunc);
        for (auto &link : links)
            out << link.first << ',' << link.second << '\n';
    }

public:
    explicit PlaylistRepository(const std::string &path = "data/playlists.txt",
                                 const std::string &linkPath = "data/playlist_songs.csv")
        : filePath(path), linkFilePath(linkPath), nextId(1) {
        load();
    }

    int save(Playlist &entity) override {
        auto it = std::find_if(playlists_.begin(), playlists_.end(),
                                [&](const Playlist &p) { return p.getId() == entity.getId() && entity.getId() != 0; });
        if (it != playlists_.end()) {
            *it = entity;
            persist();
            return entity.getId();
        }
        entity.setId(nextId++);
        playlists_.push_back(entity);
        persist();
        return entity.getId();
    }

    bool remove(int id) override {
        auto it = std::find_if(playlists_.begin(), playlists_.end(),
                                [&](const Playlist &p) { return p.getId() == id; });
        if (it == playlists_.end()) return false;
        playlists_.erase(it);
        persist();

        // حذف كامل يك ليست پخش يعني حذف ارتباط آن با آهنگ‌ها هم بايد پاك شود
        // (طبق سند: حذف ليست پخش، آهنگ را از كل سيستم حذف نمي‌كند؛ فقط ارتباط
        // اين ليست خاص با آهنگ‌ها از بين مي‌رود).
        auto links = loadLinks();
        links.erase(std::remove_if(links.begin(), links.end(),
                                    [&](const std::pair<int, int> &l) { return l.first == id; }),
                    links.end());
        persistLinks(links);
        return true;
    }

    std::optional<Playlist> search(int id) override {
        auto it = std::find_if(playlists_.begin(), playlists_.end(),
                                [&](const Playlist &p) { return p.getId() == id; });
        if (it == playlists_.end()) return std::nullopt;
        return *it;
    }

    // ---- توابع اختصاصي PlaylistRepository ----

    // درج يك آهنگ در يك ليست پخش (هردو بايد از قبل وجود داشته باشند)
    bool insertSong(int playlistId, int songId) {
        auto links = loadLinks();
        for (auto &l : links)
            if (l.first == playlistId && l.second == songId)
                return true; // از قبل موجود است
        links.push_back({playlistId, songId});
        persistLinks(links);
        return true;
    }

    // حذف يك آهنگ از يك ليست پخش
    bool removeSong(int playlistId, int songId) {
        auto links = loadLinks();
        auto before = links.size();
        links.erase(std::remove_if(links.begin(), links.end(),
                                    [&](const std::pair<int, int> &l) {
                                        return l.first == playlistId && l.second == songId;
                                    }),
                    links.end());
        persistLinks(links);
        return links.size() != before;
    }

    // تمام ليست‌هاي پخش يك شنونده
    std::vector<Playlist> playlists(int listenerId) {
        std::vector<Playlist> result;
        for (auto &p : playlists_)
            if (p.getListenerId() == listenerId)
                result.push_back(p);
        return result;
    }
};

#endif // PLAYLISTREPOSITORY_H
