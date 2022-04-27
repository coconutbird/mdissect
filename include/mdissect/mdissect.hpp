#ifndef MEMORY_MONO_HPP_
#define MEMORY_MONO_HPP_

#include "offsets.hpp"

#include <iostream>
#include <vector>
#include <functional>

namespace mdissect {
    using fn_read_memory = bool(*)(uint64_t address, void* buffer, size_t size);
    using fn_write_memory = bool(*)(uint64_t address, const void* buffer, size_t size);

    extern fn_read_memory read_memory;
    extern fn_write_memory write_memory;

    struct mono_class;

    struct mono_vtable {
        explicit mono_vtable(uint64_t address) : address(address) {};

        uint64_t address;

        bool initialized() const;

        mono_class mono_class() const;

        uint64_t static_field_data() const;
    };

    struct mono_class;

    struct mono_image {
        explicit mono_image(uint64_t address) : address(address) {};

        uint64_t address;

        std::vector<mono_class> types() const;

        using fn_match_callback = std::function<bool(mono_class)>;
        mono_class get_type(fn_match_callback callback) const;
    };

    struct mono_assembly {
        explicit mono_assembly(uint64_t address) : address(address) {}

        uint64_t address;

        std::string name() const;

        mono_image image() const;
    };

    struct mono_domain {
        explicit mono_domain(uint64_t address) : address(address) {}

        uint64_t address;

        bool operator==(const mono_domain other) const {
            return address == other.address;
        }

        bool operator!=(const mono_domain other) const {
            return !(*this == other);
        }

        int32_t domain_id() const;

        std::vector<mono_assembly> domain_assemblies() const;
    };

    struct mono_method {
        explicit mono_method(uint64_t address) : address(address) {}

        uint64_t address;

        std::string name() const;
    };

    struct mono_type {
        explicit mono_type(uint64_t address) : address(address) {}

        uint64_t address;

        uint32_t attributes() const;

        bool is_static() const;
        bool is_literal() const;

        mono_class mono_class() const;
    };

    struct mono_field {
        explicit mono_field(uint64_t address) : address(address) {}

        uint64_t address;

        std::string name() const;

        int32_t offset() const;

        mono_class parent() const;
        mono_type type() const;
    };

    struct mono_class {
        explicit mono_class(uint64_t address) : address(address) {}

        uint64_t address;

        mono_class parent() const;

        std::string name() const;

        std::vector<mono_field> fields() const;

        using fn_match_field_callback = std::function<bool(mono_field)>;
        mono_field get_field(fn_match_field_callback callback) const;

        std::vector<mono_method> methods() const;

        using fn_match_method_callback = std::function<bool(mono_method)>;
        mono_method get_method(fn_match_method_callback callback) const;

        mono_vtable vtable(mono_domain domain) const;

        int32_t vtable_size() const;
    };

    struct mono_object {
        explicit mono_object(uint64_t address) : address(address) {}

        uint64_t address;

        mono_vtable vtable() const;
    };

    struct mono {
        explicit mono(uint64_t address) : address(address) {}

        uint64_t address;

        mono_domain root_domain(uint64_t offset) const;
    };

    struct mono_hash_table {
        explicit mono_hash_table(uint64_t address);

        uint64_t address;

        int32_t size;
        uint64_t table;
    };
}

#endif