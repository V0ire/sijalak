#pragma once
#ifndef TEST_ACTIVE_HIGH_SH1106
#define CONFIG_JALAK_TOUCH_ACTIVE_LOW 1
#else
#define CONFIG_JALAK_OLED_SH1106 1
#endif
#define CONFIG_JALAK_OLED_ADDRESS 0x3c
// SH1106 intentionally undefined, matching a disabled IDF bool.
