#ifndef SONG_H
#define SONG_H

#include <string>

// موجوديت آهنگ. طبق سند، اگر آهنگ متعلق به هيچ آلبومي نباشد،
// مقدار albumId بايد برابر با صفر باشد.
class Song {
private:
    int id;
    std::string name;
    int releaseYear;
    std::string genre;          // سبك آهنگ
    std::string audioFileName;  // نام/مسير فايل صوتي
    int artistId;
    int albumId;                // 0 يعني بدون آلبوم (Single)
    std::string coverImage;     // تصوير آهنگ (امتيازي - فعلا خالي)

public:
    Song()
        : id(0), name(""), releaseYear(0), genre(""), audioFileName(""),
          artistId(0), albumId(0), coverImage("") {}

    Song(int id, const std::string &name, int releaseYear, const std::string &genre,
         const std::string &audioFileName, int artistId, int albumId)
        : id(id), name(name), releaseYear(releaseYear), genre(genre),
          audioFileName(audioFileName), artistId(artistId), albumId(albumId), coverImage("") {}

    // ---- getters ----
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getReleaseYear() const { return releaseYear; }
    std::string getGenre() const { return genre; }
    std::string getAudioFileName() const { return audioFileName; }
    int getArtistId() const { return artistId; }
    int getAlbumId() const { return albumId; }
    std::string getCoverImage() const { return coverImage; }

    // ---- setters ----
    void setId(int v) { id = v; }
    void setName(const std::string &v) { name = v; }
    void setReleaseYear(int v) { releaseYear = v; }
    void setGenre(const std::string &v) { genre = v; }
    void setAudioFileName(const std::string &v) { audioFileName = v; }
    void setArtistId(int v) { artistId = v; }
    void setAlbumId(int v) { albumId = v; }
    void setCoverImage(const std::string &v) { coverImage = v; }
};

#endif // SONG_H
