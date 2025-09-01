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
constexpr const char* kPropVendorDevice = "ro.vendor.device";

void SetProperty(const std::string& prop, const std::string& value) {
    LOG(INFO) << "Setting property: " << prop << " to " << value;
    if (!android::base::SetProperty(prop, value)) {
        LOG(ERROR) << "Unable to set: " << prop << " to " << value;
    }
}

void DetectNfc() {
    std::string nfcChipset;
    if (!android::base::ReadFileToString(kProcNfcPath, &nfcChipset)) {
        LOG(ERROR) << "Failed to read file: " << kProcNfcPath;
        return;
    }

    if (nfcChipset != "NULL") {
        SetProperty(kPropNfcDevice, "1");
    }
}

void SetDevice() {
    std::string device, model, powerProfileDevice, operatorContent;
    if (!android::base::ReadFileToString(kProcOperatorPath,
                                         &operatorContent)) {
        LOG(ERROR) << "Failed to read file: " << kProcOperatorPath;
        return;
    }

    int operatorCode = stoi(operatorContent);

    switch (operatorCode) {
        case 101:
        case 102:
        case 104:
        case 105:
        case 106:
        case 113:
            device = "realme 6";
            break;
        case 111:
            device = "realme 6i";
            break;
        case 112:
            device = "realme Narzo";
            break;
        case 114:
            device = "realme 6s";
            break;
        case 94:
        case 140:
        case 141:
        case 142:
        case 146:
        case 148:
        case 149:
            device = "realme 7";
            powerProfileDevice = "RMX2151";
            break;
        case 90:
        case 92:
            device = "realme Narzo 30";
            powerProfileDevice = "RMX2151";
            break;
        case 143:
        case 145:
        case 147:
            device = "realme Narzo 20 Pro";
            powerProfileDevice = "RMX2161";
            break;
        default:
            LOG(ERROR) << "Unknown operator found: " << operatorCode;
    }

    if (!device.empty()) {
        SetProperty(kPropVendorDevice, device);
    }

    if (!powerProfileDevice.empty()) {
        SetProperty(kPropPowerProfile, powerProfileDevice);
    }
}

int main() {
    DetectNfc();
    SetDevice();

    return 0;
}
