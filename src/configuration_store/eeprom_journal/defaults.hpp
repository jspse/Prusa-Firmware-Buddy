#pragma once
#include "constants.hpp"
#include <module/temperature.h>
#include <config.h>
#include <eeprom.h>
#include <sound_enum.h>
#include <footer_eeprom.hpp>
#include <time_tools.hpp>
#include <filament.hpp>
#include <selftest_result.hpp>
#include <metric_config.h>
#include <option/development_items.h>

namespace eeprom_journal {

// Holds default constants so they can be referenced by store item. Placing these constants in another header where it's more meaningful is welcome. These defaults could be passed directly as template parameter to store items from gcc 11 onwards (and store items would accept them as value instead of as a const ref).
namespace defaults {
    // Variables without a distinct default values can use these shared ones
    inline constexpr bool bool_true { true };
    inline constexpr bool bool_false { false };

    inline constexpr float float_zero { 0.0f };
    inline constexpr uint8_t uint8_t_zero { 0 };
    inline constexpr uint16_t uint16_t_zero { 0 };
    inline constexpr uint32_t uint32_t_zero { 0 };

    // default values for variables that have distinct requirements
    inline constexpr float pid_nozzle_p {
#ifdef DEFAULT_Kp
        DEFAULT_Kp
#else
        0.0f
#endif
    };
    inline constexpr float pid_nozzle_i {
#ifdef DEFAULT_Ki
        scalePID_i(DEFAULT_Ki)
#else
        0.0f
#endif
    };
    inline constexpr float pid_nozzle_d {
#ifdef DEFAULT_Kd
        scalePID_d(DEFAULT_Kd)
#else
        0.0f
#endif
    };

    inline constexpr float pid_bed_p {
#ifdef DEFAULT_bedKp
        DEFAULT_bedKp
#else
        0.0f
#endif
    };
    inline constexpr float pid_bed_i {
#ifdef DEFAULT_bedKi
        scalePID_i(DEFAULT_bedKi)
#else
        0.0f
#endif
    };
    inline constexpr float pid_bed_d {
#ifdef DEFAULT_bedKd
        scalePID_d(DEFAULT_bedKd)
#else
        0.0f
#endif
    };

    inline constexpr std::array<char, LAN_HOSTNAME_MAX_LEN + 1> net_hostname { DEFAULT_HOST_NAME };
    inline constexpr int8_t lan_timezone { 1 };
    inline constexpr std::array<char, WIFI_MAX_SSID_LEN + 1> wifi_ap_ssid { "" };
    inline constexpr std::array<char, WIFI_MAX_PASSWD_LEN + 1> wifi_ap_password { "" };

    inline constexpr eSOUND_MODE sound_mode { eSOUND_MODE::UNDEF };
    inline constexpr uint8_t sound_volume { 5 };
    inline constexpr uint16_t language { 0xffff };
    inline constexpr uint32_t footer_setting { footer::eeprom::Encode(footer::DefaultItems) };
    inline constexpr uint32_t footer_draw_type { footer::ItemDrawCnf::Default() };
    inline constexpr std::array<char, PL_PASSWORD_SIZE> prusalink_password { "" };

    inline constexpr std::array<char, CONNECT_HOST_SIZE + 1> connect_host { "buddy-a.\x01\x01" }; // "Compressed" - this means buddy-a.connect.prusa3d.com.
    inline constexpr std::array<char, CONNECT_TOKEN_SIZE + 1> connect_token { "" };
    inline constexpr uint16_t connect_port { 443 };

    // Defaults for metrics
#if DEVELOPMENT_ITEMS()
    // Development build has metrics allowed
    inline constexpr MetricsAllow metrics_allow { MetricsAllow::All };
#else  /*DEVELOPMENT_ITEMS()*/
    // Production build need user to intentionally allow them
    inline constexpr MetricsAllow metrics_allow { MetricsAllow::None };
#endif /*DEVELOPMENT_ITEMS()*/

    inline constexpr bool crash_enabled {
#if (PRINTER_IS_PRUSA_MK4 || PRINTER_IS_PRUSA_MK3_5)
        false
#else
        true
#endif // (( PRINTER_IS_PRUSA_MK4) || ( PRINTER_IS_PRUSA_MK3_5))
    };

    inline constexpr int16_t crash_sens[2] =
#if ENABLED(CRASH_RECOVERY)
        CRASH_STALL_GUARD;
#else
        { 0, 0 };
#endif // ENABLED(CRASH_RECOVERY)

    inline constexpr int16_t crash_sens_x { crash_sens[0] };
    inline constexpr int16_t crash_sens_y { crash_sens[1] };

