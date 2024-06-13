#include "sig.hpp"
#include <android/log.h>
#include <dlfcn.h>

extern "C" void __attribute__ ((visibility ("default"))) mod_preinit() {}

extern "C" void __attribute__ ((visibility ("default"))) mod_init() {
    auto mc = dlopen("libminecraftpe.so", 0);

    auto addr = findSig("48 8D 05 ? ? ? ? 48 89 03 48 8D 05 ? ? ? ? 48 89 43 18 48 B8 56 03 00 00 E0 01", mc);

    if (!addr) {
        __android_log_print(ANDROID_LOG_ERROR, "Patches", "Failed to find AppPlatform_android vtable");
        return;
    }

    auto vt = (void**) (addr + *(int*) (addr + 3) + 7);

    // supportsFilePicking
    vt[82] = (void*) +[](void*) -> bool {
        return true;
    };

    // getMaxSimRadiusInChunks
    vt[230] = (void*) +[](void*) -> int {
        return 12;
    };

    // getEdition
    vt[248] = (void*) +[](void*) -> std::string {
        return "win10";
    };

    // getDefaultNetworkMaxPlayers
    vt[197] = (void*) +[](void*) -> int {
        return 8;
    };

    // blankLineDismissesChat
    vt[39] = (void*) +[](void*) -> bool {
        return true;
    };
}
