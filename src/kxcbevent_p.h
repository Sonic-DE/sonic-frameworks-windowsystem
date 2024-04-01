// KXcbEvent allocates the xcb event in a 32 bytes large, zero-initialized buffer to avoid
// out-of-bounds reads and reading data from the stack. Also add alignment for good measure.
// According to XCB documentation, the wire size of all XCB events is 32 bytes, so this should
// work to initialize all the data that will be sent.

template<class T, bool needsPadding = (sizeof(T) < 32)>
struct KXcbEvent;

template<class T>
class alignas(size_t) KXcbEvent<T, false> : public T
{
public:
    inline KXcbEvent()
    {
        static_assert(sizeof(*this) == evt_size);
        memset(this, 0, evt_size);
    }

    const char *buffer() const
    {
        return reinterpret_cast<const char *>(this);
    }

private:
    static constexpr size_t evt_size = 32;
};

template<class T>
class alignas(size_t) KXcbEvent<T, true> : public T
{
public:
    inline KXcbEvent()
    {
        static_assert(sizeof(*this) == evt_size);
        memset(this, 0, evt_size);
    }

    const char *buffer() const
    {
        return reinterpret_cast<const char *>(this);
    }

private:
    static constexpr size_t evt_size = 32;
    char filler[evt_size - sizeof(T)];
};
