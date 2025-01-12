template<>
struct AppPlatform_vtable<MC_VERSION> {
#if MC_VERSION >= 1021040
    void* pad0[40];
#elif MC_VERSION >= 1021020
    void* pad0[38];
#else
    void* pad0[39];
#endif
    void* blankLineDismissesChat;
    void* pad1[42];
    void* supportsFilePicking;
#if MC_VERSION >= 1021020
    void* pad2[115];
#elif MC_VERSION >= 1021000
    void* pad2[114];
#elif MC_VERSION >= 1020080
    void* pad2[113];
#else
    void* pad2[112];
#endif
    void* getDefaultNetworkMaxPlayers;
#if MC_VERSION >= 1021050
    void* pad3[34];
#elif MC_VERSION >= 1021040
    void* pad3[33];
#else
    void* pad3[32];
#endif
    void* getMaxSimRadiusInChunks;
#if MC_VERSION >= 1021030
    void* pad4[18];
#elif MC_VERSION >= 1020060
    void* pad4[17];
#else
    void* pad4[18];
#endif
    void* getEdition;
};
#undef MC_VERSION
