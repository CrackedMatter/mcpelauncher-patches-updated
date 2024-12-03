#include <android/native_activity.h>
#include <dlfcn.h>
#include <string>

struct AppPlatform_vtable {
#if MC_VERSION >= 1021040
    void* pad0[40];
#elif MC_VERSION >= 1021020
    void* pad0[38];
#else
    void* pad0[39];
#endif
    bool (* blankLineDismissesChat)(void*);
    void* pad1[42];
    bool (* supportsFilePicking)(void*);
#if MC_VERSION >= 1021020
    void* pad2[115];
#elif MC_VERSION >= 1021000
    void* pad2[114];
#elif MC_VERSION >= 1020080
    void* pad2[113];
#else
    void* pad2[112];
#endif
    int (* getDefaultNetworkMaxPlayers)(void*);
#if MC_VERSION >= 1021050
    void* pad3[34];
#elif MC_VERSION >= 1021040
    void* pad3[33];
#else
    void* pad3[32];
#endif
    int (* getMaxSimRadiusInChunks)(void*);
#if MC_VERSION >= 1021030
    void* pad4[18];
#elif MC_VERSION >= 1020060
    void* pad4[17];
#else
    void* pad4[18];
#endif
    std::string (* getEdition)(void*);
};

extern "C" __attribute__ ((visibility ("default"))) void mod_preinit() {
    auto h = dlopen("libmcpelauncher_mod.so", 0);

    auto mcpelauncher_preinithook = (void (*)(const char*, void*, void**)) dlsym(h, "mcpelauncher_preinithook");

    static ANativeActivity_createFunc* onCreate_orig;

    mcpelauncher_preinithook("ANativeActivity_onCreate", (void*) +[](ANativeActivity* activity, void* savedState, size_t savedStateSize) {
        onCreate_orig(activity, savedState, savedStateSize);

        static auto onStart_orig = activity->callbacks->onStart;

        activity->callbacks->onStart = +[](ANativeActivity* activity) {
            onStart_orig(activity);

            auto vt = *((AppPlatform_vtable****) activity->instance)[0][1];

            vt->blankLineDismissesChat = +[](void*) -> bool {
                return true;
            };

            vt->supportsFilePicking = +[](void*) -> bool {
                return true;
            };

            vt->getDefaultNetworkMaxPlayers = +[](void*) -> int {
                return 8;
            };

            vt->getMaxSimRadiusInChunks = +[](void*) -> int {
                return 12;
            };

            vt->getEdition = +[](void*) -> std::string {
                return "win10";
            };
        };
    }, (void**) &onCreate_orig);
}

extern "C" __attribute__ ((visibility ("default"))) void mod_init() {}
