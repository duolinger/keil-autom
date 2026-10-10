#include <REG52.H>

#define LED_STEP_INTERVAL_MS 200
#define KEY_DEBOUNCE_INTERVAL_MS 20
#define KEY_NOTE_DURATION_MS 180
#define COUNTER_MAX_VALUE 60

sbit DIGIT_1 = P2^0;
sbit DIGIT_2 = P2^1;
sbit DIGIT_3 = P2^2;
sbit DIGIT_4 = P2^3;
sbit KEY_1 = P3^5;
sbit KEY_2 = P3^4;
sbit KEY_3 = P3^3;
sbit KEY_4 = P3^2;
sbit BEEP = P3^6;

unsigned char code SEGMENT_CODE[10] = {
    0xC0, 0xF9, 0xA4, 0xB0, 0x99,
    0x92, 0x82, 0xF8, 0x80, 0x90
};

/* Timer 1 reload values for C5, D5, E5, and G5 at 11.0592 MHz, 12T. */
unsigned int code PIANO_NOTE_RELOAD[4] = {
    0xFC90, 0xFCEF, 0xFD45, 0xFDB4
};

unsigned char volatile display_code[4] = {0xFF, 0xFF, 0xFF, 0xF9};
unsigned char volatile scan_position;
unsigned char volatile led_tick;
unsigned char volatile led_position;
unsigned char volatile led_direction = 1;
unsigned char volatile led_step_pending;
unsigned char volatile key_sample_tick;
unsigned char volatile key_sample_pending;
unsigned char key_history[4] = {0x03, 0x03, 0x03, 0x03};
unsigned char key_stable_state = 0x0F;
unsigned char volatile beep_remaining_ms;
unsigned char volatile beep_reload_high = 0xFC;
unsigned char volatile beep_reload_low = 0x90;

void update_display(unsigned char seconds)
{
    unsigned char tens_code = 0xFF;
    unsigned char units_code = SEGMENT_CODE[seconds % 10];
    unsigned char tens = seconds / 10;

    if (tens != 0) {
        tens_code = SEGMENT_CODE[tens];
    }

    EA = 0;
    display_code[0] = 0xFF;
    display_code[1] = 0xFF;
    display_code[2] = tens_code;
    display_code[3] = units_code;
    EA = 1;
}

unsigned char scan_keys(void)
{
    unsigned char raw_state = 0;
    unsigned char pressed_events = 0;
    unsigned char i;

    if (KEY_1) raw_state |= 0x01;
    if (KEY_2) raw_state |= 0x02;
    if (KEY_3) raw_state |= 0x04;
    if (KEY_4) raw_state |= 0x08;

    for (i = 0; i < 4; i++) {
        key_history[i] = ((key_history[i] << 1) | ((raw_state >> i) & 0x01)) & 0x03;
        if (key_history[i] == 0x00 && (key_stable_state & (1u << i))) {
            key_stable_state &= (unsigned char)~(1u << i);
            pressed_events |= (unsigned char)(1u << i);
        } else if (key_history[i] == 0x03) {
            key_stable_state |= (unsigned char)(1u << i);
        }
    }

    return pressed_events;
}

void timer0_isr(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x66;

    DIGIT_1 = 1;
    DIGIT_2 = 1;
    DIGIT_3 = 1;
    DIGIT_4 = 1;
    P0 = display_code[scan_position];

    switch (scan_position) {
    case 0:
        DIGIT_1 = 0;
        break;
    case 1:
        DIGIT_2 = 0;
        break;
    case 2:
        DIGIT_3 = 0;
        break;
    default:
        DIGIT_4 = 0;
        break;
    }

    scan_position = (scan_position + 1) & 0x03;

    key_sample_tick++;
    if (key_sample_tick >= (KEY_DEBOUNCE_INTERVAL_MS / 2)) {
        key_sample_tick = 0;
        key_sample_pending = 1;
    }

    if (beep_remaining_ms != 0) {
        beep_remaining_ms--;
        if (beep_remaining_ms == 0) {
            ET1 = 0;
            TR1 = 0;
            BEEP = 1;
        }
    }

    led_tick++;
    if (led_tick >= LED_STEP_INTERVAL_MS) {
        led_tick = 0;
        if (led_direction) {
            if (led_position == 7) {
                led_direction = 0;
            } else {
                led_position++;
            }
        } else if (led_position == 0) {
            led_direction = 1;
        } else {
            led_position--;
        }
        P1 = (unsigned char)~(1u << led_position);
        led_step_pending = 1;
    }
}

void timer1_isr(void) interrupt 3
{
    TH1 = beep_reload_high;
    TL1 = beep_reload_low;
    BEEP = !BEEP;
}

void main(void)
{
    unsigned char seconds = 1;
    unsigned char key_events;
    unsigned char note_index;
    unsigned int note_reload;

    P1 = 0xFE;
    P3 = 0xFF;
    BEEP = 1;
    update_display(seconds);

    TMOD = (TMOD & 0x00) | 0x11;
    TH0 = 0xFC;
    TL0 = 0x66;
    TH1 = 0xFF;
    TL1 = 0x1A;
    ET0 = 1;
    ET1 = 0;
    TR0 = 1;
    TR1 = 0;
    EA = 1;

    while (1) {
        if (led_step_pending) {
            EA = 0;
            led_step_pending = 0;
            EA = 1;

            if (seconds >= COUNTER_MAX_VALUE) {
                seconds = 1;
            } else {
                seconds++;
            }
            update_display(seconds);
        }

        if (key_sample_pending) {
            EA = 0;
            key_sample_pending = 0;
            EA = 1;

            key_events = scan_keys();
            if (key_events != 0) {
                if (seconds >= COUNTER_MAX_VALUE) {
                    seconds = 1;
                } else {
                    seconds++;
                }
                update_display(seconds);

                if (key_events & 0x01) {
                    note_index = 0;
                } else if (key_events & 0x02) {
                    note_index = 1;
                } else if (key_events & 0x04) {
                    note_index = 2;
                } else {
                    note_index = 3;
                }

                note_reload = PIANO_NOTE_RELOAD[note_index];
                ET1 = 0;
                TR1 = 0;
                beep_reload_high = (unsigned char)(note_reload >> 8);
                beep_reload_low = (unsigned char)note_reload;
                beep_remaining_ms = KEY_NOTE_DURATION_MS;
                BEEP = 0;
                TH1 = beep_reload_high;
                TL1 = beep_reload_low;
                TF1 = 0;
                ET1 = 1;
                TR1 = 1;
            }
        }
    }
}
