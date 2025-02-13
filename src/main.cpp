#define LOG_TAG "Patches"
#include <android/log_macros.h>
#include <android/native_activity.h>
#include <dlfcn.h>
#include <string>
#include <thread>
#include <tuple>

template<int32_t>
struct AppPlatform_vtable;

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
            if (patch >= 50)
                return get<1021050>();
            if (patch >= 40)
                return get<1021040>();
            if (patch >= 30)
                return get<1021030>();
            if (patch >= 20)
                return get<1021020>();
            return get<1021000>();
        }
        if (patch >= 80)
            return get<1020080>();
        if (patch >= 70)
            return get<1020070>();
        if (patch >= 60)
            return get<1020060>();
        return get<1020050>();
    }
};

extern "C" [[gnu::visibility("default")]] void mod_preinit() {
    auto h = dlopen("libmcpelauncher_mod.so", 0);

    auto mcpelauncher_preinithook = (void (*)(const char*, void*, void**))dlsym(h, "mcpelauncher_preinithook");

    static ANativeActivity_createFunc* onCreate_orig;

    mcpelauncher_preinithook(
        "ANativeActivity_onCreate",
        (void*)+[](ANativeActivity* activity, void* savedState, size_t savedStateSize) {
            onCreate_orig(activity, savedState, savedStateSize);

            Dl_info info;
            dladdr((void*)onCreate_orig, &info);

            auto version = info.dli_fname + strlen(info.dli_fname) - 1;
            for (int n = 4; version >= info.dli_fname; --version) {
                if (*version == '/' && --n == 0)
                    break;
            }

            int major, minor, patch, revision;
            if (sscanf(++version, "%d.%d.%d.%d", &major, &minor, &patch, &revision) != 4) {
                ALOGE("Failed to detect version");
                return;
            }

            if (std::tie(major, minor, patch) < std::tuple{1, 20, 50}) {
                ALOGE("Versions before 1.20.50 are unsupported");
                return;
            }

            if (revision >= 20) {
                ALOGE("Beta versions are unsupported");
                return;
            }

            std::thread{[instance = (uintptr_t***)activity->instance,
                         offsets  = AppPlatform_offsets::get(minor, patch)] {
                while (!instance[0])
                    std::this_thread::yield();

                auto vt = instance[0][1][0];

                *(void**)(vt + offsets.blankLineDismissesChat) = (void*)+[](void*) -> bool {
                    return true;
                };

                *(void**)(vt + offsets.supportsFilePicking) = (void*)+[](void*) -> bool {
                    return true;
                };

                *(void**)(vt + offsets.getDefaultNetworkMaxPlayers) = (void*)+[](void*) -> int {
                    return 8;
                };

                *(void**)(vt + offsets.getMaxSimRadiusInChunks) = (void*)+[](void*) -> int {
                    return 12;
                };

                *(void**)(vt + offsets.getEdition) = (void*)+[](void*) -> std::string {
                    return "win10";
                };
            }}.detach();
        },
        (void**)&onCreate_orig);
}

extern "C" [[gnu::visibility("default")]] void mod_init() {}
