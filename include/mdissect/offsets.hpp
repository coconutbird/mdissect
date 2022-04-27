#ifndef MDISSECT_OFFSETS_HPP
#define MDISSECT_OFFSETS_HPP

#include <cstdint>

// offsets are taken from IDA

namespace offsets {
    // _MonoDomain
    constexpr int32_t DomainDomainAssemblies = 0xC8;

    // _MonoAssembly
    constexpr int32_t MonoAssemblyName = 0x10;
    constexpr int32_t MonoAssemblyImage = 0x60;

    // _MonoImage
    constexpr int32_t MonoImageClassCache = 0x4C0;

    // _MonoClass
    constexpr int32_t MonoClassParent = 0x30;
    constexpr int32_t MonoClassName = 0x48;
    constexpr int32_t MonoClassVTableSize = 0x5C;
    constexpr int32_t MonoClassFields = 0x98;
    constexpr int32_t MonoClassMethods = 0xA0;
    constexpr int32_t MonoClassRuntimeInfo = 0xD0;

    // MonoClassDef
    constexpr int32_t MonoClassDefMethodCount = 0xFC;
    constexpr int32_t MonoClassDefFieldCount = 0x100;
    constexpr int32_t MonoClassDefNextCache = 0x108;

    // _MonoClassField
    constexpr int32_t MonoClassFieldType = 0x0;
    constexpr int32_t MonoClassFieldName = 0x8;
    constexpr int32_t MonoClassFieldParent = 0x10;
    constexpr int32_t MonoClassFieldOffset = 0x18;

    // _MonoMethod
    constexpr int32_t MonoMethodName = 0x18;

    // _MonoObject
    constexpr int32_t MonoObjectVTable = 0x0;

    // _MonoType
    constexpr int32_t MonoTypeData = 0x0;
    constexpr int32_t MonoTypeAttributes = 0x8;

    // MonoClassRuntimeInfo
    constexpr int32_t MonoClassRuntimeInfoMaxDomain = 0x0;
    constexpr int32_t MonoClassRuntimeInfoDomainVTables = 0x8;

    // MonoVTable
    constexpr int32_t MonoVTableClass = 0x0;
    constexpr int32_t MonoVTableInitialized = 0x2D;
    constexpr int32_t MonoVTableFlags = 0x30;
    constexpr int32_t MonoVTableVTable = 0x40;

    // _MonoInternalHashTable
    constexpr int32_t MonoInternalHashTableSize = 0x18;
    constexpr int32_t MonoInternalHashTableTable = 0x20;
}

#endif