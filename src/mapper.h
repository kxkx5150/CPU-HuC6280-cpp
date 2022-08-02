#include <cstdint>

class Mapper0 {

  public:
    uint8_t *ROM     = nullptr;
    int64_t Address = 0;
    int64_t romlen  = 0;

    Mapper0(uint8_t *rom, int64_t _romlen)
    {
        ROM          = rom;
        romlen       = _romlen;
        int64_t tmp = romlen - 1;
        Address      = 0x80000;

        while (Address > 0x0000) {
            if ((Address & tmp) != 0x0000)
                break;
            Address >>= 1;
        }
    }

    ~Mapper0()
    {
        delete ROM;
    }
    uint8_t Read(int64_t address)
    {
        if (address >= romlen)
            return ROM[(address & (Address - 1)) | Address];
        else
            return ROM[address];
    }

    void Write(int64_t address, int64_t data)
    {
    }
};
