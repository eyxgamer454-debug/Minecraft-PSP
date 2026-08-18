
#include <pspctrl.h>
#include <pspgu.h>

#include "client/gui/screens/menu.h"
#include "client/gui/screens/screen.h"
#include "gpu/sprite.h"
#include "gpu/widgets.h"

#include <cmath>
#include <ctime>
#include <pspkernel.h>

#include "client/gui/screens/splashes.h"
#include "client/i18n.h"

static int s_splash = -1;

// Classic vertical menu: Singleplayer / Multiplayer / (Options | Quit Game).
// selected: 0=Singleplayer 1=Multiplayer 2=Options 3=Quit
static const float listW  = 210.0f;
static const float rowH   = 20.0f;
static const float rowGap = 5.0f;
static const float listX  = (VW - listW) / 2.0f;
static const float listY0 = 2.0f + VH / 3.0f;

struct TitleScreen : Screen {
    void renderContent(MenuState& s);
    void handleInput(MenuState& s, unsigned int pressed, unsigned int held);
};

void TitleScreen::handleInput(MenuState& s, unsigned int pressed, unsigned int ) {

    int& selected = s.selected;
    AppScreen& screen = s.screen;
    char (&statusMsg)[128] = s.statusMsg;
    int& optFocus = s.optFocus;
    int& optTabHighlight = s.optTabHighlight;
    int& optItemHighlight = s.optItemHighlight;
    int& optCategory = s.optCategory;

    if (pressed & PSP_CTRL_DOWN) {
        if (selected < 0)      selected = 0;
        else if (selected == 0) selected = 1;
        else                     selected = 2;
    }
    if (pressed & PSP_CTRL_UP) {
        if (selected < 0)      selected = 0;
        else if (selected >= 2) selected = 1;
        else if (selected == 1) selected = 0;
        else                     selected = 2;
    }
    if (pressed & PSP_CTRL_LEFT)  { if (selected == 3) selected = 2; }
    if (pressed & PSP_CTRL_RIGHT) { if (selected == 2) selected = 3; }

    if ((pressed & PSP_CTRL_CROSS) && selected >= 0) {
        if (selected == 0) {
            screen = SCREEN_WORLDS;
            statusMsg[0] = '\0';
        } else if (selected == 1) {
            joinListReset(s);
            screen = SCREEN_JOIN;
            statusMsg[0] = '\0';
        } else if (selected == 2) {
            optFocus = 1;
            optTabHighlight = optCategory;
            optItemHighlight = 0;
            screen = SCREEN_OPTIONS;
            statusMsg[0] = '\0';
        } else if (selected == 3) {
            requestGameExit();
        }
    }

    if (pressed & PSP_CTRL_SQUARE) {
        screen = SCREEN_CREDITS;
        statusMsg[0] = '\0';
    }
    if (pressed & PSP_CTRL_TRIANGLE) {
        s.langSelected = g_language;
        screen = SCREEN_LANGUAGE;
        statusMsg[0] = '\0';
    }
}

void TitleScreen::renderContent(MenuState& s) {
    Font& font = s.font; bool haveFont = s.haveFont;
    Texture& logo = s.logo; bool haveLogo = s.haveLogo;
    int& selected = s.selected;

    const float LOGO_SCALE = 1.15f;
    float logoYV = 4.0f;

    float logoWV = ((VW < (float)logo.realW) ? VW : (float)logo.realW) * LOGO_SCALE;
    float logoHV = logoWV / (float)logo.realW * (float)logo.realH;
    float logoXV = (VW - logoWV) / 2.0f;
    if (haveLogo) {
        textureBind(&logo);
        sceGuDisable(GU_DEPTH_TEST);
        spriteDraw(&logo, logoXV * UI_SCALE, logoYV * UI_SCALE,
                  logoWV * UI_SCALE, logoHV * UI_SCALE,
                  0, 0, (float)logo.realW, (float)logo.realH, WHITE);
        sceGuEnable(GU_DEPTH_TEST);
    }

    if (haveFont) {

        if (s_splash < 0) {
            unsigned seed = (unsigned)time(0) * 2654435761u + sceKernelGetSystemTimeLow();
            s_splash = (int)(seed % (unsigned)kSplashCount);
        }
        const char* splash = kSplashes[s_splash];

        float t = (float)sceKernelGetSystemTimeLow() * 1e-6f;
        float scale = powf(sinf(t * 3.14f * 2.3f), 4.0f) * 0.06f + 1.3f;

        float len = (float)fontTextWidth(&font, splash);
        float fit = (VW * 0.3125f) / (len * 1.3f);
        if (fit > 1.0f) fit = 1.0f;

        sceGuDisable(GU_DEPTH_TEST);
        fontDrawTransformed(&font, (logoXV + logoWV) * 0.71f * UI_SCALE,
                            (logoYV + logoHV - 15.0f) * UI_SCALE,
                            splash, 0xFF00FFFFu ,
                            -20.0f, scale * fit * UI_SCALE, true);
        sceGuEnable(GU_DEPTH_TEST);
    }

    if (haveFont) {
        sceGuDisable(GU_DEPTH_TEST);

        const float halfW = (listW - rowGap) / 2.0f;

        guiTButton(s, listX, listY0, listW, rowH, selected == 0);
        guiTButtonLabel(s, listX, listY0, listW, rowH,
                        T("Singleplayer", "Un Jugador"), selected == 0, true, MENU_BAR_TEXT);

        guiTButton(s, listX, listY0 + (rowH + rowGap), listW, rowH, selected == 1);
        guiTButtonLabel(s, listX, listY0 + (rowH + rowGap), listW, rowH,
                        T("Multiplayer", "Multijugador"), selected == 1, true, MENU_BAR_TEXT);

        float row3Y = listY0 + 2.0f * (rowH + rowGap);
        guiTButton(s, listX, row3Y, halfW, rowH, selected == 2);
        guiTButtonLabel(s, listX, row3Y, halfW, rowH,
                        T("Options...", "Opciones..."), selected == 2, true, MENU_BAR_TEXT);
        guiTButton(s, listX + halfW + rowGap, row3Y, halfW, rowH, selected == 3);
        guiTButtonLabel(s, listX + halfW + rowGap, row3Y, halfW, rowH,
                        T("Quit Game", "Salir"), selected == 3, true, MENU_BAR_TEXT);

        float cbw = 60.0f, cbh = 16.0f;
        guiTButton(s, VW - cbw - 6.0f, VH - cbh - 6.0f, cbw, cbh, false);
        guiTButtonLabel(s, VW - cbw - 6.0f, VH - cbh - 6.0f, cbw, cbh,
                        T("Credits", "Creditos"), false, true, MENU_BAR_TEXT);
        guiTButton(s, 6.0f, VH - cbh - 6.0f, cbw, cbh, false);
        guiTButtonLabel(s, 6.0f, VH - cbh - 6.0f, cbw, cbh,
                        T("Language", "Idioma"), false, true, MENU_BAR_TEXT);

        sceGuEnable(GU_DEPTH_TEST);
    }

    if (haveFont) {
        sceGuDisable(GU_DEPTH_TEST);
        const char* copyright = "\xffMojang AB";
        float cw = fontTextWidth(&font, copyright) * UI_SCALE;
        fontDrawTextShadow(&font, 480.0f - cw - 4.0f, 272.0f - 9.0f * UI_SCALE, copyright, WHITE, UI_SCALE);
        sceGuEnable(GU_DEPTH_TEST);
    }
}

static TitleScreen s_titleScreen;
Screen& titleScreen() { return s_titleScreen; }
