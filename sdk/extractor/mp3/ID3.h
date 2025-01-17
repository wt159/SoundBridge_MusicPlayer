#ifndef ID3_H_
#define ID3_H_

#include "DataSource.hpp"

struct ID3 {
    enum Version {
        ID3_UNKNOWN,
        ID3_V1,
        ID3_V1_1,
        ID3_V2_2,
        ID3_V2_3,
        ID3_V2_4,
    };

    explicit ID3(DataSourceBase *source, bool ignoreV1 = false, off64_t offset = 0);
    ID3(const uint8_t *data, size_t size, bool ignoreV1 = false);
    ~ID3();

    bool isValid() const;

    Version version() const;

    const void *getAlbumArt(size_t *length, std::string *mime) const;

    struct Iterator {
        Iterator(const ID3 &parent, const char *id);
        ~Iterator();

        bool done() const;
        void getID(std::string *id) const;
        void getString(std::string *s, std::string *ss = NULL) const;
        const uint8_t *getData(size_t *length) const;
        void next();

    private:
        const ID3 &mParent;
        char *mID;
        size_t mOffset;

        const uint8_t *mFrameData;
        size_t mFrameSize;

        void findFrame();

        size_t getHeaderLength() const;
        void getstring(std::string *s, bool secondhalf) const;

        Iterator(const Iterator &);
        Iterator &operator=(const Iterator &);
    };

    size_t rawSize() const { return mRawSize; }

private:
    bool mIsValid;
    uint8_t *mData;
    size_t mSize;
    size_t mFirstFrameOffset;
    Version mVersion;

    // size of the ID3 tag including header before any unsynchronization.
    // only valid for IDV2+
    size_t mRawSize;

    bool parseV1(DataSourceBase *source);
    bool parseV2(DataSourceBase *source, off64_t offset);
    void removeUnsynchronization();
    bool removeUnsynchronizationV2_4(bool iTunesHack);

    static bool ParseSyncsafeInteger(const uint8_t encoded[4], size_t *x);

    ID3(const ID3 &);
    ID3 &operator=(const ID3 &);
};

#endif // ID3_H_
