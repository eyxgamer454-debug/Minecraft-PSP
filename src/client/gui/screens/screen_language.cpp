
#include <pspctrl.h>
#include <pspgu.h>

#include "client/gui/screens/menu.h"
#include "client/gui/screens/screen.h"
#include "client/i18n.h"

struct LanguageScreen : Screen {
    void renderContent(MenuState& s);
    void handleInput(MenuState& s, unsigned int pressed, unsigned int held);
};

void LanguageScreen::handleInput(MenuState& s, unsigned int pressed, unsigned int) {
    AppScreen& screen = s.screen;
    int& sel = s.langSelected;

    if ((pressed & PSP_CTRL_LEFT) || (pressed & PSP_CTRL_UP))   sel = 0;
    if ((pressed & PSP_CTRL_RIGHT) || (pressed & PSP_CTRL_DOWN)) sel = 1;

    if (pressed & PSP_CTRL_CROSS) {
        g_language = sel;
        langSave();
        screen = SCREEN_TITLE;
    }
    if (pressed & PSP_CTRL_CIRCLE) {
        screen = SCREEN_TITLE;
    }
}

void LanguageScreen::renderContent(MenuState& s) {
    Font& font = s.font; bool haveFont = s.haveFont;
    if (!haveFont) return;

    sceGuDisable(GU_DEPTH_TEST);

    {
        float lb = 4.0f * MENU_PX + menuBarButtonW(s, T("Back", "Atras"));
        drawMenuHeader(s, T("Choose Language", "Elegir Idioma"), 0.0f, VW, MENU_BAR_H, MENU_BAR_TEXT, lb, VW - lb);
    }
    {
        float bw = menuBarButtonW(s, T("Back", "Atras"));
        menuBarButton(s, 4.0f * MENU_PX, bw, T("Back", "Atras"), false);
    }

    const float btnW = 90.0f, btnH = 22.0f, gap = 10.0f;
    float totalW = btnW * 2.0f + gap;
    float x0 = (VW - totalW) / 2.0f;
    float y  = VH / 2.0f - btnH / 2.0f;

    guiTButtonLabel(s, x0,               y, btnW, btnH, "English",
                    s.langSelected == 0, g_language == 0, MENU_BAR_TEXT);
    guiTButtonLabel(s, x0 + btnW + gap,  y, btnW, btnH, "Espanol",
                    s.langSelected == 1, g_language == 1, MENU_BAR_TEXT);

    const char* hint = T("Cross: select   Circle: back", "X: elegir   O: atras");
    float hw = fontTextWidth(&font, hint) * MENU_PX;
    fontDrawTextShadow(&font, (VW - hw) / 2.0f, y + btnH + 14.0f, hint, 0xFFBBBBBBu, MENU_PX);

    sceGuEnable(GU_DEPTH_TEST);
}

static LanguageScreen s_languageScreen;
Screen& languageScreen() { return s_languageScreen; }
