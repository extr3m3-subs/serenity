#pragma once

#include <AK/Array.h>
#include <AK/Atomic.h>
#include <AK/FixedArray.h>
#include <AK/NonnullRefPtr.h>
#include <AK/RefCounted.h>
#include <AK/Vector.h>
#include <LibAudio/SampleFormats.h>
#include <LibThreading/Mutex.h>

namespace Audio {

class AudioEffectProcessor : public RefCounted<AudioEffectProcessor> {
public:
    static ErrorOr<NonnullRefPtr<AudioEffectProcessor>> create();

    ErrorOr<void> process_effect(Bytes buffer, PcmSampleFormat format, size_t sample_count, AudioEffect effect, float strength);
    
    // Reverb effect
    ErrorOr<void> apply_reverb(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength);
    
    // Echo effect
    ErrorOr<void> apply_echo(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength);
    
    // Chorus effect
    ErrorOr<void> apply_chorus(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength);
    
    // Distortion effect
    ErrorOr<void> apply_distortion(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength);
    
    // Filter effects
    ErrorOr<void> apply_low_pass(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength);
    ErrorOr<void> apply_high_pass(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength);
    ErrorOr<void> apply_band_pass(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength);

private:
    AudioEffectProcessor() = default;
    
    // Reverb parameters
    static constexpr size_t reverb_buffer_size = 44100; // 1 second at 44.1kHz
    FixedArray<float> m_reverb_buffer;
    size_t m_reverb_position { 0 };
    
    // Echo parameters
    static constexpr size_t echo_buffer_size = 22050; // 0.5 seconds at 44.1kHz
    FixedArray<float> m_echo_buffer;
    size_t m_echo_position { 0 };
    
    // Chorus parameters
    static constexpr size_t chorus_buffer_size = 1024;
    FixedArray<float> m_chorus_buffer;
    size_t m_chorus_position { 0 };
    float m_chorus_phase { 0.0f };
    
    // Filter parameters
    float m_low_pass_cutoff { 0.0f };
    float m_high_pass_cutoff { 0.0f };
    float m_band_pass_center { 0.0f };
    float m_band_pass_width { 0.0f };
    
    Threading::Mutex m_effect_lock;
};

} 