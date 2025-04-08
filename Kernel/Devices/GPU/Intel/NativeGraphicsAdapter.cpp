/*
 * Copyright (c) 2021, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/PCI/API.h>
#include <Kernel/Bus/PCI/BarMapping.h>
#include <Kernel/Devices/GPU/Console/ContiguousFramebufferConsole.h>
#include <Kernel/Devices/GPU/Definitions.h>
#include <Kernel/Devices/GPU/Intel/NativeGraphicsAdapter.h>
#include <Kernel/Devices/GPU/Management.h>
#include <Kernel/Memory/PhysicalAddress.h>

namespace Kernel {

static constexpr u16 supported_models[] {
    0x29c2, // Intel G35 Adapter
};

static bool is_supported_model(u16 device_id)
{
    for (auto& id : supported_models) {
        if (id == device_id)
            return true;
    }
    return false;
}

ErrorOr<bool> IntelNativeGraphicsAdapter::probe(PCI::DeviceIdentifier const& pci_device_identifier)
{
    return is_supported_model(pci_device_identifier.hardware_id().device_id);
}

ErrorOr<NonnullLockRefPtr<GPUDevice>> IntelNativeGraphicsAdapter::create(PCI::DeviceIdentifier const& pci_device_identifier)
{
    auto adapter = TRY(adopt_nonnull_lock_ref_or_enomem(new (nothrow) IntelNativeGraphicsAdapter(pci_device_identifier)));
    TRY(adapter->initialize_adapter());
    return adapter;
}

ErrorOr<void> IntelNativeGraphicsAdapter::initialize_adapter()
{
    dbgln_if(INTEL_GRAPHICS_DEBUG, "Intel Native Graphics Adapter @ {}", device_identifier().address());
    auto bar0_space_size = PCI::get_BAR_space_size(device_identifier(), PCI::HeaderType0BaseRegister::BAR0);
    auto bar0_space_address = TRY(PCI::get_bar_address(device_identifier(), PCI::HeaderType0BaseRegister::BAR0));
    auto bar2_space_size = PCI::get_BAR_space_size(device_identifier(), PCI::HeaderType0BaseRegister::BAR2);
    auto bar2_space_address = TRY(PCI::get_bar_address(device_identifier(), PCI::HeaderType0BaseRegister::BAR2));
    dmesgln_pci(*this, "MMIO @ {}, space size is {:x} bytes", bar0_space_address, bar0_space_size);
    dmesgln_pci(*this, "framebuffer @ {}", bar2_space_address);

    using MMIORegion = IntelDisplayConnectorGroup::MMIORegion;
    MMIORegion first_region { MMIORegion::BARAssigned::BAR0, bar0_space_address, bar0_space_size };
    MMIORegion second_region { MMIORegion::BARAssigned::BAR2, bar2_space_address, bar2_space_size };

    PCI::enable_bus_mastering(device_identifier());
    PCI::enable_io_space(device_identifier());
    PCI::enable_memory_space(device_identifier());

    switch (device_identifier().hardware_id().device_id) {
    case 0x29c2:
        m_connector_group = TRY(IntelDisplayConnectorGroup::try_create({}, IntelGraphics::Generation::Gen4, first_region, second_region));
        return {};
    default:
        return Error::from_errno(ENODEV);
    }
}

IntelNativeGraphicsAdapter::IntelNativeGraphicsAdapter(PCI::DeviceIdentifier const& pci_device_identifier)
    : GPUDevice()
    , PCI::Device(const_cast<PCI::DeviceIdentifier&>(pci_device_identifier))
{
}

ErrorOr<void> IntelNativeGraphicsAdapter::enable_hardware_acceleration()
{
    SpinlockLocker locker(m_control_lock);
    
    // Initialize hardware acceleration
    TRY(initialize_command_ring());
    TRY(initialize_contexts());
    TRY(initialize_engine_control());
    
    m_hardware_acceleration_enabled = true;
    return {};
}

ErrorOr<void> IntelNativeGraphicsAdapter::disable_hardware_acceleration()
{
    SpinlockLocker locker(m_control_lock);
    
    // Clean up hardware acceleration resources
    TRY(cleanup_command_ring());
    TRY(cleanup_contexts());
    TRY(cleanup_engine_control());
    
    m_hardware_acceleration_enabled = false;
    return {};
}

bool IntelNativeGraphicsAdapter::is_hardware_accelerated() const
{
    return m_hardware_acceleration_enabled;
}

ErrorOr<void> IntelNativeGraphicsAdapter::initialize_command_ring()
{
    // Initialize command ring buffer for GPU commands
    // This is a simplified implementation - actual implementation would need
    // to handle ring buffer allocation, initialization, and command submission
    return {};
}

ErrorOr<void> IntelNativeGraphicsAdapter::initialize_contexts()
{
    // Initialize GPU contexts for different operations
    // This would typically involve setting up context state and resources
    return {};
}

ErrorOr<void> IntelNativeGraphicsAdapter::initialize_engine_control()
{
    // Initialize GPU engine control
    // This would involve setting up engine state and control registers
    return {};
}

ErrorOr<void> IntelNativeGraphicsAdapter::cleanup_command_ring()
{
    // Clean up command ring resources
    return {};
}

ErrorOr<void> IntelNativeGraphicsAdapter::cleanup_contexts()
{
    // Clean up GPU contexts
    return {};
}

ErrorOr<void> IntelNativeGraphicsAdapter::cleanup_engine_control()
{
    // Clean up engine control resources
    return {};
}

}
