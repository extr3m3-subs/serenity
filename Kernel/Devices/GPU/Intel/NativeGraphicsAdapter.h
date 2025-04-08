/*
 * Copyright (c) 2021, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>
#include <Kernel/Bus/PCI/Device.h>
#include <Kernel/Devices/GPU/Definitions.h>
#include <Kernel/Devices/GPU/Intel/DisplayConnectorGroup.h>
#include <Kernel/Devices/GPU/Intel/NativeDisplayConnector.h>
#include <Kernel/Memory/PhysicalAddress.h>
#include <LibEDID/EDID.h>

namespace Kernel {

class IntelNativeGraphicsAdapter final
    : public GPUDevice
    , public PCI::Device {

public:
    static ErrorOr<bool> probe(PCI::DeviceIdentifier const&);
    static ErrorOr<NonnullLockRefPtr<GPUDevice>> create(PCI::DeviceIdentifier const&);

    virtual ~IntelNativeGraphicsAdapter() = default;

    virtual StringView device_name() const override { return "IntelNativeGraphicsAdapter"sv; }

    // Hardware acceleration support
    virtual ErrorOr<void> enable_hardware_acceleration() override;
    virtual ErrorOr<void> disable_hardware_acceleration() override;
    virtual bool is_hardware_accelerated() const override;

private:
    ErrorOr<void> initialize_adapter();
    ErrorOr<void> initialize_command_ring();
    ErrorOr<void> initialize_contexts();
    ErrorOr<void> initialize_engine_control();
    ErrorOr<void> cleanup_command_ring();
    ErrorOr<void> cleanup_contexts();
    ErrorOr<void> cleanup_engine_control();

    explicit IntelNativeGraphicsAdapter(PCI::DeviceIdentifier const&);

    LockRefPtr<IntelDisplayConnectorGroup> m_connector_group;
    bool m_hardware_acceleration_enabled { false };
};
}
