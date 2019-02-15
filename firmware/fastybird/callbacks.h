/*

FASTYBIRD MQTT MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT

#include <vector>

std::vector<fastybird_report_configuration_schema_callback_f> _fastybird_report_configuration_schema_callbacks;
std::vector<fastybird_report_configuration_callback_f> _fastybird_report_configuration_callbacks;
std::vector<fastybird_on_configure_callback_f> _fastybird_on_configure_callbacks;

std::vector<fastybird_channels_report_configuration_callback_f> _fastybird_channels_report_configuration_callbacks;

std::vector<fastybird_channels_report_direct_controls_callback_f> _fastybird_channels_report_direct_controls_callbacks;
std::vector<fastybird_channels_report_scheduler_callback_f> _fastybird_channels_report_scheduler_callbacks;

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void fastybirdReportConfigurationSchemaRegister(fastybird_report_configuration_schema_callback_f callback) {
    _fastybird_report_configuration_schema_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdReportConfigurationRegister(fastybird_report_configuration_callback_f callback) {
    _fastybird_report_configuration_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdOnConfigureRegister(fastybird_on_configure_callback_f callback) {
    _fastybird_on_configure_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdChannelsReportConfigurationRegister(fastybird_channels_report_configuration_callback_f callback) {
    _fastybird_channels_report_configuration_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdChannelsReportDirectControlsRegister(fastybird_channels_report_direct_controls_callback_f callback) {
    _fastybird_channels_report_direct_controls_callbacks.push_back(callback);
}

// -----------------------------------------------------------------------------

void fastybirdChannelsReportSchedulerRegister(fastybird_channels_report_scheduler_callback_f callback) {
    _fastybird_channels_report_scheduler_callbacks.push_back(callback);
}

#endif