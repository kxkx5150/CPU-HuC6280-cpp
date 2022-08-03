#include "mem.h"
#include "pce.h"

Mem::Mem(PC *_core)
{
    Core = _core;
}
int64_t Mem::Get(int64_t address)
{
    address = MPR[address >> 13] | (address & 0x1fff);

    if (address < 0x100000) {
        return Core->mapper->Read(address);
    } else if (address < 0x1ee000) {
        return 0xff;
    } else if (address < 0x1f0000) {
        if (BRAMUse)
            return BRAM[address & 0x1fff];
        else
            return 0xff;
    } else if (address < 0x1f8000) {
        return RAM[address & RAMMask];
    } else if (address < 0x1fe000) {
        return 0xff;
    } else if (address < 0x1fe400) {
        switch (address & 0x000003) {
            case 0x00:
                return Core->vdc->GetVDCStatus(0);
            case 0x01:
                return 0x00;
            case 0x02:
                return Core->vdc->GetVDCLow(0);
            case 0x03:
                return Core->vdc->GetVDCHigh(0);
        }
    } else if (address < 0x1fe800) {
        switch (address & 0x000007) {
            case 0x04:
                return Core->vce->GetVCEDataLow();
            case 0x05:
                return Core->vce->GetVCEDataHigh();
            default:
                return 0x00;
        }
    }

    if (address < 0x1fec00) {
        // return Core.psg.GetPSG(address & 0x00000f);
    } else if (address < 0x1ff000) {
        return Core->timer->ReadTimerCounter();
    } else if (address < 0x1ff400) {
        return Core->io->GetJoystick();
    } else if (address < 0x1ff800) {
        switch (address & 0x000003) {
            case 0x02:
                return Core->irq->GetIntDisable();
            case 0x03:
                return Core->irq->GetIntReqest();
            default:
                return 0x00;
        }
    }
    return 0xff;
}
void Mem::Set(int64_t address, int64_t data)
{
    address = MPR[address >> 13] | (address & 0x1fff);

    if (address < 0x100000) {
        Core->mapper->Write(address, data);
        return;
    } else if (address < 0x1ee000) {
        return;
    } else if (address < 0x1f0000) {
        if (BRAMUse)
            BRAM[address & 0x1fff] = data;
        return;
    } else if (address < 0x1f8000) {
        RAM[address & RAMMask] = data;
        return;
    } else if (address < 0x1fe000) {
        return;
    } else if (address < 0x1fe400) {
        switch (address & 0x000003) {
            case 0x00:
                Core->vdc->SetVDCRegister(data, 0);
                break;
            case 0x01:
                break;
            case 0x02:
                Core->vdc->SetVDCLow(data, 0);
                break;
            case 0x03:
                Core->vdc->SetVDCHigh(data, 0);
                break;
        }
        return;
    } else if (address < 0x1fe800) {
        switch (address & 0x000007) {
            case 0x00:
                Core->vce->SetVCEControl(data);
                break;
            case 0x02:
                Core->vce->SetVCEAddressLow(data);
                break;
            case 0x03:
                Core->vce->SetVCEAddressHigh(data);
                break;
            case 0x04:
                Core->vce->SetVCEDataLow(data);
                break;
            case 0x05:
                Core->vce->SetVCEDataHigh(data);
                break;
        }
        return;
    } else if (address < 0x1fec00) {
        // Core.psg.SetPSG(address & 0x00000f, data);
        return;
    } else if (address < 0x1ff000) {
        switch (address & 0x000001) {
            case 0x00:
                Core->timer->WirteTimerReload(data);
                break;
            case 0x01:
                Core->timer->WirteTimerControl(data);
                break;
        }
        return;
    } else if (address < 0x1ff400) {
        Core->io->SetJoystick(data);
        return;
    } else if (address < 0x1ff800) {
        switch (address & 0x000003) {
            case 0x02:
                Core->irq->SetIntDisable(data);
                break;
            case 0x03:
                Core->irq->SetIntReqest(data);
                break;
        }
        return;
    }
}
int64_t Mem::Get16(int64_t address)
{
    return (Get(address + 1) << 8) | Get(address);
}
void Mem::StorageReset()
{
    for (int i = 0; i < 7; i++)
        MPR[i] = 0xff << 13;
    MPR[7]    = 0x00;
    MPRSelect = 0x01;
}
void Mem::Init()
{
    for (int i = 0; i < 0x8000; i++) {
        RAM[i] = 0x00;
    }
    RAMMask = 0x1fff;
    StorageReset();
}