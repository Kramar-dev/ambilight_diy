#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 138
#define DI_PIN 2
#define OFF_TIME 1        //seconds, auto off timer
#define CURRENT_LIMIT 0   //mAh
#define serialRate 500000 //baudrate

int new_bright, new_bright_f;
unsigned long bright_timer, off_timer;

uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;
CRGB leds[NUM_LEDS];
boolean led_state = true;

void setup()
{
    FastLED.addLeds<WS2812, DI_PIN, GRB>(leds, NUM_LEDS);
    if (CURRENT_LIMIT > 0)
        FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
    Serial.begin(serialRate);
    Serial.print("Ada\n");
}

void check_connection()
{
    if (led_state)
    {
        if (millis() - off_timer > (OFF_TIME * 1000))
        {
            led_state = false;
            FastLED.clear();
            FastLED.show();
        }
    }
}

void loop()
{
    if (!led_state)
        led_state = true;
    off_timer = millis();

    for (i = 0; i < sizeof prefix; ++i)
    {
    waitLoop:
        while (!Serial.available())
            check_connection();
        ;
        if (prefix[i] == Serial.read())
            continue;
        i = 0;
        goto waitLoop;
    }

    while (!Serial.available())
        check_connection();
    ;
    hi = Serial.read();
    while (!Serial.available())
        check_connection();
    ;
    lo = Serial.read();
    while (!Serial.available())
        check_connection();
    ;
    chk = Serial.read();
    if (chk != (hi ^ lo ^ 0x55))
    {
        i = 0;
        goto waitLoop;
    }

    memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        byte r, g, b;
        while (!Serial.available())
            check_connection();
        r = Serial.read();
        while (!Serial.available())
            check_connection();
        g = Serial.read();
        while (!Serial.available())
            check_connection();
        b = Serial.read();
        leds[i].r = r;
        leds[i].g = g;
        leds[i].b = b;
    }
    FastLED.show();
}
