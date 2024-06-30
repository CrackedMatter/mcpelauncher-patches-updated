#include <android/native_activity.h>
#include <dlfcn.h>
#include <string>

extern "C" __attribute__ ((visibility ("default"))) void mod_preinit() {
    auto h = dlopen("libmcpelauncher_mod.so", 0);

    auto mcpelauncher_preinithook = (void (*)(const char*, void*, void**)) dlsym(h, "mcpelauncher_preinithook");

    static ANativeActivity_createFunc* onCreate_orig;

    mcpelauncher_preinithook("ANativeActivity_onCreate", (void*) +[](ANativeActivity* activity, void* savedState, size_t savedStateSize) {
        onCreate_orig(activity, savedState, savedStateSize);

        static auto onStart_orig = activity->callbacks->onStart;

        activity->callbacks->onStart = +[](ANativeActivity* activity) {
            onStart_orig(activity);

            auto vt = ((void*****) activity->instance)[0][1][0];

            // blankLineDismissesChat
            vt[39] = (void*) +[](void*) -> bool {
                return true;
            };

            // supportsFilePicking
            vt[82] = (void*) +[](void*) -> bool {
                return true;
            };

            // getDefaultNetworkMaxPlayers
            vt[197] = (void*) +[](void*) -> int {
                return 8;
            };

            // getMaxSimRadiusInChunks
            vt[230] = (void*) +[](void*) -> int {
                return 12;
            };

            // getEdition
            vt[248] = (void*) +[](void*) -> std::string {
                return "win10";
            };
        };
    }, (void**) &onCreate_orig);
}

extern "C" __attribute__ ((visibility ("default"))) void mod_init() {}
