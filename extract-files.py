#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/realme/nemo',
    'hardware/mediatek',
    'hardware/mediatek/libaedv',
    'hardware/mediatek/libmtkperf_client',
]


def lib_fixup_odm_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'odm' else None


def lib_fixup_vendor_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'vendor' else None


lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
    ('vendor.oplus.hardware.biometrics.fingerprint@2.1',): lib_fixup_odm_suffix,
    ('vendor.mediatek.hardware.videotelephony@1.0',): lib_fixup_vendor_suffix,
}

blob_fixups: blob_fixups_user_type = {
    'product/etc/permissions/com.android.hotwordenrollment.common.util.xml': blob_fixup()
        .regex_replace('/my_product', '/product'),
    'system_ext/lib64/libimsma.so': blob_fixup()
        .replace_needed('libsink.so', 'libsink-mtk.so'),
    'system_ext/lib64/libsource.so': blob_fixup()
        .add_needed('libui_shim.so'),
    ('vendor/lib/libnvram.so',
    'vendor/lib/libsysenv.so',
    'vendor/lib64/libnvram.so',
    'vendor/lib64/libsysenv.so',
    'vendor/bin/hw/android.hardware.neuralnetworks@1.3-service-mtk-neuron'): blob_fixup()
         .add_needed('libbase_shim.so'),
    'vendor/bin/hw/mtkfusionrild': blob_fixup()
        .add_needed('libutils-v32.so'),
    ('vendor/bin/mnld', 'vendor/lib/libcam.utils.sensorprovider.so', 'vendor/lib/libaalservice.so', 'vendor/lib64/libcam.utils.sensorprovider.so', 'vendor/lib64/libaalservice.so'): blob_fixup()
        .add_needed('android.hardware.sensors@1.0-convert-shared.so'),
    'vendor/lib64/hw/android.hardware.camera.provider@2.6-impl-mediatek.so': blob_fixup()
        .add_needed('libcamera_metadata_shim.so'),
    ('vendor/lib64/libmtkcam_stdutils.so', 'vendor/lib64/hw/dfps.mt6785.so', 'vendor/lib64/hw/vendor.mediatek.hardware.pq@2.6-impl.so'): blob_fixup()
        .replace_needed('libutils.so', 'libutils-v30.so'),
    'vendor/lib64/libmtkcam_featurepolicy.so': blob_fixup()
        .binary_regex_replace(b'\x34\xE8\x87\x40\xB9', b'\x34\x28\x02\x80\x52'),
    'vendor/lib64/lib3a.flash.so': blob_fixup()
         .add_needed('liblog.so'),
    'vendor/lib64/libcam.halsensor.so': blob_fixup()
         .add_needed('libshim_utils.so'),
    'vendor/lib64/libutils-v30.so': blob_fixup()
         .add_needed('libprocessgroup_shim.so'),
    'vendor/lib/hw/audio.primary.mt6785.so': blob_fixup()
        .add_needed('libshim_audio.so')
        .replace_needed('libalsautils.so','libalsautils-v30.so'),
    'vendor/lib/libMtkOmxCore.so': blob_fixup()
        .binary_regex_replace(b'mtk.vendor.omx.core.log', b'ro.vendor.mtk.omx.log\x00\x00'),
    'vendor/lib/libMtkOmxVdecEx.so': blob_fixup()
        .replace_needed('libui.so','libui-v32.so')
        .binary_regex_replace(b'ro.mtk_crossmount_support', b'ro.vendor.mtk_crossmount\x00')
        .binary_regex_replace(b'ro.mtk_deinterlace_support', b'ro.vendor.mtk_deinterlace\x00'),
    'vendor/lib/libaudio_param_parser-vnd.so': blob_fixup()
        .binary_regex_replace(b'\x00audio.tuning.def_path', b'\x00ro.vendor.tuning_path')
        .binary_regex_replace(b'\x20audio.tuning.def_path', b'\x20ro.vendor.tuning_path'),
    'vendor/lib/libmnl.so': blob_fixup()
         .add_needed('libcutils.so'),
}  # fmt: skip

module = ExtractUtilsModule(
    'nemo',
    'realme',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
    add_firmware_proprietary_file=True,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
