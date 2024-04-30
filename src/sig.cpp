#include "sig.hpp"
#include <algorithm>
#include <span>
#include <sstream>
#include <tuple>
#include <vector>
#include <dlfcn.h>
#include <link.h>

uintptr_t findSig(const std::string& str, void* handle) {
    std::vector<std::pair<uint8_t, bool>> sig;
    uintptr_t result{};

    std::istringstream ss(str);
    std::string byteStr;
    while (ss >> byteStr) {
        if (byteStr == "?")
            sig.emplace_back(0, true);
        else
            sig.emplace_back(std::stoi(byteStr, nullptr, 16), false);
    }

    auto data = std::make_tuple(&sig, handle, &result);

    dl_iterate_phdr([](dl_phdr_info* info, size_t, void* data) -> int {
        std::vector<std::pair<uint8_t, bool>>* sig;
        void* handle;
        uintptr_t* result;
        std::tie(sig, handle, result) = *static_cast<std::tuple<decltype(sig), void*, uintptr_t*>*>(data);

        auto h = dlopen(info->dlpi_name, RTLD_NOLOAD);
        dlclose(h);
        if (h != handle)
            return 0;

        for (auto& phdr: std::span(info->dlpi_phdr, info->dlpi_phnum)) {
            if (!(phdr.p_flags & PF_X))
                continue;

            auto base = reinterpret_cast<uint8_t*>(info->dlpi_addr + phdr.p_vaddr);
            auto it = std::search(base, base + phdr.p_memsz, sig->begin(), sig->end(), [](uint8_t lhs, std::pair<uint8_t, bool> rhs) {
                return rhs.second || lhs == rhs.first;
            });
            if (it != base + phdr.p_memsz) {
                *result = reinterpret_cast<uintptr_t>(it);
                return 1;
            }
        }

        return 0;
    }, &data);

    return result;
}
