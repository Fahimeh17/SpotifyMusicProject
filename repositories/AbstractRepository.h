#ifndef ABSTRACTREPOSITORY_H
#define ABSTRACTREPOSITORY_H

#include <optional>

// كلاس الگو (Template) و انتزاعي. طبق سند، هيچ‌كدام از توابع زير در اين
// كلاس پياده‌سازي نمي‌شوند؛ پياده‌سازي در كلاس‌هاي فرزند صورت مي‌گيرد.
// T نوع موجوديتي است كه اين مخزن (Repository) روي آن كار مي‌كند
// (مثلا Song, Album, Playlist, Artist, Listener).
template <typename T>
class AbstractRepository {
public:
    virtual ~AbstractRepository() = default;

    // موجوديت را دريافت مي‌كند؛ اگر وجود نداشته باشد ذخيره مي‌كند و شناسه‌ي
    // جديد را برمي‌گرداند، اگر وجود داشته باشد آن را جايگزين و شناسه‌اش را برمي‌گرداند.
    virtual int save(T &entity) = 0;

    // شناسه را دريافت و موجوديت متناظر را حذف مي‌كند. موفق/ناموفق بودن را برمي‌گرداند.
    virtual bool remove(int id) = 0;

    // شناسه را دريافت و در صورت وجود موجوديت را برمي‌گرداند؛ در غير اين صورت مقدار تهي.
    virtual std::optional<T> search(int id) = 0;
};

#endif // ABSTRACTREPOSITORY_H
