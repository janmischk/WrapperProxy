#pragma once

#include <juce_core/juce_core.h>

// ============================================================
// Cross-Platform Shared Memory Wrapper
// ============================================================
// Erzeugt oder �ffnet einen gemeinsamen Speicherbereich
// zwischen zwei Prozessen, damit z.B. Audio- oder Steuerdaten
// ohne Kopie ausgetauscht werden k�nnen.
// ============================================================

class SharedMemoryBuffer
{
public:
    /** Erstellt oder �ffnet einen Shared Memory Bereich.

        @param name       Eindeutiger Name des Shared Memory Objekts.
        @param bytes      Gr��e des Speicherbereichs in Bytes.
        @param createNew  true = neuen Bereich anlegen, false = existierenden �ffnen.
    */
    SharedMemoryBuffer(const juce::String& name, size_t bytes, bool createNew = false);

    /** Gibt den Speicher wieder frei. */
    ~SharedMemoryBuffer();

    /** Gibt einen Pointer auf den gemeinsamen Speicher zur�ck. */
    void* getData() const noexcept;

    /** Gibt true zur�ck, wenn der Speicher erfolgreich erstellt/verbunden wurde. */
    bool isValid() const noexcept;

    juce::String getSharedBufferName() {
        return memName;
    }

private:
    juce::String memName;
    size_t memSize{};
    void* data = nullptr;


#if JUCE_WINDOWS
    void* handle = nullptr;
#else
    int fd = -1;
#endif
};