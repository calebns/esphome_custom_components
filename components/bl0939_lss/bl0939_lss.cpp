#include "bl0939_lss.h"
#include "esphome/core/log.h"
#include <cinttypes>

namespace esphome {
namespace bl0939_lss {

static const char *const TAG = "bl0939_lss";

// https://www.belling.com.cn/media/file_object/bel_product/BL0939_lss/datasheet/BL0939_lss_V1.2_cn.pdf
// (unfortunately chinese, but the protocol can be understood with some translation tool)
static const uint8_t BL0939_lss_READ_COMMAND = 0x55;  // 0x5{A4,A3,A2,A1}
static const uint8_t BL0939_lss_FULL_PACKET = 0xAA;
static const uint8_t BL0939_lss_PACKET_HEADER = 0x55;

static const uint8_t BL0939_lss_WRITE_COMMAND = 0xA5;  // 0xA{A4,A3,A2,A1}
static const uint8_t BL0939_lss_REG_IA_FAST_RMS_CTRL = 0x10;
static const uint8_t BL0939_lss_REG_IB_FAST_RMS_CTRL = 0x1E;
static const uint8_t BL0939_lss_REG_MODE = 0x18;
static const uint8_t BL0939_lss_REG_SOFT_RESET = 0x19;
static const uint8_t BL0939_lss_REG_USR_WRPROT = 0x1A;
static const uint8_t BL0939_lss_REG_TPS_CTRL = 0x1B;

const uint8_t BL0939_lss_INIT[6][6] = {
    // Reset to default
    {BL0939_lss_WRITE_COMMAND, BL0939_lss_REG_SOFT_RESET, 0x5A, 0x5A, 0x5A, 0x33},
    // Enable User Operation Write
    {BL0939_lss_WRITE_COMMAND, BL0939_lss_REG_USR_WRPROT, 0x55, 0x00, 0x00, 0xEB},
    // 0x0100 = CF_UNABLE energy pulse, AC_FREQ_SEL 50Hz, RMS_UPDATE_SEL 800mS
    {BL0939_lss_WRITE_COMMAND, BL0939_lss_REG_MODE, 0x00, 0x10, 0x00, 0x32},
    // 0x47FF = Over-current and leakage alarm on, Automatic temperature measurement, Interval 100mS
    {BL0939_lss_WRITE_COMMAND, BL0939_lss_REG_TPS_CTRL, 0xFF, 0x47, 0x00, 0xF9},
    // 0x181C = Half cycle, Fast RMS threshold 6172
    {BL0939_lss_WRITE_COMMAND, BL0939_lss_REG_IA_FAST_RMS_CTRL, 0x1C, 0x18, 0x00, 0x16},
    // 0x181C = Half cycle, Fast RMS threshold 6172
    {BL0939_lss_WRITE_COMMAND, BL0939_lss_REG_IB_FAST_RMS_CTRL, 0x1C, 0x18, 0x00, 0x08}};

void BL0939_lss::loop() {
  DataPacket buffer;
  if (!this->available()) {
    return;
  }
  if (read_array((uint8_t *) &buffer, sizeof(buffer))) {
    if (validate_checksum(&buffer)) {
      received_package_(&buffer);
    }
  } else {
    ESP_LOGW(TAG, "Junk on wire. Throwing away partial message");
    while (read() >= 0)
      ;
  }
  esp_pm_lock_release(light_sleep_lock);
}

bool BL0939_lss::validate_checksum(const DataPacket *data) {
  uint8_t checksum = BL0939_lss_READ_COMMAND;
  // Whole package but checksum
  for (uint32_t i = 0; i < sizeof(data->raw) - 1; i++) {
    checksum += data->raw[i];
  }
  checksum ^= 0xFF;
  if (checksum != data->checksum) {
    ESP_LOGW(TAG, "BL0939_lss invalid checksum! 0x%02X != 0x%02X", checksum, data->checksum);
  }
  return checksum == data->checksum;
}

void BL0939_lss::update() {
  /* Release all lock recursively and aquire once */
  while (ESP_OK == esp_pm_lock_release(light_sleep_lock)) {}
  esp_pm_lock_acquire(light_sleep_lock);

  this->flush();
  this->write_byte(BL0939_lss_READ_COMMAND);
  this->write_byte(BL0939_lss_FULL_PACKET);
}

void BL0939_lss::setup() {
  esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "bl0939_lock", &light_sleep_lock);

