#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>

// موجوديت ليست پخش: مخصوص شنوندگان است و امكان انتقال آن به شنونده‌ي ديگر وجود ندارد.
class Playlist {
private:
    int id;
    std::string name;
    int listenerId; // شناسه شنونده صاحب ليست پخش

public:
    Playlist() : id(0), name(""), listenerId(0) {}

    Playlist(int id, const std::string &name, int listenerId)
        : id(id), name(name), listenerId(listenerId) {}

    // ---- getters ----
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getListenerId() const { return listenerId; }

    // ---- setters ----
    void setId(int v) { id = v; }
    void setName(const std::string &v) { name = v; }
    void setListenerId(int v) { listenerId = v; }
};

#endif // PLAYLIST_H