    static constexpr uint16_t crash_max_period[2] =
#if ENABLED(CRASH_RECOVERY)
        CRASH_MAX_PERIOD;
#else
        { 0, 0 };
#endif // ENABLED(CRASH_RECOVERY)

    inline constexpr uint16_t crash_max_period_x { crash_max_period[0] };
    inline constexpr uint16_t crash_max_period_y { crash_max_period[1] };

    inline constexpr bool crash_filter {
#if ENABLED(CRASH_RECOVERY)
        CRASH_FILTER
#else
        false
#endif // ENABLED(CRASH_RECOVERY)
    };

    inline constexpr time_format::TF_t time_format { time_format::TF_t::TF_24H };

    inline constexpr float loadcell_scale { 0.0192f };
    inline constexpr float loadcell_threshold_static { -125.f };
    inline constexpr float loadcell_hysteresis { 80.f };
    inline constexpr float loadcell_threshold_continuous { -40.f };

    inline constexpr int32_t extruder_fs_ref_value { std::numeric_limits<int32_t>::min() }; // min == will require calibration
    inline constexpr uint32_t extruder_fs_value_span {
#if (BOARD_IS_XBUDDY && defined LOVEBOARD_HAS_PT100)
        100
#elif (BOARD_IS_XLBUDDY)
        1000
#else
        350000
#endif
    };

    inline constexpr int32_t side_fs_ref_value { std::numeric_limits<int32_t>::min() }; // min == will require calibration
    inline constexpr uint32_t side_fs_value_span { 310 };

    inline constexpr uint16_t print_progress_time { 30 };

    inline constexpr DockPosition dock_position { 0, 0 };
    inline constexpr ToolOffset tool_offset { 0, 0, 0 };

    inline constexpr filament::Type filament_type { filament::Type::NONE };
    inline constexpr float nozzle_diameter {
#if PRINTER_IS_PRUSA_XL
        0.60f
#else
        0.40f
#endif
    };

    inline constexpr HWCheckSeverity hw_check_severity { HWCheckSeverity::Warning };
    inline constexpr SelftestResult selftest_result {};
    inline constexpr SelftestResult_pre_23 selftest_result_pre_23 {};

    inline constexpr Sheet sheet_0 { "Smooth1", 0.0f };
    inline constexpr Sheet sheet_1 { "Smooth2", eeprom_z_offset_uncalibrated };
    inline constexpr Sheet sheet_2 { "Textur1", eeprom_z_offset_uncalibrated };
    inline constexpr Sheet sheet_3 { "Textur2", eeprom_z_offset_uncalibrated };
    inline constexpr Sheet sheet_4 { "Custom1", eeprom_z_offset_uncalibrated };
    inline constexpr Sheet sheet_5 { "Custom2", eeprom_z_offset_uncalibrated };
    inline constexpr Sheet sheet_6 { "Custom3", eeprom_z_offset_uncalibrated };
    inline constexpr Sheet sheet_7 { "Custom4", eeprom_z_offset_uncalibrated };

    inline constexpr float default_axis_steps_flt[4] = DEFAULT_AXIS_STEPS_PER_UNIT;
    inline constexpr float axis_steps_per_unit_x { default_axis_steps_flt[0] * ((DEFAULT_INVERT_X_DIR == true) ? -1.f : 1.f) };
    inline constexpr float axis_steps_per_unit_y { default_axis_steps_flt[1] * ((DEFAULT_INVERT_Y_DIR == true) ? -1.f : 1.f) };
    inline constexpr float axis_steps_per_unit_z { default_axis_steps_flt[2] * ((DEFAULT_INVERT_Z_DIR == true) ? -1.f : 1.f) };
    inline constexpr float axis_steps_per_unit_e0 { default_axis_steps_flt[3] * ((DEFAULT_INVERT_E0_DIR == true) ? -1.f : 1.f) };
    inline constexpr uint16_t axis_microsteps_X_ { X_MICROSTEPS };
    inline constexpr uint16_t axis_microsteps_Y_ { Y_MICROSTEPS };
    inline constexpr uint16_t axis_microsteps_Z_ { Z_MICROSTEPS };
    inline constexpr uint16_t axis_microsteps_E0_ { E0_MICROSTEPS };
    inline constexpr uint16_t axis_rms_current_ma_X_ { X_CURRENT };
    inline constexpr uint16_t axis_rms_current_ma_Y_ { Y_CURRENT };
    inline constexpr uint16_t axis_rms_current_ma_Z_ { Z_CURRENT };
    inline constexpr uint16_t axis_rms_current_ma_E0_ { E0_CURRENT };
    inline constexpr float axis_z_max_pos_mm {
#ifdef DEFAULT_Z_MAX_POS
        DEFAULT_Z_MAX_POS
#else
        0
#endif
    };

} // namespace defaults

} // namespace eeprom_journal
