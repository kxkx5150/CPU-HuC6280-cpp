#include <cstdint>
#include <cstdio>
#include <fstream>
#include "cpu.h"
#include "pce.h"

CPU::CPU(Mem *_mem)
{
    mem = _mem;

    A             = 0;
    X             = 0;
    Y             = 0;
    PC            = 0;
    S             = 0;
    P             = 0x00;
    ProgressClock = 0;
    CPUBaseClock  = BaseClock1;
    TransferSrc   = 0;
    TransferDist  = 0;
    TransferLen   = 0;
    TransferAlt   = 0;
    LastInt       = 0x00;

    for (int i = 0; i < 0x100; i++) {
        NZCacheTable[i] = i & 0x80;
    }
    NZCacheTable[0x00] = 0x02;
    file_read();
}
void CPU::CPUReset()
{
    TransferSrc  = 0;
    TransferDist = 0;
    TransferLen  = 0;
    TransferAlt  = 0;
    CPUBaseClock = BaseClock1;
    P |= IFlag;
    PC = mem->Get16(0xfffe);
}
void CPU::CPUInit()
{
    A             = 0;
    X             = 0;
    Y             = 0;
    PC            = 0;
    S             = 0;
    P             = 0x00;
    ProgressClock = 0;
    CPUBaseClock  = BaseClock1;
    TransferSrc   = 0;
    TransferDist  = 0;
    TransferLen   = 0;
    TransferAlt   = 0;
    LastInt       = 0x00;
}
void CPU::CPURun()
{
    ProgressClock = 0;
    int64_t tmp   = LastInt;
    LastInt       = (P & IFlag) == 0x00 ? mem->Core->irq->GetIntStatus() : 0x00;

    if (tmp != 0x00 && TransferLen == 0) {
        LastInt = 0x00;
        if ((tmp & mem->Core->irq->TIQFlag) == mem->Core->irq->TIQFlag) {
            Push(PCH());
            Push(PCL());
            Push(P);
            P  = 0x04;
            PC = mem->Get16(0xfffa);
        } else if ((tmp & mem->Core->irq->IRQ1Flag) == mem->Core->irq->IRQ1Flag) {
            Push(PCH());
            Push(PCL());
            Push(P);
            P  = 0x04;
            PC = mem->Get16(0xfff8);
        } else if ((tmp & mem->Core->irq->IRQ2Flag) == mem->Core->irq->IRQ2Flag) {
            Push(PCH());
            Push(PCL());
            Push(P);
            P |= IFlag;
            PC = mem->Get16(0xfff6);
        }
        ProgressClock = 8 * CPUBaseClock;
    } else {
        int64_t op = mem->Get(PC);
        OpExec(op);
    }
}
int CPU::file_read()
{
    logcheck = false;
    stepinfo = false;
    filename = "logs/log0.txt";

    if (logcheck) {
        std::string   line;
        std::ifstream input_file(filename);
        if (!input_file.is_open()) {
            logcheck = false;
            stepinfo = false;
        }

        while (getline(input_file, line)) {
            lines.push_back(line);
        }

        input_file.close();

        if (filename == "logs/log0.txt") {
            filecheck_start = 1;
            filecheck_end   = 500000;
        } else if (filename == "logs/log1.txt") {
            filecheck_start = 500001;
            filecheck_end   = 1000000;
            fileoffset      = 500000;
        } else if (filename == "logs/log2.txt") {
            filecheck_start = 1000001;
            filecheck_end   = 1500000;
            fileoffset      = 1000000;
        } else if (filename == "logs/log3.txt") {
            filecheck_start = 1500001;
            filecheck_end   = 2000000;
            fileoffset      = 1500000;
        } else if (filename == "logs/log4.txt") {
            filecheck_start = 2000001;
            filecheck_end   = 2500000;
            fileoffset      = 2000000;
        } else if (filename == "logs/log5.txt") {
            filecheck_start = 2500001;
            filecheck_end   = 3000000;
            fileoffset      = 2500000;
        }
    }
    return EXIT_SUCCESS;
}
void CPU::Dump(int64_t op)
{
    if (logcheck && filecheck_start <= counts && counts <= filecheck_end) {

        char buf1[1000];
        char buf2[1000];
        char buf3[1000];
        char buf4[1000];
        char buf5[1000];
        char buf6[1000];
        char buf7[1000];
        char buf8[1000];
        char buf9[1000];

        sprintf(buf1, "counts:%ld op:%ld", counts, op);

        sprintf(buf2, "A:%ld X:%ld Y:%ld PC:%ld S:%ld P:%ld", A, X, Y, PC, S, P);

        sprintf(buf3,
                "ProgressClock:%ld CPUBaseClock:%ld TransferSrc:%ld TransferDist:%ld TransferLen:%ld TransferAlt:%ld",
                ProgressClock, CPUBaseClock, TransferSrc, TransferDist, TransferLen, TransferAlt);

        sprintf(buf4, "VCE:  VCEControl:%ld VCEAddress:%ld VCEData:%ld", mem->Core->vce->VCEControl,
                mem->Core->vce->VCEAddress, mem->Core->vce->VCEData);

        sprintf(buf5, "VPC:  VDCSelect:%ld VPCWindow1:%ld VPCWindow2:%ld", mem->Core->vpc->VDCSelect,
                mem->Core->vpc->VPCWindow1, mem->Core->vpc->VPCWindow2);

        sprintf(
            buf6,
            "VDCBurst:%s SpriteLimit:%s VDCStatus:%ld VDCRegisterSelect:%ld WriteVRAMData:%ld VRAMtoSATBStartFlag:%s",
            (mem->Core->vdc->vdcp[0]->VDCBurst ? "true" : "false"),
            (mem->Core->vdc->vdcp[0]->SpriteLimit ? "true" : "false"), mem->Core->vdc->vdcp[0]->VDCStatus,
            mem->Core->vdc->vdcp[0]->VDCRegisterSelect, mem->Core->vdc->vdcp[0]->WriteVRAMData,
            mem->Core->vdc->vdcp[0]->VRAMtoSATBStartFlag ? "true" : "false");

        sprintf(buf7, "VDCPutLine:%ld VRAMtoSATBCount:%ld VRAMtoVRAMCount:%ld RasterCount:%ld VDCProgressClock:%ld",
                mem->Core->vdc->VDCPutLine, mem->Core->vdc->vdcp[0]->VRAMtoSATBCount,
                mem->Core->vdc->vdcp[0]->VRAMtoVRAMCount, mem->Core->vdc->vdcp[0]->RasterCount,
                mem->Core->vdc->vdcp[0]->VDCProgressClock);

        sprintf(buf8, "DrawBGYLine:%ld DrawBGLine:%ld DrawLineWidth:%ld DrawBGIndex:%ld",
                mem->Core->vdc->vdcp[0]->DrawBGYLine, mem->Core->vdc->vdcp[0]->DrawBGLine,
                mem->Core->vdc->vdcp[0]->DrawLineWidth, mem->Core->vdc->vdcp[0]->DrawBGIndex);

        sprintf(buf9, "HDS:%ld HSW:%ld HDE:%ld HDW:%ld VDS:%ld VSW:%ld VDW:%ld VCR:%ld", mem->Core->vdc->vdcp[0]->HDS,
                mem->Core->vdc->vdcp[0]->HSW, mem->Core->vdc->vdcp[0]->HDE, mem->Core->vdc->vdcp[0]->HDW,
                mem->Core->vdc->vdcp[0]->VDS, mem->Core->vdc->vdcp[0]->VSW, mem->Core->vdc->vdcp[0]->VDW,
                mem->Core->vdc->vdcp[0]->VCR);

        if (stepinfo) {
            printf("\n");
            printf("count : %zu\n", counts);
            printf("%s\n", buf1);
            printf("%s\n", buf2);
            printf("%s\n", buf3);
            printf("%s\n", buf4);
            printf("%s\n", buf5);
            printf("%s\n", buf6);
            printf("%s\n", buf7);
            printf("%s\n", buf8);
            printf("%s\n", buf9);
        }

        if (counts < filecheck_end) {
            int len = lines[0].size() + 1010;

            char *sbuf = new char[len];
            sprintf(sbuf, "%s", lines[counts - 1 - fileoffset].c_str());
            std::string s = sbuf;

            char *tbf = new char[len];
            sprintf(tbf, "%s %s %s %s %s %s %s %s %s", buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8, buf9);
            std::string t = tbf;

            if (std::equal(t.begin(), t.end(), s.begin())) {
                // printf("ok !\n");
            } else {
                printf("\n\n\n***************\n");
                printf("*** Error ! ***\n");
                printf("***************\n\n\n");

                printf("count : %zu\n", counts);
                printf("OK : %s\n", lines[counts - 1 - fileoffset].c_str());
                printf("NG : %s %s %s %s %s %s %s %s %s\n\n", buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8, buf9);
                exit(1);
            }

            delete[] sbuf;
            delete[] tbf;
        } else {
            printf("\n\n\n\n\n-- Compare OK ! ---\n");
            printf("count : %zu\n\n", counts);
            printf("\n\n\n\n\n\n");
            exit(1);
        }
    }
}
void CPU::OpExec(int64_t op)
{
    counts++;
    Dump(op);
    if (counts == 2900000) {
        printf(" ");
    }

    switch (op) {
        case 0x69: {
            ADC(PC + 1);
        } break;
        case 0x65: {
            ADC(ZP());
        } break;
        case 0x75: {
            ADC(ZP_X());
        } break;
        case 0x72: {
            ADC(IND());
        } break;
        case 0x61: {
            ADC(IND_X());
        } break;
        case 0x71: {
            ADC(IND_Y());
        } break;
        case 0x6d: {
            ADC(ABS());
        } break;
        case 0x7d: {
            ADC(ABS_X());
        } break;
        case 0x79: {
            ADC(ABS_Y());
        } break;
        case 0xe9: {
            SBC(PC + 1);
        } break;
        case 0xe5: {
            SBC(ZP());
        } break;
        case 0xf5: {
            SBC(ZP_X());
        } break;
        case 0xf2: {
            SBC(IND());
        } break;
        case 0xe1: {
            SBC(IND_X());
        } break;
        case 0xf1: {
            SBC(IND_Y());
        } break;
        case 0xed: {
            SBC(ABS());
        } break;
        case 0xfd: {
            SBC(ABS_X());
        } break;
        case 0xf9: {
            SBC(ABS_Y());
        } break;
        case 0x29: {
            AND(PC + 1);
        } break;
        case 0x25: {
            AND(ZP());
        } break;
        case 0x35: {
            AND(ZP_X());
        } break;
        case 0x32: {
            AND(IND());
        } break;
        case 0x21: {
            AND(IND_X());
        } break;
        case 0x31: {
            AND(IND_Y());
        } break;
        case 0x2d: {
            AND(ABS());
        } break;
        case 0x3d: {
            AND(ABS_X());
        } break;
        case 0x39: {
            AND(ABS_Y());
        } break;
        case 0x49: {
            EOR(PC + 1);
        } break;
        case 0x45: {
            EOR(ZP());
        } break;
        case 0x55: {
            EOR(ZP_X());
        } break;
        case 0x52: {
            EOR(IND());
        } break;
        case 0x41: {
            EOR(IND_X());
        } break;
        case 0x51: {
            EOR(IND_Y());
        } break;
        case 0x4d: {
            EOR(ABS());
        } break;
        case 0x5d: {
            EOR(ABS_X());
        } break;
        case 0x59: {
            EOR(ABS_Y());
        } break;
        case 0x09: {
            ORA(PC + 1);
        } break;
        case 0x05: {
            ORA(ZP());
        } break;
        case 0x15: {
            ORA(ZP_X());
        } break;
        case 0x12: {
            ORA(IND());
        } break;
        case 0x01: {
            ORA(IND_X());
        } break;
        case 0x11: {
            ORA(IND_Y());
        } break;
        case 0x0d: {
            ORA(ABS());
        } break;
        case 0x1d: {
            ORA(ABS_X());
        } break;
        case 0x19: {
            ORA(ABS_Y());
        } break;
        case 0x06: {
            int64_t address = ZP();
            mem->Set(address, ASL(mem->Get(address)));
        } break;
        case 0x16: {
            int64_t address = ZP_X();
            mem->Set(address, ASL(mem->Get(address)));
        } break;
        case 0x0e: {
            int64_t address = ABS();
            mem->Set(address, ASL(mem->Get(address)));
        } break;
        case 0x1e: {
            int64_t address = ABS_X();
            mem->Set(address, ASL(mem->Get(address)));
        } break;
        case 0x0a: {
            A = ASL(A);
        } break;
        case 0x46: {
            int64_t address = ZP();
            mem->Set(address, LSR(mem->Get(address)));
        } break;
        case 0x56: {
            int64_t address = ZP_X();
            mem->Set(address, LSR(mem->Get(address)));
        } break;
        case 0x4e: {
            int64_t address = ABS();
            mem->Set(address, LSR(mem->Get(address)));
        } break;
        case 0x5e: {
            int64_t address = ABS_X();
            mem->Set(address, LSR(mem->Get(address)));
        } break;
        case 0x4a: {
            A = LSR(A);
        } break;
        case 0x26: {
            int64_t address = ZP();
            mem->Set(address, ROL(mem->Get(address)));
        } break;
        case 0x36: {
            int64_t address = ZP_X();
            mem->Set(address, ROL(mem->Get(address)));
        } break;
        case 0x2e: {
            int64_t address = ABS();
            mem->Set(address, ROL(mem->Get(address)));
        } break;
        case 0x3e: {
            int64_t address = ABS_X();
            mem->Set(address, ROL(mem->Get(address)));
        } break;
        case 0x2a: {
            A = ROL(A);
        } break;
        case 0x66: {
            int64_t address = ZP();
            mem->Set(address, ROR(mem->Get(address)));
        } break;
        case 0x76: {
            int64_t address = ZP_X();
            mem->Set(address, ROR(mem->Get(address)));
        } break;
        case 0x6e: {
            int64_t address = ABS();
            mem->Set(address, ROR(mem->Get(address)));
        } break;
        case 0x7e: {
            int64_t address = ABS_X();
            mem->Set(address, ROR(mem->Get(address)));
        } break;
        case 0x6a: {
            A = ROR(A);
        } break;
        case 0x0f: {
            BBRi(0);
        } break;
        case 0x1f: {
            BBRi(1);
        } break;
        case 0x2f: {
            BBRi(2);
        } break;
        case 0x3f: {
            BBRi(3);
        } break;
        case 0x4f: {
            BBRi(4);
        } break;
        case 0x5f: {
            BBRi(5);
        } break;
        case 0x6f: {
            BBRi(6);
        } break;
        case 0x7f: {
            BBRi(7);
        } break;
        case 0x8f: {
            BBSi(0);
        } break;
        case 0x9f: {
            BBSi(1);
        } break;
        case 0xaf: {
            BBSi(2);
        } break;
        case 0xbf: {
            BBSi(3);
        } break;
        case 0xcf: {
            BBSi(4);
        } break;
        case 0xdf: {
            BBSi(5);
        } break;
        case 0xef: {
            BBSi(6);
        } break;
        case 0xff: {
            BBSi(7);
        } break;
        case 0x90: {
            Branch((P & CFlag) == 0x00, 1);
        } break;
        case 0xb0: {
            Branch((P & CFlag) == CFlag, 1);
        } break;
        case 0xd0: {
            Branch((P & ZFlag) == 0x00, 1);
        } break;
        case 0xf0: {
            Branch((P & ZFlag) == ZFlag, 1);
        } break;
        case 0x10: {
            Branch((P & NFlag) == 0x00, 1);
        } break;
        case 0x30: {
            Branch((P & NFlag) == NFlag, 1);
        } break;
        case 0x50: {
            Branch((P & VFlag) == 0x00, 1);
        } break;
        case 0x70: {
            Branch((P & VFlag) == VFlag, 1);
        } break;
        case 0x80: {
            Branch(true, 1);
        } break;
        case 0x44: {
            PC++;
            Push(PCH());
            Push(PCL());
            Branch(true, 0);
        } break;
        case 0x20: {
            int64_t tmp = ABS();
            PC += 2;
            Push(PCH());
            Push(PCL());
            PC = tmp;
            P &= ~TFlag;
        } break;
        case 0x40: {
            P = Pull();
            toPCL(Pull());
            toPCH(Pull());
        } break;
        case 0x60: {
            P &= ~TFlag;
            toPCL(Pull());
            toPCH(Pull());
            PC++;
        } break;
        case 0x4c: {
            PC = ABS();
            P &= ~TFlag;
        } break;
        case 0x6c: {
            PC = ABS_IND();
            P &= ~TFlag;
        } break;
        case 0x7c: {
            PC = ABS_X_IND();
            P &= ~TFlag;
        } break;
        case 0x00: {
            PC += 2;
            Push(PCH());
            Push(PCL());
            P |= BFlag;
            Push(P);
            P &= ~DFlag;
            P &= ~TFlag;
            P |= IFlag;
            PC = mem->Get16(0xfff6);
        } break;
        case 0x62: {
            A = 0x00;
            P &= ~TFlag;
        } break;
        case 0x82: {
            X = 0x00;
            P &= ~TFlag;
        } break;
        case 0xc2: {
            Y = 0x00;
            P &= ~TFlag;
        } break;

        case 0x18: {
            P &= ~CFlag;
            P &= ~TFlag;
        } break;
        case 0xd8: {
            P &= ~DFlag;
            P &= ~TFlag;
        } break;
        case 0x58: {
            P &= ~IFlag;
            P &= ~TFlag;
        } break;
        case 0xb8: {
            P &= ~VFlag;
            P &= ~TFlag;
        } break;
        case 0x38: {
            P |= CFlag;
            P &= ~TFlag;
        } break;
        case 0xf8: {
            P |= DFlag;
            P &= ~TFlag;
        } break;
        case 0x78: {
            P |= IFlag;
            P &= ~TFlag;
        } break;
        case 0xf4: {
            P |= TFlag;
        } break;
        case 0xc9: {
            Compare(A, PC + 1);
        } break;
        case 0xc5: {
            Compare(A, ZP());
        } break;
        case 0xd5: {
            Compare(A, ZP_X());
        } break;
        case 0xd2: {
            Compare(A, IND());
        } break;
        case 0xc1: {
            Compare(A, IND_X());
        } break;
        case 0xd1: {
            Compare(A, IND_Y());
        } break;
        case 0xcd: {
            Compare(A, ABS());
        } break;
        case 0xdd: {
            Compare(A, ABS_X());
        } break;
        case 0xd9: {
            Compare(A, ABS_Y());
        } break;
        case 0xe0: {
            Compare(X, PC + 1);
        } break;
        case 0xe4: {
            Compare(X, ZP());
        } break;
        case 0xec: {
            Compare(X, ABS());
        } break;
        case 0xc0: {
            Compare(Y, PC + 1);
        } break;
        case 0xc4: {
            Compare(Y, ZP());
        } break;
        case 0xcc: {
            Compare(Y, ABS());
        } break;
        case 0xc6: {
            int64_t address = ZP();
            mem->Set(address, Decrement(mem->Get(address)));
        } break;
        case 0xd6: {
            int64_t address = ZP_X();
            mem->Set(address, Decrement(mem->Get(address)));
        } break;
        case 0xce: {
            int64_t address = ABS();
            mem->Set(address, Decrement(mem->Get(address)));
        } break;
        case 0xde: {
            int64_t address = ABS_X();
            mem->Set(address, Decrement(mem->Get(address)));
        } break;
        case 0x3a: {
            A = Decrement(A);
        } break;
        case 0xca: {
            X = Decrement(X);
        } break;
        case 0x88: {
            Y = Decrement(Y);
        } break;
        case 0xe6: {
            int64_t address = ZP();
            mem->Set(address, Increment(mem->Get(address)));
        } break;
        case 0xf6: {
            int64_t address = ZP_X();
            mem->Set(address, Increment(mem->Get(address)));
        } break;
        case 0xee: {
            int64_t address = ABS();
            mem->Set(address, Increment(mem->Get(address)));
        } break;
        case 0xfe: {
            int64_t address = ABS_X();
            mem->Set(address, Increment(mem->Get(address)));
        } break;
        case 0x1a: {
            A = Increment(A);
        } break;
        case 0xe8: {
            X = Increment(X);
        } break;
        case 0xc8: {
            Y = Increment(Y);
        } break;
        case 0x48: {
            Push(A);
            P &= ~TFlag;
        } break;
        case 0x08: {
            Push(P);
            P &= ~TFlag;
        } break;
        case 0xda: {
            Push(X);
            P &= ~TFlag;
        } break;
        case 0x5a: {
            Push(Y);
            P &= ~TFlag;
        } break;
        case 0x68: {
            A = Pull();
            SetNZFlag(A);
            P &= ~TFlag;
        } break;
        case 0x28: {
            P = Pull();
        } break;
        case 0xfa: {
            X = Pull();
            SetNZFlag(X);
            P &= ~TFlag;
        } break;
        case 0x7a: {
            Y = Pull();
            SetNZFlag(Y);
            P &= ~TFlag;
        } break;
        case 0x07: {
            RMBi(0);
        } break;
        case 0x17: {
            RMBi(1);
        } break;
        case 0x27: {
            RMBi(2);
        } break;
        case 0x37: {
            RMBi(3);
        } break;
        case 0x47: {
            RMBi(4);
        } break;
        case 0x57: {
            RMBi(5);
        } break;
        case 0x67: {
            RMBi(6);
        } break;
        case 0x77: {
            RMBi(7);
        } break;
        case 0x87: {
            SMBi(0);
        } break;
        case 0x97: {
            SMBi(1);
        } break;
        case 0xa7: {
            SMBi(2);
        } break;
        case 0xb7: {
            SMBi(3);
        } break;
        case 0xc7: {
            SMBi(4);
        } break;
        case 0xd7: {
            SMBi(5);
        } break;
        case 0xe7: {
            SMBi(6);
        } break;
        case 0xf7: {
            SMBi(7);
        } break;
        case 0x22: {
            int64_t tmp = A;
            A           = X;
            X           = tmp;
            P &= ~TFlag;
        } break;
        case 0x42: {
            int64_t tmp = A;
            A           = Y;
            Y           = tmp;
            P &= ~TFlag;
        } break;
        case 0x02: {
            int64_t tmp = X;
            X           = Y;
            Y           = tmp;
            P &= ~TFlag;
        } break;
        case 0xaa: {
            X = A;
            SetNZFlag(X);
            P &= ~TFlag;
        } break;
        case 0xa8: {
            Y = A;
            SetNZFlag(Y);
            P &= ~TFlag;
        } break;
        case 0xba: {
            X = S;
            SetNZFlag(X);
            P &= ~TFlag;
        } break;
        case 0x8a: {
            A = X;
            SetNZFlag(A);
            P &= ~TFlag;
        } break;
        case 0x9a: {
            S = X;
            P &= ~TFlag;
        } break;
        case 0x98: {
            A = Y;
            SetNZFlag(A);
            P &= ~TFlag;
        } break;
        case 0x89: {
            BIT(PC + 1);
        } break;
        case 0x24: {
            BIT(ZP());
        } break;
        case 0x34: {
            BIT(ZP_X());
        } break;
        case 0x2c: {
            BIT(ABS());
        } break;
        case 0x3c: {
            BIT(ABS_X());
        } break;
        case 0x83: {
            TST(PC + 1, 0x2000 | mem->Get(PC + 2));
        } break;
        case 0xa3: {
            TST(PC + 1, 0x2000 | ((mem->Get(PC + 2) + X) & 0xff));
        } break;
        case 0x93: {
            TST(PC + 1, mem->Get16(PC + 2));
        } break;
        case 0xb3: {
            TST(PC + 1, (mem->Get16(PC + 2) + X) & 0xffff);
        } break;
        case 0x14: {
            TRB(ZP());
        } break;
        case 0x1c: {
            TRB(ABS());
        } break;
        case 0x04: {
            TSB(ZP());
        } break;
        case 0x0c: {
            TSB(ABS());
        } break;
        case 0xa9: {
            A = Load(PC + 1);
        } break;
        case 0xa5: {
            A = Load(ZP());
        } break;
        case 0xb5: {
            A = Load(ZP_X());
        } break;
        case 0xb2: {
            A = Load(IND());
        } break;
        case 0xa1: {
            A = Load(IND_X());
        } break;
        case 0xb1: {
            A = Load(IND_Y());
        } break;
        case 0xad: {
            A = Load(ABS());
        } break;
        case 0xbd: {
            A = Load(ABS_X());
        } break;
        case 0xb9: {
            A = Load(ABS_Y());
        } break;
        case 0xa2: {
            X = Load(PC + 1);
        } break;
        case 0xa6: {
            X = Load(ZP());
        } break;
        case 0xb6: {
            X = Load(ZP_Y());
        } break;
        case 0xae: {
            X = Load(ABS());
        } break;
        case 0xbe: {
            X = Load(ABS_Y());
        } break;
        case 0xa0: {
            Y = Load(PC + 1);
        } break;
        case 0xa4: {
            Y = Load(ZP());
        } break;
        case 0xb4: {
            Y = Load(ZP_X());
        } break;
        case 0xac: {
            Y = Load(ABS());
        } break;
        case 0xbc: {
            Y = Load(ABS_X());
        } break;
        case 0x85: {
            Store(ZP(), A);
        } break;
        case 0x95: {
            Store(ZP_X(), A);
        } break;
        case 0x92: {
            Store(IND(), A);
        } break;
        case 0x81: {
            Store(IND_X(), A);
        } break;
        case 0x91: {
            Store(IND_Y(), A);
        } break;
        case 0x8d: {
            Store(ABS(), A);
        } break;
        case 0x9d: {
            Store(ABS_X(), A);
        } break;
        case 0x99: {
            Store(ABS_Y(), A);
        } break;
        case 0x86: {
            Store(ZP(), X);
        } break;
        case 0x96: {
            Store(ZP_Y(), X);
        } break;
        case 0x8e: {
            Store(ABS(), X);
        } break;
        case 0x84: {
            Store(ZP(), Y);
        } break;
        case 0x94: {
            Store(ZP_X(), Y);
        } break;
        case 0x8c: {
            Store(ABS(), Y);
        } break;
        case 0x64: {
            Store(ZP(), 0x00);
        } break;
        case 0x74: {
            Store(ZP_X(), 0x00);
        } break;
        case 0x9c: {
            Store(ABS(), 0x00);
        } break;
        case 0x9e: {
            Store(ABS_X(), 0x00);
        } break;
        case 0xea: {
            P &= ~TFlag;
        } break;
        case 0x03: {
            mem->Core->vdc->SetVDCRegister(mem->Get(PC + 1), mem->Core->vpc->VDCSelect);
            P &= ~TFlag;
        } break;
        case 0x13: {
            mem->Core->vdc->SetVDCLow(mem->Get(PC + 1), mem->Core->vpc->VDCSelect);
            P &= ~TFlag;
        } break;
        case 0x23: {
            mem->Core->vdc->SetVDCHigh(mem->Get(PC + 1), mem->Core->vpc->VDCSelect);
            P &= ~TFlag;
        } break;
        case 0x53: {
            int64_t data = mem->Get(PC + 1);
            int64_t bit  = 0x01;
            if (data == 0x00)
                data = mem->MPRSelect;
            else
                mem->MPRSelect = data;
            for (int64_t i = 0; i < 8; i++)
                if ((data & (bit << i)) != 0x00)
                    mem->MPR[i] = A << 13;
        } break;
        case 0x43: {
            int64_t data = mem->Get(PC + 1);
            int64_t bit  = 0x01;
            if (data == 0x00)
                data = mem->MPRSelect;
            else
                mem->MPRSelect = data;

            for (int64_t i = 0; i < 8; i++)
                if ((data & (bit << i)) != 0x00) {
                    uint64_t tmp = mem->MPR[i];
                    A            = tmp >> 13;
                }
        } break;
        case 0xf3: {
            if (TransferLen == 0) {
                TransferSrc   = mem->Get16(PC + 1);
                TransferDist  = mem->Get16(PC + 3);
                TransferLen   = mem->Get16(PC + 5);
                TransferAlt   = 1;
                ProgressClock = 17;
            }
            mem->Set(TransferDist, mem->Get(TransferSrc));
            TransferSrc  = (TransferSrc + TransferAlt) & 0xffff;
            TransferDist = (TransferDist + 1) & 0xffff;
            TransferLen  = (TransferLen - 1) & 0xffff;
            TransferAlt  = TransferAlt == 1 ? -1 : 1;
            ProgressClock += 6;
            if (TransferLen == 0) {
                P &= ~TFlag;
                PC += 7;
            }
        } break;
        case 0xc3: {
            if (TransferLen == 0) {
                TransferSrc   = mem->Get16(PC + 1);
                TransferDist  = mem->Get16(PC + 3);
                TransferLen   = mem->Get16(PC + 5);
                ProgressClock = 17;
            }
            mem->Set(TransferDist, mem->Get(TransferSrc));
            TransferSrc  = (TransferSrc - 1) & 0xffff;
            TransferDist = (TransferDist - 1) & 0xffff;
            TransferLen  = (TransferLen - 1) & 0xffff;
            ProgressClock += 6;
            if (TransferLen == 0) {
                P &= ~TFlag;
                PC += 7;
            }
        } break;
        case 0xe3: {
            if (TransferLen == 0) {
                TransferSrc   = mem->Get16(PC + 1);
                TransferDist  = mem->Get16(PC + 3);
                TransferLen   = mem->Get16(PC + 5);
                TransferAlt   = 1;
                ProgressClock = 17;
            }
            mem->Set(TransferDist, mem->Get(TransferSrc));
            TransferSrc  = (TransferSrc + 1) & 0xffff;
            TransferDist = (TransferDist + TransferAlt) & 0xffff;
            TransferLen  = (TransferLen - 1) & 0xffff;
            TransferAlt  = TransferAlt == 1 ? -1 : 1;
            ProgressClock += 6;
            if (TransferLen == 0) {
                P &= ~TFlag;
                PC += 7;
            }
        } break;
        case 0x73: {
            if (TransferLen == 0) {
                TransferSrc   = mem->Get16(PC + 1);
                TransferDist  = mem->Get16(PC + 3);
                TransferLen   = mem->Get16(PC + 5);
                ProgressClock = 17;
            }
            mem->Set(TransferDist, mem->Get(TransferSrc));
            TransferSrc  = (TransferSrc + 1) & 0xffff;
            TransferDist = (TransferDist + 1) & 0xffff;
            TransferLen  = (TransferLen - 1) & 0xffff;
            ProgressClock += 6;
            if (TransferLen == 0) {
                P &= ~TFlag;
                PC += 7;
            }
        } break;
        case 0xd3: {
            if (TransferLen == 0) {
                TransferSrc   = mem->Get16(PC + 1);
                TransferDist  = mem->Get16(PC + 3);
                TransferLen   = mem->Get16(PC + 5);
                ProgressClock = 17;
            }
            mem->Set(TransferDist, mem->Get(TransferSrc));
            TransferSrc = (TransferSrc + 1) & 0xffff;
            TransferLen = (TransferLen - 1) & 0xffff;
            ProgressClock += 6;
            if (TransferLen == 0) {
                P &= ~TFlag;
                PC += 7;
            }
        } break;
        case 0xd4: {
            P &= ~TFlag;
            CPUBaseClock = BaseClock7;
        } break;
        case 0x54: {
            P &= ~TFlag;
            CPUBaseClock = BaseClock1;
        } break;
        default: {
            P &= ~TFlag;
        } break;
    }
    PC += OpBytes[op];
    ProgressClock = (ProgressClock + OpCycles[op]) * CPUBaseClock;
}
void CPU::Adder(int64_t address, int64_t neg)
{
    int64_t data0;
    int64_t data1 = mem->Get(address);
    if (!neg && (P & TFlag) == TFlag) {
        ProgressClock = 3;
        data0         = mem->Get(0x2000 | X);
    } else
        data0 = A;
    if (neg)
        data1 = ~data1 & 0xff;
    int64_t carry = P & 0x01;
    int64_t tmp   = data0 + data1 + carry;
    if ((P & DFlag) == 0x00) {
        if ((((~data0 & ~data1 & tmp) | (data0 & data1 & ~tmp)) & 0x80) == 0x80)
            P |= VFlag;
        else
            P &= ~VFlag;
    } else {
        ProgressClock += 1;
        if (neg) {
            if ((tmp & 0x0f) > 0x09)
                tmp -= 0x06;
            if ((tmp & 0xf0) > 0x90)
                tmp -= 0x60;
        } else {
            if ((data0 & 0x0f) + (data1 & 0x0f) + carry > 0x09)
                tmp += 0x06;
            if ((tmp & 0x1f0) > 0x90)
                tmp += 0x60;
        }
    }
    if (tmp > 0xff)
        P |= CFlag;
    else
        P &= ~CFlag;
    tmp &= 0xff;
    SetNZFlag(tmp);
    if (!neg && (P & TFlag) == TFlag)
        mem->Set(0x2000 | X, tmp);
    else
        A = tmp;
    P &= ~TFlag;
}

