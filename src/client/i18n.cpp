#include "client/i18n.h"
#include "platform/path.h"

#include <cstdio>

int g_language = 0;

void langLoad() {
    FILE* f = fopen(assetPath("language.txt"), "r");
    if (!f) return;
    int v = 0;
    if (fscanf(f, "%d", &v) == 1) g_language = (v == 1) ? 1 : 0;
    fclose(f);
}

void langSave() {
    FILE* f = fopen(assetPath("language.txt"), "w");
    if (!f) return;
    fprintf(f, "%d\n", g_language);
    fclose(f);
}
