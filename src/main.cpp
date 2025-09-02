#define LOG_TAG "Patches"
#include <android/log_macros.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <libhat/scanner.hpp>
#include <link.h>
#include <string>
#include <tuple>

template<int32_t>
struct AppPlatform_vtable;

#define MC_VERSION 1021090
#include "vtable.inl"
#define MC_VERSION 1021080
#include "vtable.inl"
#define MC_VERSION 1021070
#include "vtable.inl"
#define MC_VERSION 1021060
#include "vtable.inl"
#define MC_VERSION 1021050
#include "vtable.inl"
#define MC_VERSION 1021040
#include "vtable.inl"
#define MC_VERSION 1021030
#include "vtable.inl"
#define MC_VERSION 1021020
#include "vtable.inl"
#define MC_VERSION 1021000
#include "vtable.inl"
#define MC_VERSION 1020080
#include "vtable.inl"
#define MC_VERSION 1020070
#include "vtable.inl"
#define MC_VERSION 1020060
#include "vtable.inl"
#define MC_VERSION 1020050
#include "vtable.inl"

struct AppPlatform_offsets {
    uint16_t blankLineDismissesChat;
    uint16_t supportsFilePicking;
    uint16_t getDefaultNetworkMaxPlayers;
    uint16_t getMaxSimRadiusInChunks;
    uint16_t getEdition;

    template<int32_t Version>
    static consteval AppPlatform_offsets get() {
        using T = AppPlatform_vtable<Version>;
        return {
            .blankLineDismissesChat      = offsetof(T, blankLineDismissesChat),
            .supportsFilePicking         = offsetof(T, supportsFilePicking),
            .getDefaultNetworkMaxPlayers = offsetof(T, getDefaultNetworkMaxPlayers),
            .getMaxSimRadiusInChunks     = offsetof(T, getMaxSimRadiusInChunks),
            .getEdition                  = offsetof(T, getEdition),
        };
    }

    static AppPlatform_offsets get(int minor, int patch) {
        if (minor >= 21) {
            if (patch >= 90) return get<1021090>();
            if (patch >= 80) return get<1021080>();
            if (patch >= 70) return get<1021070>();
            if (patch >= 60) return get<1021060>();
            if (patch >= 50) return get<1021050>();
            if (patch >= 40) return get<1021040>();
            if (patch >= 30) return get<1021030>();
            if (patch >= 20) return get<1021020>();
            return get<1021000>();
        }
        if (patch >= 80) return get<1020080>();
        if (patch >= 70) return get<1020070>();
        if (patch >= 60) return get<1020060>();
        return get<1020050>();
    }
};

extern "C" [[gnu::visibility("default")]] void mod_preinit() {}

extern "C" [[gnu::visibility("default")]] void mod_init() {
    auto mcLib = dlopen("libminecraftpe.so", 0);

    std::span<std::byte> range1, range2;
    int major, minor, patch, revision;

    auto callback = [&](const dl_phdr_info& info) {
        if (auto h = dlopen(info.dlpi_name, RTLD_NOLOAD); dlclose(h), h != mcLib)
            return 0;

        range1 = {reinterpret_cast<std::byte*>(info.dlpi_addr + info.dlpi_phdr[1].p_vaddr), info.dlpi_phdr[1].p_memsz};
        range2 = {reinterpret_cast<std::byte*>(info.dlpi_addr + info.dlpi_phdr[2].p_vaddr), info.dlpi_phdr[2].p_memsz};

        auto version = info.dlpi_name + strlen(info.dlpi_name) - 1;
        for (int n = 4; version >= info.dlpi_name; --version) {
            if (*version == '/' && --n == 0)
                break;
        }

        if (sscanf(++version, "%d.%d.%d.%d", &major, &minor, &patch, &revision) != 4) { // NOLINT(cert-err34-c)
            ALOGE("Failed to detect version");
            return 0;
        }

        ALOGI("Detected version %d.%d.%d.%d", major, minor, patch, revision);

        if (std::tie(major, minor, patch) < std::tuple{1, 20, 50}) {
            ALOGE("Versions before 1.20.50 are unsupported");
            return 0;
        }

        if (revision >= 20) {
            ALOGE("Beta versions are unsupported");
            return 0;
        }

        return 1;
    };

    if (!dl_iterate_phdr(
            [](dl_phdr_info* info, size_t, void* data) {
                return (*static_cast<decltype(callback)*>(data))(*info);
            },
            &callback))
        return;

    auto typeinfo_name = hat::find_pattern(range1, hat::object_to_signature("19AppPlatform_android")).get();
    auto typeinfo      = hat::find_pattern(range2, hat::object_to_signature(typeinfo_name)).get() - sizeof(void*);
    auto vtable        = hat::find_pattern(range2, hat::object_to_signature(typeinfo)).get() + sizeof(void*);
    auto offsets       = AppPlatform_offsets::get(minor, patch);

    *(void**)(vtable + offsets.blankLineDismissesChat) = (void*)+[](void*) -> bool {
        return true;
    };

    *(void**)(vtable + offsets.supportsFilePicking) = (void*)+[](void*) -> bool {
        return true;
    };

    *(void**)(vtable + offsets.getDefaultNetworkMaxPlayers) = (void*)+[](void*) -> int {
        return 8;
    };

    *(void**)(vtable + offsets.getMaxSimRadiusInChunks) = (void*)+[](void*) -> int {
        return 12;
    };

    *(void**)(vtable + offsets.getEdition) = (void*)+[](void*) -> std::string {
        return "win10";
    };
}
