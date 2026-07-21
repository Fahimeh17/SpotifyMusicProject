#ifndef SORTFILTERUTILS_H
#define SORTFILTERUTILS_H

#include <vector>
#include <string>
#include <algorithm>
#include "../entities/Song.h"
#include "../entities/Album.h"
#include "../entities/Playlist.h"

// توجه: تمام توابع اين فايل روي يك كپي از ليست ورودي كار مي‌كنند و هيچ‌كدام
// داده‌ي ذخيره‌شده (فايل‌ها) را تغيير نمي‌دهند؛ فقط ترتيب/زيرمجموعه‌ي نمايشي را مي‌سازند.
namespace SortFilterUtils {

inline std::vector<Song> sortSongsByName(std::vector<Song> songs) {
    std::sort(songs.begin(), songs.end(),
              [](const Song &a, const Song &b) { return a.getName() < b.getName(); });
    return songs;
}

inline std::vector<Song> sortSongsByYear(std::vector<Song> songs) {
    std::sort(songs.begin(), songs.end(),
              [](const Song &a, const Song &b) { return a.getReleaseYear() < b.getReleaseYear(); });
    return songs;
}

inline std::vector<Song> searchSongsByName(std::vector<Song> songs, const std::string &keyword) {
    std::vector<Song> result;
    for (auto &s : songs)
        if (s.getName().find(keyword) != std::string::npos)
            result.push_back(s);
    return result;
}

inline std::vector<Song> filterSongsByGenre(std::vector<Song> songs, const std::string &genre) {
    std::vector<Song> result;
    for (auto &s : songs)
        if (s.getGenre() == genre)
            result.push_back(s);
    return result;
}

inline std::vector<Song> filterSongsByYear(std::vector<Song> songs, int year) {
    std::vector<Song> result;
    for (auto &s : songs)
        if (s.getReleaseYear() == year)
            result.push_back(s);
    return result;
}

// آلبوم‌ها و ليست‌پخش‌ها هميشه صعودي براساس نام مرتب نمايش داده مي‌شوند (طبق سند)
inline std::vector<Album> sortAlbumsByName(std::vector<Album> albums) {
    std::sort(albums.begin(), albums.end(),
              [](const Album &a, const Album &b) { return a.getName() < b.getName(); });
    return albums;
}

inline std::vector<Playlist> sortPlaylistsByName(std::vector<Playlist> playlists) {
    std::sort(playlists.begin(), playlists.end(),
              [](const Playlist &a, const Playlist &b) { return a.getName() < b.getName(); });
    return playlists;
}

} // namespace SortFilterUtils

#endif // SORTFILTERUTILS_H
