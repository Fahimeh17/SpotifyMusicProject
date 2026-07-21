#ifndef ARTISTSERVICE_H
#define ARTISTSERVICE_H

#include <string>
#include <vector>
#include "../entities/Album.h"
#include "../entities/Song.h"
#include "../repositories/ArtistRepository.h"
#include "../repositories/AlbumRepository.h"
#include "../repositories/SongRepository.h"
#include "../repositories/PlaylistRepository.h"
#include "../exceptions/Exceptions.h"

// شناسه‌ي ثابت و قراردادي براي آلبوم مجازي "Singles" (يك آلبوم واقعي در Repository
// نيست؛ فقط براي نمايشِ آهنگ‌هاي بدون آلبوم در UI استفاده مي‌شود).
constexpr int SINGLES_ALBUM_ID = 0;
const std::string SINGLES_ALBUM_NAME = "Singles";

class ArtistService {
private:
    ArtistRepository &artistRepo;
    AlbumRepository &albumRepo;
    SongRepository &songRepo;
    PlaylistRepository &playlistRepo;

public:
    ArtistService(ArtistRepository &artistRepo, AlbumRepository &albumRepo,
                  SongRepository &songRepo, PlaylistRepository &playlistRepo)
        : artistRepo(artistRepo), albumRepo(albumRepo), songRepo(songRepo),
          playlistRepo(playlistRepo) {}

    // ---- آلبوم ----
    int createAlbum(int artistId, const std::string &name) {
        if (!artistRepo.search(artistId).has_value())
            throw EntityNotFoundException("هنرمند", artistId);
        Album album(0, name, artistId);
        return albumRepo.save(album);
    }

    void editAlbum(int albumId, const std::string &newName) {
        auto current = albumRepo.search(albumId);
        if (!current.has_value()) throw EntityNotFoundException("آلبوم", albumId);
        Album updated(albumId, newName, current->getArtistId());
        albumRepo.save(updated);
    }

    // حذف آلبوم: تمام آهنگ‌هاي داخل آن هم حذف مي‌شوند (و از هر ليست‌پخشي كه
    // در آن قرار داشتند هم پاك مي‌شوند)، سپس خود آلبوم حذف مي‌شود.
    void deleteAlbum(int albumId) {
        if (!albumRepo.search(albumId).has_value())
            throw EntityNotFoundException("آلبوم", albumId);
        for (auto &song : songRepo.getByAlbum(albumId)) {
            playlistRepo.removeSongFromAllPlaylists(song.getId());
            songRepo.remove(song.getId());
        }
        albumRepo.remove(albumId);
    }

    // فهرست آلبوم‌هاي يك هنرمند (بدون Singles - چون آلبوم واقعي نيست)
    std::vector<Album> getAlbums(int artistId) { return albumRepo.albums(artistId); }

    // ---- آهنگ ----
    // اگر albumId صفر باشد يعني آهنگ به هيچ آلبومي تعلق ندارد (Single)
    int createSong(int artistId, const std::string &name, int releaseYear,
                   const std::string &genre, const std::string &audioFileName, int albumId) {
        if (!artistRepo.search(artistId).has_value())
            throw EntityNotFoundException("هنرمند", artistId);
        if (albumId != 0) {
            auto album = albumRepo.search(albumId);
            if (!album.has_value() || album->getArtistId() != artistId)
                throw InvalidArgumentException("آلبوم انتخاب‌شده متعلق به این هنرمند نیست.");
        }
        Song song(0, name, releaseYear, genre, audioFileName, artistId, albumId);
        return songRepo.save(song);
    }

    void editSong(int songId, const std::string &newName, int newReleaseYear,
                  const std::string &newGenre, const std::string &newAudioFileName, int newAlbumId) {
        auto current = songRepo.search(songId);
        if (!current.has_value()) throw EntityNotFoundException("آهنگ", songId);
        if (newAlbumId != 0) {
            auto album = albumRepo.search(newAlbumId);
            if (!album.has_value() || album->getArtistId() != current->getArtistId())
                throw InvalidArgumentException("آلبوم انتخاب‌شده متعلق به این هنرمند نیست.");
        }
        Song updated(songId, newName, newReleaseYear, newGenre, newAudioFileName,
                     current->getArtistId(), newAlbumId);
        songRepo.save(updated);
    }

    // حذف آهنگ: از آلبوم هنرمند، از تمام ليست‌پخش‌ها، و از سيستم حذف مي‌شود
    void deleteSong(int songId) {
        if (!songRepo.search(songId).has_value())
            throw EntityNotFoundException("آهنگ", songId);
        playlistRepo.removeSongFromAllPlaylists(songId);
        songRepo.remove(songId);
    }

    // آهنگ‌هاي بدون آلبوم يك هنرمند (نمايش‌داده‌شده تحت آلبوم مجازي Singles)
    std::vector<Song> getSingles(int artistId) { return songRepo.singleSongs(artistId); }

    std::vector<Song> getSongsOfAlbum(int albumId) { return songRepo.getByAlbum(albumId); }

    // ---- حذف كامل حساب هنرمند (سراسري) ----
    void deleteArtistAccount(int artistId) {
        if (!artistRepo.search(artistId).has_value())
            throw EntityNotFoundException("حساب هنرمند", artistId);
        // حذف تمام آلبوم‌ها (هر آلبوم، آهنگ‌هاي خودش را هم حذف مي‌كند)
        for (auto &album : albumRepo.albums(artistId))
            deleteAlbum(album.getId());
        // حذف آهنگ‌هاي Single باقي‌مانده (بدون آلبوم)
        for (auto &song : songRepo.singleSongs(artistId)) {
            playlistRepo.removeSongFromAllPlaylists(song.getId());
            songRepo.remove(song.getId());
        }
        artistRepo.remove(artistId);
    }
};

#endif // ARTISTSERVICE_H
