#
# Copyright (C) 2024 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# AudioFX
TARGET_EXCLUDES_AUDIOFX := true

# Health
PRODUCT_PACKAGES += \
    vendor.lineage.health-service.default

# Overlays
DEVICE_PACKAGE_OVERLAYS += \
    $(LOCAL_PATH)/overlay-lineage

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += \
    $(LOCAL_PATH)/hidl/touch

# Touch
PRODUCT_PACKAGES += \
    vendor.lineage.touch@1.0-service.RM6785
