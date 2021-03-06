#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "platform/audio.h"
#include "platform/gfx.h"
#include "platform/system.h"
#include "ui/config.h"
#include "ui/manager.h"
#include "ui/menu.h"

#include <3ds.h>
#include <ctrcommon/fs.hpp>
#include <ctrcommon/gpu.hpp>
#include <ctrcommon/ir.hpp>
#include <ctrcommon/platform.hpp>
#include <ctrcommon/socket.hpp>

gfxScreen_t currConsole;

PrintConsole* topConsole;
PrintConsole* bottomConsole;

bool systemInit() {
    if(!platformInit() || !gfxInit()) {
        return 0;
    }

    topConsole = (PrintConsole*) calloc(1, sizeof(PrintConsole));
    consoleInit(GFX_TOP, topConsole);

    bottomConsole = (PrintConsole*) calloc(1, sizeof(PrintConsole));
    consoleInit(GFX_BOTTOM, bottomConsole);

    gfxSetScreenFormat(GFX_TOP, GSP_BGR8_OES);
    gfxSetDoubleBuffering(GFX_TOP, true);

    consoleSelect(bottomConsole);
    currConsole = GFX_BOTTOM;

    audioInit();

    mgrInit();

    setMenuDefaults();
    readConfigFile();

    printf("GameYob 3DS\n\n");

    return true;
}

void systemExit() {
    mgrSave();
    mgrExit();

    audioCleanup();

    free(topConsole);
    free(bottomConsole);

    gfxCleanup();
    platformCleanup();

    exit(0);
}

void systemRun() {
    mgrSelectRom();
    while(true) {
        mgrRun();
    }
}

void systemCheckRunning() {
    if(!platformIsRunning()) {
        systemExit();
    }
}

int systemGetConsoleWidth() {
    PrintConsole* console = !gameScreen == 0 ? topConsole : bottomConsole;
    return console->consoleWidth;
}

int systemGetConsoleHeight() {
    PrintConsole* console = !gameScreen == 0 ? topConsole : bottomConsole;
    return console->consoleHeight;
}

void systemUpdateConsole() {
    gfxScreen_t screen = !gameScreen == 0 ? GFX_TOP : GFX_BOTTOM;
    if(currConsole != screen) {
        gfxScreen_t oldScreen = gameScreen == 0 ? GFX_TOP : GFX_BOTTOM;
        gfxSetScreenFormat(oldScreen, GSP_BGR8_OES);
        gfxSetDoubleBuffering(oldScreen, true);

        gpuClearScreens();

        currConsole = screen;
        gfxSetScreenFormat(screen, GSP_RGB565_OES);
        gfxSetDoubleBuffering(screen, false);

        gfxSwapBuffers();
        gspWaitForVBlank();

        u16* framebuffer = (u16*) gfxGetFramebuffer(screen, GFX_LEFT, NULL, NULL);
        PrintConsole* console = screen == GFX_TOP ? topConsole : bottomConsole;
        console->frameBuffer = framebuffer;
        consoleSelect(console);
    }
}

bool systemGetIRState() {
    return irGetState() == 1;
}

void systemSetIRState(bool state) {
    irSetState(state ? 1 : 0);
}

const std::string systemGetIP() {
    return inet_ntoa({socketGetHostIP()});
}

int systemSocketListen(u16 port) {
    return socketListen(port);
}

FILE* systemSocketAccept(int listeningSocket, std::string* acceptedIp) {
    return socketAccept(listeningSocket, acceptedIp);
}

FILE* systemSocketConnect(const std::string ipAddress, u16 port) {
    return socketConnect(ipAddress, port);
}