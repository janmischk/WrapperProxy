#include "SharedMemoryBuffer.h"

#if JUCE_WINDOWS
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

// ============================================================
// Konstruktor
// ============================================================
SharedMemoryBuffer::SharedMemoryBuffer(const juce::String& name, size_t bytes, bool createNew)
    : memName(name), memSize(bytes)
{
#if JUCE_WINDOWS
    handle = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
        0, (DWORD)bytes, name.toWideCharPointer());
    if (handle != nullptr)
        data = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, bytes);

#else
    int oflag = createNew ? (O_CREAT | O_RDWR) : O_RDWR;
    fd = shm_open(name.toRawUTF8(), oflag, 0666);
    if (fd >= 0)
    {
        ftruncate(fd, (off_t)bytes);
        data = mmap(nullptr, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }
#endif
}

// ============================================================
// Destruktor
// ============================================================
SharedMemoryBuffer::~SharedMemoryBuffer()
{
#if JUCE_WINDOWS
    if (data) UnmapViewOfFile(data);
    if (handle) CloseHandle(handle);
#else
    if (data) munmap(data, memSize);
    if (fd >= 0) close(fd);
#endif
}

// ============================================================
// Accessors
// ============================================================
void* SharedMemoryBuffer::getData() const noexcept { return data; }
bool SharedMemoryBuffer::isValid() const noexcept { return data != nullptr; }