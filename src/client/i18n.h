#ifndef MCPSP_CLIENT_I18N_H
#define MCPSP_CLIENT_I18N_H

// Minimal, call-site translation helper — not a keyed lookup table.
// g_language: 0 = English, 1 = Español.
extern int g_language;

inline const char* T(const char* en, const char* es) {
    return g_language ? es : en;
}

void langLoad();
void langSave();

#endif
