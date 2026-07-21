#ifndef LISTENERSERVICE_H
#define LISTENERSERVICE_H

#include <string>
#include <vector>
#include "../entities/Playlist.h"
#include "../entities/Song.h"
#include "../entities/Artist.h"
#include "../entities/Album.h"
#include "../repositories/ListenerRepository.h"
#include "../repositories/PlaylistRepository.h"
#include "../repositories/SongRepository.h"
#include "../repositories/ArtistRepository.h"
#include "../repositories/AlbumRepository.h"
#include "../exceptions/Exceptions.h"

// شناسه‌ي قراردادي پلی‌لیست مجازي و ثابت "Favorite Songs" (يك رديف واقعي در
// PlaylistRepository نيست؛ از طريق ListenerRepository::updateLiked/isLiked مديريت مي‌شود)
const std::string FAVORITE_SONGS_NAME = "Favorite Songs";

class ListenerService {
private:
    ListenerRepository &listenerRepo;
    PlaylistRepository &playlistRepo;
    SongRepository &songRepo;
    ArtistRepository &artistRepo;
    AlbumRepository &albumRepo;

    void ensureOwnership(int playlistId, int listenerId) {
        auto playlist = playlistRepo.search(playlistId);
        if (!playlist.has_value()) throw EntityNotFoundException("لیست پخش", playlistId);
        if (playlist->getListenerId() != listenerId)
            throw InvalidArgumentException("این لیست پخش متعلق به این شنونده نیست.");
    }

public:
    ListenerService(ListenerRepository &listenerRepo, PlaylistRepository &playlistRepo,
                     SongRepository &songRepo, ArtistRepository &artistRepo,
                     AlbumRepository &albumRepo)
        : listenerRepo(listenerRepo), playlistRepo(playlistRepo), songRepo(songRepo),
          artistRepo(artistRepo), albumRepo(albumRepo) {}

    // ---- ليست پخش ----
    int createPlaylist(int listenerId, const std::string &name) {
        if (!listenerRepo.search(listenerId).has_value())
            throw EntityNotFoundException("شنونده", listenerId);
        Playlist playlist(0, name, listenerId);
        return playlistRepo.save(playlist);
    }

    void editPlaylist(int playlistId, int listenerId, const std::string &newName) {
        ensureOwnership(playlistId, listenerId);
        Playlist updated(playlistId, newName, listenerId);
        playlistRepo.save(updated);
    }

    // حذف يك ليست‌پخش: فقط ارتباط آن با آهنگ‌ها پاك مي‌شود؛ خود آهنگ‌ها از سيستم حذف نمي‌شوند
    void deletePlaylist(int playlistId, int listenerId) {
        ensureOwnership(playlistId, listenerId);
        playlistRepo.remove(playlistId);
    }

    void addSongToPlaylist(int playlistId, int listenerId, int songId) {
        ensureOwnership(playlistId, listenerId);
        if (!songRepo.search(songId).has_value()) throw EntityNotFoundException("آهنگ", songId);
        playlistRepo.insertSong(playlistId, songId);
    }

    // حذف آهنگ از يك ليست‌پخش خاص - آهنگ از سيستم/آلبوم حذف نمي‌شود
    void removeSongFromPlaylist(int playlistId, int listenerId, int songId) {
        ensureOwnership(playlistId, listenerId);
        playlistRepo.removeSong(playlistId, songId);
    }

    std::vector<Playlist> getPlaylists(int listenerId) { return playlistRepo.playlists(listenerId); }
    std::vector<Song> getSongsOfPlaylist(int playlistId) { return songRepo.getByPlaylist(playlistId); }

    // ---- لايك (پلی‌لیست مجازي Favorite Songs) ----
    void likeSong(int listenerId, int songId) {
        if (!songRepo.search(songId).has_value()) throw EntityNotFoundException("آهنگ", songId);
        listenerRepo.updateLiked(listenerId, songId, true);
    }

    void unlikeSong(int listenerId, int songId) {
        listenerRepo.updateLiked(listenerId, songId, false);
    }

    bool isLiked(int listenerId, int songId) { return listenerRepo.isLiked(listenerId, songId); }

    std::vector<Song> getFavoriteSongs(int listenerId) { return songRepo.getByLikedSongs(listenerId); }

    // ---- مرور هنرمندان و آلبوم‌ها (فقط مشاهده) ----
    std::vector<Artist> getAllArtists() { return artistRepo.all(); }
    std::vector<Album> getArtistAlbums(int artistId) { return albumRepo.albums(artistId); }
    std::vector<Song> getArtistSingles(int artistId) { return songRepo.singleSongs(artistId); }

    // ---- حذف كامل حساب شنونده (سراسري) ----
    // فقط ليست‌پخش‌ها و رد لايك‌ها پاك مي‌شود؛ خود آهنگ‌ها/آلبوم‌ها دست‌نخورده باقي مي‌مانند.
    void deleteListenerAccount(int listenerId) {
        if (!listenerRepo.search(listenerId).has_value())
            throw EntityNotFoundException("حساب شنونده", listenerId);
        for (auto &playlist : playlistRepo.playlists(listenerId))
            playlistRepo.remove(playlist.getId());
        listenerRepo.remove(listenerId); // خودش رد لايك‌ها را هم پاك مي‌كند
    }
};

#endif // LISTENERSERVICE_H
