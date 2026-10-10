#include <assert.h>
#include "../boards/shields/prospector_scanner/src/scanner_host_protocol.h"

int main(void) {
    struct scanner_host_status s = {0};
    assert(scanner_host_parse("CODEX2 -1 1234 79", &s));
    assert(!s.codex_left_available && s.codex_tokens_available && s.codex_week_available);
    assert(s.codex_total_tokens == 1234 && s.codex_week_left_percent == 79);
    assert(scanner_host_parse("CODEX2 80 -1 -1", &s));
    assert(s.codex_left_available && !s.codex_tokens_available && !s.codex_week_available);
    assert(scanner_host_parse("CODEX2 -1 -1 -1", &s));
    assert(s.codex_available && !s.codex_tokens_available && !s.codex_left_available);
    assert(scanner_host_parse("CODEX 0 0", &s));
    assert(s.codex_left_available && s.codex_tokens_available && !s.codex_week_available);
    assert(scanner_host_parse("CODEX 80 4294967295 64", &s));
    assert(s.codex_tokens_available && s.codex_total_tokens == UINT32_MAX);
    const char *bad[] = {"CODEX2 101 0 50", "CODEX2 -2 0 50", "CODEX2 50 4294967296 50",
        "CODEX2 50 0", "CODEX2 50 0 50 1", "CODEX 50 ", "CODEX 50 0 ",
        "CODEX 50 0 x", "CODEX2 50 0 -10", "CODEX2 50 99999999999999999999999999 50",
        "CODEX 50 -1", "CODEX2  50 0 50"};
    for (unsigned i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        struct scanner_host_status before = s;
        assert(!scanner_host_parse(bad[i], &s));
        assert(memcmp(&s, &before, sizeof(s)) == 0);
    }
    return 0;
}
