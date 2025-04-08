#include "AudioEffects.h"
#include <AK/Math.h>
#include <AK/NumericLimits.h>

namespace Audio {

ErrorOr<NonnullRefPtr<AudioEffectProcessor>> AudioEffectProcessor::create()
{
    auto processor = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) AudioEffectProcessor));
    processor->m_reverb_buffer = TRY(FixedArray<float>::create(reverb_buffer_size));
    processor->m_echo_buffer = TRY(FixedArray<float>::create(echo_buffer_size));
    processor->m_chorus_buffer = TRY(FixedArray<float>::create(chorus_buffer_size));
    return processor;
}

ErrorOr<void> AudioEffectProcessor::process_effect(Bytes buffer, PcmSampleFormat format, size_t sample_count, AudioEffect effect, float strength)
{
    Threading::MutexLocker locker(m_effect_lock);
    
    switch (effect) {
    case AudioEffect::Reverb:
        return apply_reverb(buffer, format, sample_count, strength);
    case AudioEffect::Echo:
        return apply_echo(buffer, format, sample_count, strength);
    case AudioEffect::Chorus:
        return apply_chorus(buffer, format, sample_count, strength);
    case AudioEffect::Distortion:
        return apply_distortion(buffer, format, sample_count, strength);
    case AudioEffect::LowPass:
        return apply_low_pass(buffer, format, sample_count, strength);
    case AudioEffect::HighPass:
        return apply_high_pass(buffer, format, sample_count, strength);
    case AudioEffect::BandPass:
        return apply_band_pass(buffer, format, sample_count, strength);
    default:
        return {};
    }
}

ErrorOr<void> AudioEffectProcessor::apply_reverb(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength)
{
    // Simple reverb implementation using a feedback delay line
    float* samples = reinterpret_cast<float*>(buffer.data());
    float decay = 0.5f * strength;
    
    for (size_t i = 0; i < sample_count; ++i) {
        float delayed_sample = m_reverb_buffer[m_reverb_position];
        m_reverb_buffer[m_reverb_position] = samples[i] + delayed_sample * decay;
        samples[i] = samples[i] + delayed_sample * decay;
        m_reverb_position = (m_reverb_position + 1) % reverb_buffer_size;
    }
    
    return {};
}

ErrorOr<void> AudioEffectProcessor::apply_echo(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength)
{
    // Simple echo implementation
    float* samples = reinterpret_cast<float*>(buffer.data());
    float feedback = 0.7f * strength;
    
    for (size_t i = 0; i < sample_count; ++i) {
        float delayed_sample = m_echo_buffer[m_echo_position];
        m_echo_buffer[m_echo_position] = samples[i] + delayed_sample * feedback;
        samples[i] = samples[i] + delayed_sample * feedback;
        m_echo_position = (m_echo_position + 1) % echo_buffer_size;
    }
    
    return {};
}

ErrorOr<void> AudioEffectProcessor::apply_chorus(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength)
{
    // Simple chorus implementation using a modulated delay line
    float* samples = reinterpret_cast<float*>(buffer.data());
    float modulation_depth = 0.002f * strength;
    float modulation_rate = 0.1f;
    
    for (size_t i = 0; i < sample_count; ++i) {
        float modulation = sin(m_chorus_phase) * modulation_depth;
        size_t delay_samples = static_cast<size_t>(modulation * chorus_buffer_size);
        size_t read_pos = (m_chorus_position + chorus_buffer_size - delay_samples) % chorus_buffer_size;
        
        float delayed_sample = m_chorus_buffer[read_pos];
        m_chorus_buffer[m_chorus_position] = samples[i];
        samples[i] = samples[i] * 0.7f + delayed_sample * 0.3f;
        
        m_chorus_position = (m_chorus_position + 1) % chorus_buffer_size;
        m_chorus_phase += modulation_rate;
    }
    
    return {};
}

ErrorOr<void> AudioEffectProcessor::apply_distortion(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength)
{
    // Simple distortion implementation
    float* samples = reinterpret_cast<float*>(buffer.data());
    float threshold = 0.5f * (1.0f - strength);
    
    for (size_t i = 0; i < sample_count; ++i) {
        float sample = samples[i];
        if (sample > threshold) {
            sample = threshold + (sample - threshold) * (1.0f - strength);
        } else if (sample < -threshold) {
            sample = -threshold + (sample + threshold) * (1.0f - strength);
        }
        samples[i] = sample;
    }
    
    return {};
}

ErrorOr<void> AudioEffectProcessor::apply_low_pass(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength)
{
    // Simple low-pass filter implementation
    float* samples = reinterpret_cast<float*>(buffer.data());
    float cutoff = m_low_pass_cutoff + strength * 0.5f;
    float rc = 1.0f / (2.0f * AK::Pi<float> * cutoff);
    float dt = 1.0f / 44100.0f; // Assuming 44.1kHz sample rate
    float alpha = dt / (rc + dt);
    
    float last_sample = 0.0f;
    for (size_t i = 0; i < sample_count; ++i) {
        last_sample = last_sample + alpha * (samples[i] - last_sample);
        samples[i] = last_sample;
    }
    
    return {};
}

ErrorOr<void> AudioEffectProcessor::apply_high_pass(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength)
{
    // Simple high-pass filter implementation
    float* samples = reinterpret_cast<float*>(buffer.data());
    float cutoff = m_high_pass_cutoff + strength * 0.5f;
    float rc = 1.0f / (2.0f * AK::Pi<float> * cutoff);
    float dt = 1.0f / 44100.0f; // Assuming 44.1kHz sample rate
    float alpha = rc / (rc + dt);
    
    float last_sample = 0.0f;
    for (size_t i = 0; i < sample_count; ++i) {
        float new_sample = alpha * (last_sample + samples[i] - samples[i - 1]);
        last_sample = samples[i];
        samples[i] = new_sample;
    }
    
    return {};
}

ErrorOr<void> AudioEffectProcessor::apply_band_pass(Bytes buffer, PcmSampleFormat format, size_t sample_count, float strength)
{
    // Simple band-pass filter implementation (combination of low-pass and high-pass)
    float* samples = reinterpret_cast<float*>(buffer.data());
    float center = m_band_pass_center + strength * 0.5f;
    float width = m_band_pass_width * (1.0f - strength);
    
    // Apply low-pass filter
    float low_cutoff = center + width/2;
    float rc_low = 1.0f / (2.0f * AK::Pi<float> * low_cutoff);
    float dt = 1.0f / 44100.0f;
    float alpha_low = dt / (rc_low + dt);
    
    // Apply high-pass filter
    float high_cutoff = center - width/2;
    float rc_high = 1.0f / (2.0f * AK::Pi<float> * high_cutoff);
    float alpha_high = rc_high / (rc_high + dt);
    
    float last_sample_low = 0.0f;
    float last_sample_high = 0.0f;
    
    for (size_t i = 0; i < sample_count; ++i) {
        // Low-pass stage
        last_sample_low = last_sample_low + alpha_low * (samples[i] - last_sample_low);
        
        // High-pass stage
        float new_sample = alpha_high * (last_sample_high + last_sample_low - samples[i - 1]);
        last_sample_high = last_sample_low;
        samples[i] = new_sample;
    }
    
    return {};
}

} 