#ifndef IO_H
#define IO_H
#include <cstdint>
#include <SDL2/SDL.h>

class IO {
  public:
    int Keybord[5][4];
    int GamePad[5][4];

    int GamePadBuffer = 0x00;

  public:
    IO()
    {
        for (int i = 0; i < 5; i++) {
            Keybord[i][0] = 0xbf;
            Keybord[i][1] = 0xbf;
        }
    }

    uint64_t GetJoystick()
    {
        return Keybord[0][0];
    }
    void SetJoystick(int data)
    {
        Keybord[0][0] = Keybord[0][0] | 0x40;
    }
    void CheckKeyUpFunction(int keyCode)
    {
        switch (keyCode) {
            case SDLK_s:    // RUN 'S'
                UnsetButtonRUN(0);
                break;
            case SDLK_a:    // SELECT 'A'
                UnsetButtonSELECT(0);
                break;
            case SDLK_z:    // SHOT2 'Z'
                UnsetButtonSHOT2(0);
                break;
            case SDLK_x:    // SHOT1 'X'
                UnsetButtonSHOT1(0);
                break;

                // case SDLK_v:    // SHOT2 'V'
                //     UnsetButtonSHOT2(0);
                //     break;
                // case SDLK_b:    // SHOT1 'B'
                //     UnsetButtonSHOT1(0);
                //     break;

            case SDLK_LEFT:    // LEFT
                UnsetButtonLEFT(0);
                break;
            case SDLK_RIGHT:    // RIGHT
                UnsetButtonRIGHT(0);
                break;
            case SDLK_DOWN:    // DOWN
                UnsetButtonDOWN(0);
                break;
            case SDLK_UP:    // UP
                UnsetButtonUP(0);
                break;

                // case 71:    // SHOT6 'G'
                //     UnsetButtonSHOT6(0);
                //     break;
                // case 70:    // SHOT5 'F'
                //     UnsetButtonSHOT5(0);
                //     break;
                // case 68:    // SHOT4 'D'
                //     UnsetButtonSHOT4(0);
                //     break;
                // case 67:    // SHOT3 'C'
                //     UnsetButtonSHOT3(0);
                //     break;
        }
    }

    void CheckKeyDownFunction(int keyCode)
    {
        switch (keyCode) {
            case SDLK_s:    // RUN 'S'
                SetButtonRUN(0);
                break;
            case SDLK_a:    // SELECT 'A'
                SetButtonSELECT(0);
                break;
            case SDLK_z:    // SHOT2 'Z'
                SetButtonSHOT2(0);
                break;
            case SDLK_x:    // SHOT1 'X'
                SetButtonSHOT1(0);
                break;

                // case SDLK_v:    // SHOT2 'V'
                //     SetButtonSHOT2(0);
                //     break;
                // case SDLK_b:    // SHOT1 'B'
                //     SetButtonSHOT1(0);
                //     break;

            case SDLK_LEFT:    // LEFT
                SetButtonLEFT(0);
                break;
            case SDLK_RIGHT:    // RIGHT
                SetButtonRIGHT(0);
                break;
            case SDLK_DOWN:    // DOWN
                SetButtonDOWN(0);
                break;
            case SDLK_UP:    // UP
                SetButtonUP(0);
                break;

                // case SDLK_g:    // SHOT6 'G'
                //     SetButtonSHOT6(0);
                //     break;
                // case SDLK_f:    // SHOT5 'F'
                //     SetButtonSHOT5(0);
                //     break;
                // case SDLK_d:    // SHOT4 'D'
                //     SetButtonSHOT4(0);
                //     break;
                // case SDLK_c:    // SHOT3 'C'
                //     SetButtonSHOT3(0);
                //     break;
        }
    }

    void UnsetButtonRUN(int no)
    {
        Keybord[no][0] |= 0x08;
    }

    void UnsetButtonSELECT(int no)
    {
        Keybord[no][0] |= 0x04;
    }

    void UnsetButtonSHOT2(int no)
    {
        Keybord[no][0] |= 0x02;
    }

    void UnsetButtonSHOT1(int no)
    {
        Keybord[no][0] |= 0x01;
    }

    void UnsetButtonLEFT(int no)
    {
        Keybord[no][1] |= 0x08;
    }

    void UnsetButtonDOWN(int no)
    {
        Keybord[no][1] |= 0x04;
    }

    void UnsetButtonRIGHT(int no)
    {
        Keybord[no][1] |= 0x02;
    }

    void UnsetButtonUP(int no)
    {
        Keybord[no][1] |= 0x01;
    }

    void UnsetButtonSHOT6(int no)
    {
        Keybord[no][2] |= 0x08;
    }

    void UnsetButtonSHOT5(int no)
    {
        Keybord[no][2] |= 0x04;
    }

    void UnsetButtonSHOT4(int no)
    {
        Keybord[no][2] |= 0x02;
    }

    void UnsetButtonSHOT3(int no)
    {
        Keybord[no][2] |= 0x01;
    }

    void SetButtonRUN(int no)
    {
        Keybord[no][0] &= ~0x08;
    }

    void SetButtonSELECT(int no)
    {
        Keybord[no][0] &= ~0x04;
    }

    void SetButtonSHOT2(int no)
    {
        Keybord[no][0] &= ~0x02;
    }

    void SetButtonSHOT1(int no)
    {
        Keybord[no][0] &= ~0x01;
    }

    void SetButtonLEFT(int no)
    {
        Keybord[no][1] &= ~0x08;
    }

    void SetButtonDOWN(int no)
    {
        Keybord[no][1] &= ~0x04;
    }

    void SetButtonRIGHT(int no)
    {
        Keybord[no][1] &= ~0x02;
    }

    void SetButtonUP(int no)
    {
        Keybord[no][1] &= ~0x01;
    }

    void SetButtonSHOT6(int no)
    {
        Keybord[no][2] &= ~0x08;
    }

    void SetButtonSHOT5(int no)
    {
        Keybord[no][2] &= ~0x04;
    }

    void SetButtonSHOT4(int no)
    {
        Keybord[no][2] &= ~0x02;
    }

    void SetButtonSHOT3(int no)
    {
        Keybord[no][2] &= ~0x01;
    }
};
#endif
