
#include <pspctrl.h>
#include <pspgu.h>

#include "client/gui/screens/menu.h"
#include "client/gui/screens/screen.h"
#include "client/i18n.h"

struct CreditsScreen : Screen {
    void renderContent(MenuState& s);
    void handleInput(MenuState& s, unsigned int pressed, unsigned int held);
};

void CreditsScreen::handleInput(MenuState& s, unsigned int pressed, unsigned int) {
    if ((pressed & PSP_CTRL_CIRCLE) || (pressed & PSP_CTRL_CROSS)) {
        s.screen = SCREEN_TITLE;
    }
}

void CreditsScreen::renderContent(MenuState& s) {
    Font& font = s.font; bool haveFont = s.haveFont;
    if (!haveFont) return;

    sceGuDisable(GU_DEPTH_TEST);

    {
        float lb = 4.0f * MENU_PX + menuBarButtonW(s, T("Back", "Atras"));
        drawMenuHeader(s, T("Credits", "Creditos"), 0.0f, VW, MENU_BAR_H, MENU_BAR_TEXT, lb, VW - lb);
    }
    {
        float bw = menuBarButtonW(s, T("Back", "Atras"));
        menuBarButton(s, 4.0f * MENU_PX, bw, T("Back", "Atras"), false);
    }

    const char* line1 = "Created by dream";
    const char* line2 = "github.com/eyxgamer454-debug";

    float y = VH / 2.0f - 10.0f;
    float w1 = fontTextWidth(&font, line1) * MENU_PX;
    fontDrawTextShadow(&font, (VW - w1) / 2.0f, y, line1, 0xFFFFFFFFu, MENU_PX);

    float w2 = fontTextWidth(&font, line2) * MENU_PX;
    fontDrawTextShadow(&font, (VW - w2) / 2.0f, y + 14.0f, line2, 0xFFA0FFFFu, MENU_PX);

    sceGuEnable(GU_DEPTH_TEST);
}

static CreditsScreen s_creditsScreen;
Screen& creditsScreen() { return s_creditsScreen; }
