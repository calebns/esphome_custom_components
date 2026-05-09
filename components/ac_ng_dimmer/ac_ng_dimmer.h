#pragma once

#include "esphome.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/output/float_output.h"

namespace esphome::ac_ng_dimmer {

class AcNgDimmer : public Component, public output::FloatOutput {
public:
    void setup() override {
        // Konfiguracja Timer LEDC
        ledc_timer_config_t ledc_timer = {};
        ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;
        ledc_timer.timer_num        = LEDC_TIMER_0;
        ledc_timer.duty_resolution  = LEDC_TIMER_13_BIT; // 0-8191
        ledc_timer.freq_hz          = 100;               // 100Hz dla 50Hz sieci
        ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
        ledc_timer_config(&ledc_timer);

        // Konfiguracja Kanału LEDC
        ledc_channel_config_t ledc_channel = {};
        ledc_channel.speed_mode     = LEDC_LOW_SPEED_MODE;
        ledc_channel.channel        = LEDC_CHANNEL_0;
        ledc_channel.timer_sel      = LEDC_TIMER_0;
        ledc_channel.intr_type      = LEDC_INTR_DISABLE;
        ledc_channel.gpio_num       = GPIO_NUM_2; // TRIAC_PIN
        ledc_channel.duty           = 8191;        // Startujemy od wyłączonego (stan 1)
        ledc_channel.hpoint         = 0;
        ledc_channel_config(&ledc_channel);

        // Konfiguracja ZCD (Zero Cross)
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_NEGEDGE;
        io_conf.pin_bit_mask = (1ULL << GPIO_NUM_10); // ZCD_PIN
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&io_conf);

        gpio_install_isr_service(0);
        gpio_isr_handler_add(GPIO_NUM_10, zcd_isr, (void*)this);
    }

    static void IRAM_ATTR zcd_isr(void* arg) {
        // KLUCZ: Resetujemy sprzętowy timer LEDC dokładnie w momencie przejścia przez zero
        ledc_timer_rst(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0);
    }

    void write_state(float state) override {
        if (state < 0.01f) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8191);
        } else if (state > 0.99f) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
        } else {
            // Im wyższa moc, tym mniejszy duty (wcześniejsze odpalenie triaka zerem)
            uint32_t duty = (uint32_t)((1.0f - state) * 8191.0f);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        }
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
};

}