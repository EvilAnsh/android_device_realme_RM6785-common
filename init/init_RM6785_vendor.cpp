//
// Copyright (C) 2022-2023 The LineageOS Project
//
// SPDX-License-Identifier: Apache-2.0
//

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>

constexpr const char* kProcNfcPath = "/proc/oppo_nfc/chipset";
constexpr const char* kProcOperatorPath = "/proc/oppoVersion/operatorName";
constexpr const char* kPropPowerProfile = "ro.vendor.power_profile.device";
constexpr const char* kPropNfcDevice = "ro.vendor.nfc_device";

void set_property(const std::string& prop, const std::string& value) {
    LOG(INFO) << "Setting property: " << prop << " to " << value;
    if (!android::base::SetProperty(prop, value)) {
        LOG(ERROR) << "Unable to set: " << prop << " to " << value;
    }
}

void detect_nfc() {
    std::string nfc_chipset;
    if (!android::base::ReadFileToString(kProcNfcPath, &nfc_chipset)) {
        LOG(ERROR) << "Failed to read file: " << kProcNfcPath;
        return;
    }

    if (nfc_chipset != "NULL") {
        set_property(kPropNfcDevice, "1");
    }
}

void set_power_profile() {
    std::string device_name, operator_content;
    if (!android::base::ReadFileToString(kProcOperatorPath,
                                         &operator_content)) {
        LOG(ERROR) << "Failed to read file: " << kProcOperatorPath;
        return;
    }

    int operator_code = stoi(operator_content);

    switch (operator_code) {
        case 90:
        case 92:
        case 94:
        case 140:
        case 141:
        case 142:
        case 146:
        case 148:
        case 149:
            device_name = "RMX2151";
            break;
        case 143:
        case 145:
        case 147:
            device_name = "RMX2161";
            break;
        default:
            LOG(ERROR) << "Unknown operator found: " << operator_code;
    }

    if (!device_name.empty()) {
        set_property(kPropPowerProfile, device_name);
    }
}

int main() {
    detect_nfc();
    set_power_profile();

    return 0;
}