void CPU::ADC(int64_t address)
{
    Adder(address, false);
}
void CPU::SBC(int64_t address)
{
    Adder(address, true);
}
void CPU::AND(int64_t address)
{
    int64_t data0;
    int64_t data1 = mem->Get(address);
    if ((P & TFlag) == 0x00) {
        data0 = A;
    } else {
        ProgressClock = 3;
        data0         = mem->Get(0x2000 | X);
    }
    int64_t tmp = data0 & data1;
    SetNZFlag(tmp);
    if ((P & TFlag) == 0x00)
        A = tmp;
    else
        mem->Set(0x2000 | X, tmp);
    P &= ~TFlag;
}
void CPU::EOR(int64_t address)
{
    int64_t data0;
    int64_t data1 = mem->Get(address);
    if ((P & TFlag) == 0x00) {
        data0 = A;
    } else {
        ProgressClock = 3;
        data0         = mem->Get(0x2000 | X);
    }
    int64_t tmp = data0 ^ data1;
    SetNZFlag(tmp);
    if ((P & TFlag) == 0x00)
        A = tmp;
    else
        mem->Set(0x2000 | X, tmp);
    P &= ~TFlag;
}
void CPU::ORA(int64_t address)
{
    int64_t data0;
    int64_t data1 = mem->Get(address);
    if ((P & TFlag) == 0x00) {
        data0 = A;
    } else {
        ProgressClock = 3;
        data0         = mem->Get(0x2000 | X);
    }
    int64_t tmp = data0 | data1;
    SetNZFlag(tmp);
    if ((P & TFlag) == 0x00)
        A = tmp;
    else
        mem->Set(0x2000 | X, tmp);
    P &= ~TFlag;
}
int64_t CPU::ASL(int64_t data)
{
    data <<= 1;
    if (data > 0xff)
        P |= CFlag;
    else
        P &= ~CFlag;
    data &= 0xff;
    SetNZFlag(data);
    P &= ~TFlag;
    return data;
}
int64_t CPU::LSR(int64_t data)
{
    if ((data & 0x01) == 0x01)
        P |= CFlag;
    else
        P &= ~CFlag;
    data >>= 1;
    SetNZFlag(data);
    P &= ~TFlag;
    return data;
}
int64_t CPU::ROL(int64_t data)
{
    data = (data << 1) | (P & 0x01);
    if (data > 0xff)
        P |= CFlag;
    else
        P &= ~CFlag;
    data &= 0xff;
    SetNZFlag(data);
    P &= ~TFlag;
    return data;
}
int64_t CPU::ROR(int64_t data)
{
    int64_t tmp = P & CFlag;
    if ((data & 0x01) == 0x01)
        P |= CFlag;
    else
        P &= ~CFlag;
    data = (data >> 1) | (tmp << 7);
    SetNZFlag(data);
    P &= ~TFlag;
    return data;
}
void CPU::BBRi(int64_t bit)
{
    int64_t tmp = mem->Get(ZP());
    tmp         = (tmp >> bit) & 0x01;
    Branch(tmp == 0, 2);
}
void CPU::BBSi(int64_t bit)
{
    int64_t tmp = mem->Get(ZP());
    tmp         = (tmp >> bit) & 0x01;
    Branch(tmp == 1, 2);
}
void CPU::Branch(bool status, int64_t adr)
{
    P &= ~TFlag;
    if (status) {
        int64_t tmp = mem->Get(PC + adr);
        if (tmp >= 0x80)
            tmp |= 0xff00;
        PC            = (PC + adr + 1 + tmp) & 0xffff;
        ProgressClock = 2;
    } else
        PC += adr + 1;
}
void CPU::Compare(int64_t data0, int64_t data1)
{
    int64_t data0i  = data0;
    int64_t tmpdata = mem->Get(data1);
    data0i          = data0i - tmpdata;
    if (data0i < 0)
        P &= ~CFlag;
    else
        P |= CFlag;
    P &= ~TFlag;
    SetNZFlag(data0i & 0xff);
}
int64_t CPU::Decrement(int64_t data)
{
    data = (data - 1) & 0xff;
    SetNZFlag(data);
    P &= ~TFlag;
    return data;
}
int64_t CPU::Increment(int64_t data)
{
    data = (data + 1) & 0xff;
    SetNZFlag(data);
    P &= ~TFlag;
    return data;
}
void CPU::Push(int64_t data)
{
    mem->Set(0x2100 | S, data);
    S = (S - 1) & 0xff;
}
int64_t CPU::Pull()
{
    S = (S + 1) & 0xff;
    return mem->Get(0x2100 | S);
}
void CPU::RMBi(int64_t bit)
{
    int64_t address = ZP();
    mem->Set(address, mem->Get(address) & ~(0x01 << bit));
    P &= ~TFlag;
}
void CPU::SMBi(int64_t bit)
{
    int64_t address = ZP();
    mem->Set(address, mem->Get(address) | (0x01 << bit));
    P &= ~TFlag;
}
void CPU::BIT(int64_t address)
{
    int64_t tmp = mem->Get(address);
    SetNZFlag(A & tmp);
    P = (P & ~(NFlag | VFlag)) | (tmp & (NFlag | VFlag));
    P &= ~TFlag;
}
void CPU::TST(int64_t address0, int64_t address1)
{
    int64_t tmp0 = mem->Get(address0);
    int64_t tmp1 = mem->Get(address1);
    SetNZFlag(tmp0 & tmp1);
    P = (P & ~(NFlag | VFlag)) | (tmp1 & (NFlag | VFlag));
    P &= ~TFlag;
}
void CPU::TRB(int64_t address)
{
    int64_t tmp = mem->Get(address);
    int64_t res = ~A & tmp;
    mem->Set(address, res);
    SetNZFlag(res);
    P = (P & ~(NFlag | VFlag)) | (tmp & (NFlag | VFlag));
    P &= ~TFlag;
}
void CPU::TSB(int64_t address)
{
    int64_t tmp = mem->Get(address);
    int64_t res = A | tmp;
    mem->Set(address, res);
    SetNZFlag(res);
    P = (P & ~(NFlag | VFlag)) | (tmp & (NFlag | VFlag));
    P &= ~TFlag;
}
int64_t CPU::Load(int64_t address)
{
    int64_t data = mem->Get(address);
    SetNZFlag(data);
    P &= ~TFlag;
    return data;
}
void CPU::Store(int64_t address, int64_t data)
{
    mem->Set(address, data);
    P &= ~TFlag;
}
int64_t CPU::ZP()
{
    return 0x2000 | mem->Get(PC + 1);
}
int64_t CPU::ZP_X()
{
    return 0x2000 | ((mem->Get(PC + 1) + X) & 0xff);
}
int64_t CPU::ZP_Y()
{
    return 0x2000 | ((mem->Get(PC + 1) + Y) & 0xff);
}
int64_t CPU::IND()
{
    return mem->Get16(0x2000 | mem->Get(PC + 1));
}
int64_t CPU::IND_X()
{
    return mem->Get16(0x2000 | ((mem->Get(PC + 1) + X) & 0xff));
}
int64_t CPU::IND_Y()
{
    return ((mem->Get16(0x2000 | mem->Get(PC + 1)) + Y) & 0xffff);
}
int64_t CPU::ABS()
{
    return mem->Get16(PC + 1);
}
int64_t CPU::ABS_X()
{
    return (mem->Get16(PC + 1) + X) & 0xffff;
}
int64_t CPU::ABS_Y()
{
    return (mem->Get16(PC + 1) + Y) & 0xffff;
}
int64_t CPU::ABS_IND()
{
    return mem->Get16(mem->Get16(PC + 1));
}
int64_t CPU::ABS_X_IND()
{
    return mem->Get16((mem->Get16(PC + 1) + X) & 0xffff);
}
void CPU::SetNZFlag(int64_t data)
{
    P = (P & ~(NFlag | ZFlag)) | NZCacheTable[data];
}
int64_t CPU::PCH()
{
    return PC >> 8;
}
int64_t CPU::PCL()
{
    return PC & 0x00ff;
}
void CPU::toPCH(int64_t data)
{
    PC = (PC & 0x00ff) | (data << 8);
}
void CPU::toPCL(int64_t data)
{
    PC = (PC & 0xff00) | data;
}
