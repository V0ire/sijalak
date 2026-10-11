#include "boundary.h"
#include <cstdlib>
#include <iostream>
extern "C" void app_main(void);
int main() {
    int checks = 0;
    for (int scenario = 0; scenario < 4; ++scenario) {
        boundary::reset(); boundary::begin_ok = scenario != 1;
        boundary::write_ok = scenario != 2; boundary::touch_ok = scenario != 3;
        try { app_main(); } catch (boundary::Finished&) {}
        const bool ok = boundary::display_begin == 1 && boundary::touch_begin == 1 &&
            boundary::delays == 10 && boundary::polls == (scenario == 3 ? 0 : 10) &&
            boundary::writes == (scenario == 1 ? 0 : scenario == 2 ? 1 : 10) &&
            // The local demo is allowed to use SD + I2S TX only.
            boundary::sd_begin == 1 && boundary::i2s_begin == 1 &&
            // ...but never the microphone, Wi-Fi, or HTTP (no network/mic path).
            boundary::i2s == 0 && boundary::wifi == 0 && boundary::http == 0 &&
            boundary::mic == 0;
        if (!ok) return 1;
        ++checks;
    }
    std::cout << checks << " default-entrypoint stub scenarios passed (NOT IDF validation)\n";
}
