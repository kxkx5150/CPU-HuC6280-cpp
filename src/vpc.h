#ifndef VPC_H
#define VPC_H
#include <cstdint>

class VPC {
  public:
    int64_t VDCSelect  = 0;
    int64_t  VPCWindow1 = 0;
    int64_t  VPCWindow2 = 0;
    int64_t VPCPriority[4];
    int64_t VPCRegister[8];

    VPC()
    {
        for (int i = 0; i < 4; i++) {
            VPCPriority[i] = 0x01;
        }
        for (int i = 0; i < 8; i++) {
            VPCRegister[i] = 0x00;
        }
        VPCRegister[0] = 0x11;
        VPCRegister[1] = 0x11;
        VPCRegister[7] = 0xff;
    }

    void SetVPC(int64_t no, int64_t data)
    {
        if (no == 0x07)
            return;

        VPCRegister[no] = data;
        if (no == 0x06)
            VDCSelect = data & 0x01;

        if (no == 0x02 || no == 0x03) {
            VPCWindow1 = (VPCRegister[0x02] | ((VPCRegister[0x03] & 0x03) << 8)) - 64;
            if (VPCWindow1 < 0)
                VPCWindow1 = 1024;
        }

        if (no == 0x04 || no == 0x05) {
            VPCWindow2 = (VPCRegister[0x04] | ((VPCRegister[0x05] & 0x03) << 8)) - 64;
            if (VPCWindow2 < 0)
                VPCWindow2 = -1;
        }

        if (no == 0x00) {
            VPCPriority[2] = VPCRegister[0x00] >> 4;
            VPCPriority[3] = VPCRegister[0x00] & 0x0f;
        }

        if (no == 0x01) {
            VPCPriority[0] = VPCRegister[0x01] >> 4;
            VPCPriority[1] = VPCRegister[0x01] & 0x0f;
        }
    }

    int64_t GetVPC(int64_t no)
    {
        return VPCRegister[no];
    }
};
#endif