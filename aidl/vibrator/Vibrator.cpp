/*
 * Copyright (C) 2022-2023 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fstream>
#include <thread>
#include <android-base/logging.h>

#include "Vibrator.h"

namespace aidl {
namespace android {
namespace hardware {
namespace vibrator {

/*
 * Write value to path and close file.
 */
template <typename T>
static void write(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value << std::endl;
}

static bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return (file.good());
}

Vibrator::Vibrator() {
    mAmplitudeControl = fileExists(VIBRATOR_INTENSITY);
    write(VIBRATOR_STATE, 1);
}

ndk::ScopedAStatus Vibrator::activate(int32_t timeoutMs) {
    write(VIBRATOR_DURATION, timeoutMs);
    write(VIBRATOR_ACTIVATE, 1);

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getCapabilities(int32_t* _aidl_return) {
    *_aidl_return = IVibrator::CAP_ON_CALLBACK | IVibrator::CAP_PERFORM_CALLBACK;

    if (mAmplitudeControl)
        *_aidl_return |= IVibrator::CAP_AMPLITUDE_CONTROL;

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::off() {
    write(VIBRATOR_ACTIVATE, 0);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::on(int32_t timeoutMs, const std::shared_ptr<IVibratorCallback>& callback) {
    ndk::ScopedAStatus status = activate(timeoutMs);

    if (callback != nullptr) {
        std::thread([=] {
            LOG(DEBUG) << "Starting on on another thread";
            usleep(timeoutMs * 1000);
            LOG(DEBUG) << "Notifying on complete";
            if (!callback->onComplete().isOk()) {
                LOG(ERROR) << "Failed to call onComplete";
            }
        }).detach();
    }

    return status;
}

ndk::ScopedAStatus Vibrator::perform(Effect effect, EffectStrength strength, const std::shared_ptr<IVibratorCallback>& callback, int32_t* _aidl_return) {
    ndk::ScopedAStatus status;
    int32_t timeoutMs;
    float amplitude;

    switch (strength) {
        case EffectStrength::LIGHT:
            amplitude = AMPLITUDE_LIGHT;
            break;
        case EffectStrength::MEDIUM:
            amplitude = AMPLITUDE_MEDIUM;
            break;
        case EffectStrength::STRONG:
            amplitude = AMPLITUDE_STRONG;
            break;
        default:
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    setAmplitude(amplitude);

    switch (effect) {
        case Effect::CLICK:
            timeoutMs = 70;
            break;
        case Effect::TEXTURE_TICK:
            timeoutMs = 40;
            break;
        case Effect::TICK:
            timeoutMs = 80;
            break;
        default:
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    status = activate(timeoutMs);

    if (callback != nullptr) {
        std::thread([=] {
            LOG(DEBUG) << "Starting perform on another thread";
            usleep(timeoutMs * 1000);
            LOG(DEBUG) << "Notifying perform complete";
            callback->onComplete();
        }).detach();
    }

    *_aidl_return = timeoutMs;
    return status;
}

ndk::ScopedAStatus Vibrator::getSupportedEffects(std::vector<Effect>* _aidl_return) {
    *_aidl_return = {Effect::CLICK, Effect::TEXTURE_TICK, Effect::TICK};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setAmplitude(float amplitude) {
    int32_t intensity;

    if (amplitude <= 0.0f || amplitude > 1.0f)
        return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_ILLEGAL_ARGUMENT));

    LOG(DEBUG) << "Setting amplitude: " << amplitude;

    intensity = amplitude * INTENSITY_MAX;

    LOG(DEBUG) << "Setting intensity: " << intensity;

    if (mAmplitudeControl)
        write(VIBRATOR_INTENSITY, intensity);

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setExternalControl(bool /*enabled*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getCompositionDelayMax(int32_t* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getCompositionSizeMax(int32_t* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedPrimitives(std::vector<CompositePrimitive>* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getPrimitiveDuration(CompositePrimitive /*primitive*/, int32_t* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::compose(const std::vector<CompositeEffect>& /*composite*/, const std::shared_ptr<IVibratorCallback>& /*callback*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedAlwaysOnEffects(std::vector<Effect>* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::alwaysOnEnable(int32_t /*id*/, Effect /*effect*/, EffectStrength /*strength*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::alwaysOnDisable(int32_t /*id*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getResonantFrequency(float* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getQFactor(float* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getFrequencyResolution(float* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getFrequencyMinimum(float* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getBandwidthAmplitudeMap(std::vector<float>* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getPwlePrimitiveDurationMax(int32_t* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getPwleCompositionSizeMax(int32_t* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedBraking(std::vector<Braking>* /*_aidl_return*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::composePwle(const std::vector<PrimitivePwle>& /*composite*/, const std::shared_ptr<IVibratorCallback>& /*callback*/) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

} // namespace vibrator
} // namespace hardware
} // namespace android
} // namespace aidl
