#include <mdissect/mdissect.hpp>

#include <mdissect/offsets.hpp>

namespace mdissect {
    fn_read_memory read_memory = nullptr;
    fn_write_memory write_memory = nullptr;

    __forceinline bool read(uint64_t address, void* buffer, size_t size) {
        return read_memory(address, buffer, size);
    }

    __forceinline bool write(uint64_t address, const void* buffer, size_t size) {
        return write_memory(address, buffer, size);
    }

    template <typename T>
    __forceinline static T read(uint64_t address) {
        T result;
        if (!read_memory(address, &result, sizeof(T)))
            return {};

        return result;
    }

    template <typename T>
    __forceinline static bool write(uint64_t address, const T& value) {
        return write_memory(address, &value, sizeof(T));
    }

    // mono_vtable

    bool mono_vtable::initialized() const {
        return read<uint8_t>(address + offsets::MonoVTableInitialized) != 0;
    }

    mono_class mono_vtable::mono_class() const {
        return mdissect::mono_class(read<uint64_t>(address + offsets::MonoVTableClass));
    }

    uint64_t mono_vtable::static_field_data() const {
        if ((read<uint8_t>(address + offsets::MonoVTableFlags) & 0x4) != 0) {
            const auto parent_class = mono_class();

            return read<uint64_t>(address + offsets::MonoVTableVTable + parent_class.vtable_size() * 0x8);
        }

        return 0;
    }

    // mono_image

    std::vector<mono_class> mono_image::types() const {
        std::vector<mono_class> result;

        const auto cache = mono_hash_table(address + offsets::MonoImageClassCache);

        for (int32_t i = 0; i < cache.size; ++i) {
            for (uint64_t it = read<uint64_t>(cache.table + i * 0x8); it != 0; it = read<uint64_t>(it + offsets::MonoClassDefNextCache)) {
                result.emplace_back(mono_class(it));
            }
        }

        return result;
    }

    mono_class mono_image::get_type(fn_match_callback callback) const {
        const auto cache = mono_hash_table(address + offsets::MonoImageClassCache);

        for (int32_t j = 0; j < cache.size; ++j) {
            for (uint64_t it = read<uint64_t>(cache.table + j * 0x8); it != 0; it = read<uint64_t>(it + offsets::MonoClassDefNextCache)) {
                const auto mono_class = mdissect::mono_class(it);

                if (callback(mono_class))
                    return mono_class;
            }
        }

        return mono_class(0);
    }

    // mono_assembly

    std::string mono_assembly::name() const {
        char name[256] {};
        if (!read(read<uint64_t>(address + offsets::MonoAssemblyName), name, sizeof(name) - 1))
            return {};

        return name;
    }

    mono_image mono_assembly::image() const {
        return mono_image(read<uint64_t>(address + offsets::MonoAssemblyImage));
    }

    // mono_domain

    int32_t mono_domain::domain_id() const {
        return read<int32_t>(address + 0xBC);
    }

    std::vector<mono_assembly> mono_domain::domain_assemblies() const {
        std::vector<mono_assembly> result;

        const uint64_t assemblies = read<uint64_t>(address + offsets::DomainDomainAssemblies);
        for (uint64_t it = assemblies; it != 0; it = read<uint64_t>(it + 0x8)) {
            const uint64_t assembly = read<uint64_t>(it);
            if (assembly == 0)
                continue;

            result.emplace_back(mono_assembly(assembly));
        }

        return result;
    }

    // mono_method

    std::string mono_method::name() const {
        char name[256] {};
        if (!read(read<uint64_t>(address + offsets::MonoMethodName), name, sizeof(name) - 1))
            return {};

        return name;
    }

    // mono_type

    uint32_t mono_type::attributes() const {
        return read<uint32_t>(address + offsets::MonoTypeAttributes);
    }

    bool mono_type::is_static() const {
        return (attributes() & 0x10) != 0;
    }

    bool mono_type::is_literal() const {
        return (attributes() & 0x40) != 0;
    }

    mono_class mono_type::mono_class() const {
        return mdissect::mono_class(read<uint64_t>(address + offsets::MonoTypeData));
    }

