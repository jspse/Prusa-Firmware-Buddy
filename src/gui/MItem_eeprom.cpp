/**
 * @file MItem_eeprom.cpp
 * @author Radek Vana
 * @date 2021-09-22
 */
#include "MItem_eeprom.hpp"
#include "i2c.hpp"
#include "eeprom_loadsave.h"
#include "GuiDefaults.hpp"
#include "i18n.h"

static constexpr bool use_long_text = GuiDefaults::infoDefaultLen >= 16;
// dev only, don't translate
constexpr static const char *label_CRC_ERR = (use_long_text) ? "INIT_CRC_ERROR" : "I_CRC";
constexpr static const char *label_UPGRADED = (use_long_text) ? "INIT_UPGRADED" : "I_UPG";
constexpr static const char *label_UPGRADE_FAILED = (use_long_text) ? "INIT_UPG_FAILED" : "I_UPF";
constexpr static const char *label_TRANSMIT_OK = (use_long_text) ? "TRANSMIT_OK" : "T_OK";
constexpr static const char *label_TRANSMIT_ERROR = (use_long_text) ? "TRANSMIT_ERROR" : "T_ERROR";
constexpr static const char *label_TRANSMIT_BUSY = (use_long_text) ? "TRANSMIT_BUSY" : "T_BUSY";
constexpr static const char *label_TRANSMIT_TIMEOUT = (use_long_text) ? "TRANSMIT_TIMEOUT" : "R_TIMEOUT";
constexpr static const char *label_TRANSMIT_UNDEF = (use_long_text) ? "TRANSMIT_UNDEF" : "T_UNDEF";
constexpr static const char *label_RECEIVE_OK = (use_long_text) ? "RECEIVE_OK" : "R_OK";
constexpr static const char *label_RECEIVE_ERROR = (use_long_text) ? "RECEIVE_ERROR" : "R_ERROR";
constexpr static const char *label_RECEIVE_BUSY = (use_long_text) ? "RECEIVE_BUSY" : "R_BUSY";
constexpr static const char *label_RECEIVE_TIMEOUT = (use_long_text) ? "RECEIVE_TIMEOUT" : "R_TIMEOUT";
constexpr static const char *label_RECEIVE_UNDEF = (use_long_text) ? "RECEIVE_UNDEF" : "R_UNDEF";

// accessing extern variables from other thread
// uint32_t will be most likely atomic, if it is not it will not break anything because it is "read only"
// meant to be used in dev mode

MI_EEPROM_INIT_CRC_ERROR::MI_EEPROM_INIT_CRC_ERROR()
    : WI_INFO_DEV_t(0, string_view_utf8::MakeCPUFLASH((const uint8_t *)label_CRC_ERR)) { // TODO(ConfigStore) : Not migrated
}

MI_EEPROM_INIT_UPGRADED::MI_EEPROM_INIT_UPGRADED()
    : WI_INFO_DEV_t(0, string_view_utf8::MakeCPUFLASH((const uint8_t *)label_UPGRADED)) { // TODO(ConfigStore) : Not migrated
}

MI_EEPROM_INIT_UPGRADE_FAILED::MI_EEPROM_INIT_UPGRADE_FAILED()
    : WI_INFO_DEV_t(0, string_view_utf8::MakeCPUFLASH((const uint8_t *)label_UPGRADE_FAILED)) { // TODO(ConfigStore) : Not migrated
}

MI_I2C_TRANSMIT_RESULTS_HAL_OK::MI_I2C_TRANSMIT_RESULTS_HAL_OK()
    : WI_INFO_DEV_t(i2c::statistics::get_transmit_HAL_OK(), string_view_utf8::MakeCPUFLASH((const uint8_t *)label_TRANSMIT_OK)) {
}

MI_I2C_TRANSMIT_RESULTS_HAL_BUSY::MI_I2C_TRANSMIT_RESULTS_HAL_BUSY()
    : WI_INFO_DEV_t(i2c::statistics::get_transmit_HAL_BUSY(), string_view_utf8::MakeCPUFLASH((const uint8_t *)label_TRANSMIT_BUSY)) {
}

MI_I2C_RECEIVE_RESULTS_HAL_OK::MI_I2C_RECEIVE_RESULTS_HAL_OK()
    : WI_INFO_DEV_t(i2c::statistics::get_receive_HAL_OK(), string_view_utf8::MakeCPUFLASH((const uint8_t *)label_RECEIVE_OK)) {
}

MI_I2C_RECEIVE_RESULTS_HAL_BUSY::MI_I2C_RECEIVE_RESULTS_HAL_BUSY()
    : WI_INFO_DEV_t(i2c::statistics::get_receive_HAL_BUSY(), string_view_utf8::MakeCPUFLASH((const uint8_t *)label_RECEIVE_BUSY)) {
}
