#ifndef ALBUM_H
#define ALBUM_H

#include <string>

// موجوديت آلبوم: نوعي ليست پخش مخصوص هنرمندان است.
class Album {
private:
    int id;
    std::string name;
    int artistId; // شناسه هنرمند صاحب آلبوم
    std::string cover; // جلد آلبوم (امتيازي - فعلا خالي)

public:
    Album() : id(0), name(""), artistId(0), cover("") {}

    Album(int id, const std::string &name, int artistId)
        : id(id), name(name), artistId(artistId), cover("") {}

    // ---- getters ----
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getArtistId() const { return artistId; }
    std::string getCover() const { return cover; }

    // ---- setters ----
    void setId(int v) { id = v; }
    void setName(const std::string &v) { name = v; }
    void setArtistId(int v) { artistId = v; }
    void setCover(const std::string &v) { cover = v; }
};

#endif // ALBUM_H