  for (auto *i : BL0939_lss_INIT) {
    this->write_array(i, 6);
    delay(1);
  }
  this->flush();
}

void BL0939_lss::received_package_(const DataPacket *data) const {
  // Bad header
  if (data->frame_header != BL0939_lss_PACKET_HEADER) {
    ESP_LOGI(TAG, "Invalid data. Header mismatch: %d", data->frame_header);
    return;
  }

  float v_rms = (float) to_uint32_t(data->v_rms) / voltage_reference_;
  float ia_rms = (float) to_uint32_t(data->ia_rms) / current_reference_;
  float ib_rms = (float) to_uint32_t(data->ib_rms) / current_reference_;
  float a_watt = (float) to_int32_t(data->a_watt) / power_reference_;
  float b_watt = (float) to_int32_t(data->b_watt) / power_reference_;
  int32_t cfa_cnt = to_int32_t(data->cfa_cnt);
  int32_t cfb_cnt = to_int32_t(data->cfb_cnt);
  float a_energy_consumption = (float) cfa_cnt / energy_reference_;
  float b_energy_consumption = (float) cfb_cnt / energy_reference_;
  float total_energy_consumption = a_energy_consumption + b_energy_consumption;

  if (voltage_sensor_ != nullptr) {
    voltage_sensor_->publish_state(v_rms);
  }
  if (current_sensor_1_ != nullptr) {
    current_sensor_1_->publish_state(ia_rms);
  }
  if (current_sensor_2_ != nullptr) {
    current_sensor_2_->publish_state(ib_rms);
  }
  if (power_sensor_1_ != nullptr) {
    power_sensor_1_->publish_state(a_watt);
  }
  if (power_sensor_2_ != nullptr) {
    power_sensor_2_->publish_state(b_watt);
  }
  if (energy_sensor_1_ != nullptr) {
    energy_sensor_1_->publish_state(a_energy_consumption);
  }
  if (energy_sensor_2_ != nullptr) {
    energy_sensor_2_->publish_state(b_energy_consumption);
  }
  if (energy_sensor_sum_ != nullptr) {
    energy_sensor_sum_->publish_state(total_energy_consumption);
  }

  ESP_LOGV(TAG,
           "BL0939_lss: U %fV, I1 %fA, I2 %fA, P1 %fW, P2 %fW, CntA %" PRId32 ", CntB %" PRId32 ", ∫P1 %fkWh, ∫P2 %fkWh",
           v_rms, ia_rms, ib_rms, a_watt, b_watt, cfa_cnt, cfb_cnt, a_energy_consumption, b_energy_consumption);
}

void BL0939_lss::dump_config() {  // NOLINT(readability-function-cognitive-complexity)
  ESP_LOGCONFIG(TAG, "BL0939_lss:");
  LOG_SENSOR("", "Voltage", this->voltage_sensor_);
  LOG_SENSOR("", "Current 1", this->current_sensor_1_);
  LOG_SENSOR("", "Current 2", this->current_sensor_2_);
  LOG_SENSOR("", "Power 1", this->power_sensor_1_);
  LOG_SENSOR("", "Power 2", this->power_sensor_2_);
  LOG_SENSOR("", "Energy 1", this->energy_sensor_1_);
  LOG_SENSOR("", "Energy 2", this->energy_sensor_2_);
  LOG_SENSOR("", "Energy sum", this->energy_sensor_sum_);
}

uint32_t BL0939_lss::to_uint32_t(ube24_t input) { return input.h << 16 | input.m << 8 | input.l; }

int32_t BL0939_lss::to_int32_t(sbe24_t input) { return input.h << 16 | input.m << 8 | input.l; }

}  // namespace bl0939_lss
}  // namespace esphome
