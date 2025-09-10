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
    auto vtable        = reinterpret_cast<void**>(hat::find_pattern(range2, hat::object_to_signature(typeinfo)).get() + sizeof(void*));

    int32_t version = major * 1000000 + minor * 1000 + patch;
    size_t index    = 0;

    size_t blankLineDismissesChat      = index += version >= 1021070   ? 41
                                                  : version >= 1021060 ? 42
                                                  : version >= 1021040 ? 40
                                                  : version >= 1021020 ? 38
                                                                       : 39;
    size_t supportsFilePicking         = index += version >= 1021090   ? 43
                                                  : version >= 1021080 ? 44
                                                                       : 43;
    size_t getDefaultNetworkMaxPlayers = index += version >= 1021080   ? 115
                                                  : version >= 1021020 ? 116
                                                  : version >= 1021000 ? 115
                                                  : version >= 1020080 ? 114
                                                                       : 113;
    size_t getMaxSimRadiusInChunks     = index += version >= 1021070   ? 38
                                                  : version >= 1021060 ? 37
                                                  : version >= 1021050 ? 35
                                                  : version >= 1021040 ? 34
                                                                       : 33;
    size_t getEdition                  = index += version >= 1021030   ? 19
                                                  : version >= 1020060 ? 18
                                                                       : 19;

    (void)index;

    vtable[blankLineDismissesChat] = (void*)+[](void*) -> bool {
        return true;
    };

    vtable[supportsFilePicking] = (void*)+[](void*) -> bool {
        return true;
    };

    vtable[getDefaultNetworkMaxPlayers] = (void*)+[](void*) -> int {
        return 8;
    };

    vtable[getMaxSimRadiusInChunks] = (void*)+[](void*) -> int {
        return 12;
    };

    vtable[getEdition] = (void*)+[](void*) -> std::string {
        return "win10";
    };
}
