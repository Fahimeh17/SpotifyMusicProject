#ifndef SONGREPOSITORY_H
#define SONGREPOSITORY_H

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "AbstractRepository.h"
#include "../entities/Song.h"

// اين كلاس نحوه ذخيره‌سازي و بازيابي آهنگ‌ها را (روي فايل) پياده‌سازي مي‌كند.
// نكته: اين كلاس فقط با موجوديت Song و شناسه‌ها سروكار دارد؛ هيچ منطق تجاري‌اي
// (مثلا اينكه حذف آلبوم باعث حذف آهنگ‌ها شود) اينجا پياده‌سازي نمي‌شود.
class SongRepository : public AbstractRepository<Song> {
private:
    std::string filePath;
    // فايل مشترك ارتباط ليست‌پخش <-> آهنگ (نوشته‌شده توسط PlaylistRepository)
    std::string playlistLinkFilePath;
    // فايل مشترك آهنگ‌هاي لايك‌شده (نوشته‌شده توسط ListenerRepository)
    std::string likedSongsFilePath;
    std::vector<Song> songs;
    int nextId;

    void load() {
        songs.clear();
        nextId = 1;
        std::ifstream in(filePath);
        if (!in.is_open()) return;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string idStr, name, yearStr, genre, audio, artistIdStr, albumIdStr;
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, yearStr, '|');
            std::getline(ss, genre, '|');
            std::getline(ss, audio, '|');
            std::getline(ss, artistIdStr, '|');
            std::getline(ss, albumIdStr, '|');

            Song s(std::stoi(idStr), name, std::stoi(yearStr), genre, audio,
                    std::stoi(artistIdStr), std::stoi(albumIdStr));
            songs.push_back(s);
            if (s.getId() >= nextId) nextId = s.getId() + 1;
        }
    }

    void persist() {
        std::ofstream out(filePath, std::ios::trunc);
        for (auto &s : songs) {
            out << s.getId() << '|' << s.getName() << '|' << s.getReleaseYear() << '|'
                << s.getGenre() << '|' << s.getAudioFileName() << '|' << s.getArtistId()
                << '|' << s.getAlbumId() << '\n';
        }
    }

    // خواندن ارتباطات (playlistId,songId) از فايل مشترك با PlaylistRepository
    std::vector<int> readSongIdsForPlaylist(int playlistId) {
        std::vector<int> result;
        std::ifstream in(playlistLinkFilePath);
        if (!in.is_open()) return result;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string pIdStr, sIdStr;
            std::getline(ss, pIdStr, ',');
            std::getline(ss, sIdStr, ',');
            if (std::stoi(pIdStr) == playlistId) {
                result.push_back(std::stoi(sIdStr));
            }
        }
        return result;
    }

    // خواندن آهنگ‌هاي لايك‌شده‌ي يك شنونده از فايل مشترك با ListenerRepository
    std::vector<int> readLikedSongIds(int listenerId) {
        std::vector<int> result;
        std::ifstream in(likedSongsFilePath);
        if (!in.is_open()) return result;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string lIdStr, sIdStr;
            std::getline(ss, lIdStr, ',');
            std::getline(ss, sIdStr, ',');
            if (std::stoi(lIdStr) == listenerId) {
                result.push_back(std::stoi(sIdStr));
            }
        }
        return result;
    }

public:
    explicit SongRepository(const std::string &path = "data/songs.txt",
                             const std::string &playlistLinkPath = "data/playlist_songs.csv",
                             const std::string &likedSongsPath = "data/liked_songs.csv")
        : filePath(path), playlistLinkFilePath(playlistLinkPath),
          likedSongsFilePath(likedSongsPath), nextId(1) {
        load();
    }

    int save(Song &entity) override {
        auto it = std::find_if(songs.begin(), songs.end(),
                                [&](const Song &s) { return s.getId() == entity.getId() && entity.getId() != 0; });
        if (it != songs.end()) {
            *it = entity;
            persist();
            return entity.getId();
        }
        entity.setId(nextId++);
        songs.push_back(entity);
        persist();
        return entity.getId();
    }

    bool remove(int id) override {
        auto it = std::find_if(songs.begin(), songs.end(),
                                [&](const Song &s) { return s.getId() == id; });
        if (it == songs.end()) return false;
        songs.erase(it);
        persist();
        return true;
    }

    std::optional<Song> search(int id) override {
        auto it = std::find_if(songs.begin(), songs.end(),
                                [&](const Song &s) { return s.getId() == id; });
        if (it == songs.end()) return std::nullopt;
        return *it;
    }

    // ---- توابع اختصاصي SongRepository ----

    // آهنگ‌هاي يك هنرمند كه به هيچ آلبومي تعلق ندارند (آلبوم Singles)
    std::vector<Song> singleSongs(int artistId) {
        std::vector<Song> result;
        for (auto &s : songs)
            if (s.getArtistId() == artistId && s.getAlbumId() == 0)
                result.push_back(s);
        return result;
    }

    std::vector<Song> getByAlbum(int albumId) {
        std::vector<Song> result;
        for (auto &s : songs)
            if (s.getAlbumId() == albumId)
                result.push_back(s);
        return result;
    }

    std::vector<Song> getByArtist(int artistId) {
        std::vector<Song> result;
        for (auto &s : songs)
            if (s.getArtistId() == artistId)
                result.push_back(s);
        return result;
    }

    std::vector<Song> getByPlaylist(int playlistId) {
        std::vector<Song> result;
        for (int songId : readSongIdsForPlaylist(playlistId)) {
            auto found = search(songId);
            if (found.has_value())
                result.push_back(found.value());
        }
        return result;
    }

    std::vector<Song> getByLikedSongs(int listenerId) {
        std::vector<Song> result;
        for (int songId : readLikedSongIds(listenerId)) {
            auto found = search(songId);
            if (found.has_value())
                result.push_back(found.value());
        }
        return result;
    }
};

#endif // SONGREPOSITORY_H