    // mono_field

    std::string mono_field::name() const {
        char name[256] {};
        if (!read(read<uint64_t>(address + offsets::MonoClassFieldName), name, sizeof(name) - 1))
            return {};

        return name;
    }

    int32_t mono_field::offset() const {
        return read<int32_t>(address + offsets::MonoClassFieldOffset);
    }

    mono_class mono_field::parent() const {
        return mono_class(read<uint64_t>(address + offsets::MonoClassFieldParent));
    }

    mono_type mono_field::type() const {
        return mono_type(read<uint64_t>(address + offsets::MonoClassFieldType));
    }

    // mono_class

    mono_class mono_class::parent() const {
        return mono_class(read<uint64_t>(address + offsets::MonoClassParent));
    }

    std::string mono_class::name() const {
        char name[256] {};
        if (!read(read<uint64_t>(address + offsets::MonoClassName), name, sizeof(name) - 1))
            return {};

        return name;
    }

    std::vector<mono_field> mono_class::fields() const {
        const auto fields = read<uint64_t>(address + offsets::MonoClassFields);
        const auto field_count = read<int32_t>(address + offsets::MonoClassDefFieldCount);

        std::vector<mono_field> result;
        result.reserve(field_count);

        for (int32_t i = 0; i < field_count; ++i)
            result.emplace_back(mono_field(fields + i * 0x20));

        return result;
    }

    mono_field mono_class::get_field(fn_match_field_callback callback) const {
        const auto fields = read<uint64_t>(address + offsets::MonoClassFields);
        const auto field_count = read<int32_t>(address + offsets::MonoClassDefFieldCount);
        for (int32_t i = 0; i < field_count; ++i) {
            const auto mono_field = mdissect::mono_field(fields + i * 0x20);
            if (callback(mono_field))
                return mono_field;
        }

        return mono_field(0);
    }

    std::vector<mono_method> mono_class::methods() const {
        const auto method_count = read<uint32_t>(address + offsets::MonoClassDefMethodCount);
        const auto methods = read<uint64_t>(address + offsets::MonoClassMethods);

        std::vector<mono_method> result;
        result.reserve(method_count);

        for (uint32_t i = 0; i < method_count; ++i)
            result.emplace_back(mono_method(read<uint64_t>(methods + i * 0x8)));

        return result;
    }

    mono_method mono_class::get_method(fn_match_method_callback callback) const {
        const auto method_count = read<uint32_t>(address + offsets::MonoClassDefMethodCount);
        const auto methods = read<uint64_t>(address + offsets::MonoClassMethods);
        for (uint32_t i = 0; i < method_count; ++i) {
            const auto mono_method = mdissect::mono_method(read<uint64_t>(methods + i * 0x8));
            if (callback(mono_method))
                return mono_method;
        }

        return mono_method(0);
    }

    mono_vtable mono_class::vtable(mono_domain domain) const {
        const auto runtime_info = read<uint64_t>(address + offsets::MonoClassRuntimeInfo);
        if (runtime_info == 0)
            return mono_vtable(0);

        const int32_t domain_id = domain.domain_id();
        const auto max_domain = read<uint16_t>(runtime_info + offsets::MonoClassRuntimeInfoMaxDomain);
        if (max_domain < domain_id)
            return mono_vtable(0);

        return mono_vtable(read<uint64_t>(runtime_info + offsets::MonoClassRuntimeInfoDomainVTables + domain_id * 0x8));
    }

    int32_t mono_class::vtable_size() const {
        return read<int32_t>(address + offsets::MonoClassVTableSize);
    }

    // mono_object

    mono_vtable mono_object::vtable() const {
        return mono_vtable(read<uint64_t>(address + offsets::MonoObjectVTable));
    }

    // mono

    mono_domain mono::root_domain(uint64_t offset) const {
        return mono_domain(read<uint64_t>(address + offset));
    }

    // mono_hash_table

    mono_hash_table::mono_hash_table(uint64_t address) : address(address) {
        size = read<int32_t>(address + offsets::MonoInternalHashTableSize);
        table = read<uint64_t>(address + offsets::MonoInternalHashTableTable);
    }
}