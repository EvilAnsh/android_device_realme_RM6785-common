#
# Copyright (C) 2023-2025 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/non_ab_device.mk)

# Inherit from device makefile
$(call inherit-product, device/realme/nemo/device.mk)

# Inherit some common LineageOS stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := lineage_nemo
PRODUCT_BRAND := realme
PRODUCT_MODEL := RMX2001L1
PRODUCT_DEVICE := nemo
PRODUCT_MANUFACTURER := realme

# Evolution specific flags
EVO_BUILD_TYPE := Official
TARGET_BUILD_APERTURE_CAMERA := false
TARGET_USES_MINI_GAPPS := true

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildFingerprint=realme/RMX2001/RMX2001L1:10/QP1A.190711.020/1594211000:user/release-keys

PRODUCT_GMS_CLIENTID_BASE := android-oppo
