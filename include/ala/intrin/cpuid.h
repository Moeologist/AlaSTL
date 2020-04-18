#ifndef _ALA_INTRIN_CPUID_H
#define _ALA_INTRIN_CPUID_H

#include <cstring>
#include "ala/type_traits.h"

#ifdef _ALA_MSVC

#ifdef __cplusplus
extern "C" {
#endif

// #include <intrin.h>
extern void __cpuid(int[4], int);
extern void __cpuidex(int[4], int, int);

#ifdef __cplusplus
}
#endif

#else
#include <cpuid.h>
#endif

namespace ala {

// Read
// AMD64 Architecture Programmer’s Manual Volume 3 - Appendix D
// and
// Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 2 - 3.2
struct CPUIDInfo {
    struct bits32 {
        uint_fast32_t data = 0;

        bits32(uint_fast32_t _data): data(_data) {}

        operator uint_fast32_t() {
            return data;
        }

        bool operator[](size_t index) {
            return (data >> index) & 1;
        }

        uint_fast32_t operator()(size_t start, size_t end) {
            return (data >> start) & (0xffffffff >> (32 - end + start));
        }
    };
    union {
        uint_fast32_t data[4];
        struct {
            bits32 eax, ebx, ecx, edx;
        };
    };

    bool operator()(unsigned int leaf, unsigned int subleaf = 0) {
#ifdef _ALA_MSVC
        __cpuid((int *)data, leaf & 0x80000000);
        unsigned int max_leaf = eax;
        if (max_leaf == 0 || max_leaf < leaf)
            return false;
        __cpuidex((int *)data, leaf, subleaf);
        return true;
#else
        __cpuid(leaf & 0x80000000, data[0], data[1], data[2], data[3]);
        unsigned int max_leaf = eax;
        if (max_leaf == 0 || max_leaf < leaf)
            return false;
        __cpuid_count(leaf, subleaf, data[0], data[1], data[2], data[3]);
        return true;
#endif
    }

    CPUIDInfo(unsigned int leaf, unsigned int subleaf = 0) {
        (*this)(leaf, subleaf);
    }

    /*
    leaf (function) = 0H, subleaf (subfunction) = 0H
    See https://en.wikipedia.org/wiki/CPUID

    The following are known processor manufacturer ID strings:

    "AMDisbetter!" – early engineering samples of AMD K5 processor
    "AuthenticAMD" – AMD
    "CentaurHauls" – Centaur (Including some VIA CPU)
    "CyrixInstead" – Cyrix
    "HygonGenuine" – Hygon
    "GenuineIntel" – Intel
    "TransmetaCPU" – Transmeta
    "GenuineTMx86" – Transmeta
    "Geode by NSC" – National Semiconductor
    "NexGenDriven" – NexGen
    "RiseRiseRise" – Rise
    "SiS SiS SiS " – SiS
    " Shanghai "   – Zhaoxin
    "UMC UMC UMC " – UMC
    "VIA VIA VIA " – VIA
    "Vortex86 SoC" – Vortex

    The following are known ID strings from virtual machines:

    "bhyve bhyve " – bhyve
    "KVMKVMKVM" – KVM
    "Microsoft Hv" – Microsoft Hyper-V or Windows Virtual PC
    " lrpepyh vr" – Parallels (it possibly should be "prl hyperv ", but it is encoded as " lrpepyh vr" due to an endianness mismatch)
    "VMwareVMware" – VMware
    "XenVMMXenVMM" – Xen HVM
    "ACRNACRNACRN" – Project ACRN
    */
    static char *GetManufacturerId() {
        static char mf_id[13] = {};
        CPUIDInfo info(0);
        std::memcpy(mf_id + 0, &info.ebx, 4);
        std::memcpy(mf_id + 4, &info.edx, 4);
        std::memcpy(mf_id + 8, &info.ecx, 4);
        return mf_id;
    }

    /*
    leaf (function) = 1H, subleaf (subfunction) = 0H
    EAX:
    Reserved | ExtFamily | ExtModel | Reserved | BaseFamily | BaseModel | Stepping
    [31:28]  | [27:20]   | [19:16]  | [15:12]  | [11:8]     | [7:4]     | [3:0]
    EBX:
    LocalApicId | LogicalProcessor Count | CLFlush | 8BitBrandId
    [31:25]     | [24:16]                | [15:8]  | [7:0]
    ECX:
    0 | RDRAND | F16C | AVX | OSXSAVE | XSAVE | AES | TSC-Deadline | POPCNT | MOVBE | x2APIC | SSE4.2 | SSE4.1 | DCA | PCID | Reserved | PDCM | xTPR Update Control | CMPXCHG16B | FMA | SDBG | CNXT-ID | SSSE3 | TM2  | EIST | SMX | VMX | DS-CPL | MONITOR | DTES64 | PCLMULQDQ | SSE3
    EDX:
    PBE | Reserved | TM | HTT | SS | SSE2 | SSE | FXSR | MMX | ACPI | DS | Reserved | CLFSH | PSN | PSE-36  | PAT | CMOV | MCA | PGE | MTRR | SEP | Reserved | APIC | CX8 | MCE | PAE | MSR | TSC | PSE | DE | VME | FPU
    */
    static unsigned int GetStepping() {
        CPUIDInfo info(1);
        return info.eax(0, 3);
    }

    static unsigned int GetBaseModel() {
        CPUIDInfo info(1);
        return info.eax(4, 7);
    }

    static unsigned int GetBaseFamily() {
        CPUIDInfo info(1);
        return info.eax(8, 11);
    }

    static unsigned int GetExtModel() {
        CPUIDInfo info(1);
        return info.eax(16, 19);
    }

    static unsigned int GetExtFamily() {
        CPUIDInfo info(1);
        return info.eax(20, 27);
    }

    // static unsigned int Get8BitBrandId() {
    //     CPUIDInfo info(1);
    //     return ebx & 0xff;
    // }

    // static unsigned int GetCLFlush() {
    //     CPUIDInfo info(1);
    //     return (ebx >> 8) & 0xff;
    // }

    // static unsigned int GetLogicalProcessorCount() {
    //     CPUIDInfo info(1);
    //     return (ebx >> 16) & 0xff;
    // }

    // static unsigned int GetLocalApicId() {
    //     CPUIDInfo info(1);
    //     return (ebx >> 24) & 0xff;
    // }

    static bool GetSSE3() {
        CPUIDInfo info(1);
        return info.ecx[0];
    }

    static bool GetSSSE3() {
        CPUIDInfo info(1);
        return info.ecx[9];
    }

    static bool GetFMA() {
        CPUIDInfo info(1);
        return info.ecx[12];
    }

    static bool GetSSE41() {
        CPUIDInfo info(1);
        return info.ecx[19];
    }

    static bool GetSSE42() {
        CPUIDInfo info(1);
        return info.ecx[20];
    }

    static bool GetAVX() {
        CPUIDInfo info(1);
        return info.ecx[28];
    }

    static bool GetF16C() {
        CPUIDInfo info(1);
        return info.ecx[29];
    }

    static bool GetRDRAND() {
        CPUIDInfo info(1);
        return info.ecx[30];
    }

    // Others

    static bool GetHLE() {
        CPUIDInfo info(7);
        return info.ebx[4];
    }

    static bool GetAVX2() {
        CPUIDInfo info(7, 0);
        return info.ebx[5];
    }

    static bool GetAVX512F() {
        CPUIDInfo info(7);
        return info.ebx[16];
    }

    static bool GetAVX512DQ() {
        CPUIDInfo info(7);
        return info.ebx[17];
    }

    static bool GetRDSEED() {
        CPUIDInfo info(7);
        return info.ebx[18];
    }

    static bool GetAVX512_IFMA() {
        CPUIDInfo info(7);
        return info.ebx[21];
    }

    static bool GetAVX512PF() {
        CPUIDInfo info(7);
        return info.ebx[26];
    }

    static bool GetAVX512ER() {
        CPUIDInfo info(7);
        return info.ebx[27];
    }

    static bool GetAVX512CD() {
        CPUIDInfo info(7);
        return info.ebx[28];
    }

    static bool GetSHA() {
        CPUIDInfo info(7);
        return info.ebx[29];
    }

    static bool GetAVX512BW() {
        CPUIDInfo info(7);
        return info.ebx[30];
    }

    static bool GetAVX512VL() {
        CPUIDInfo info(7);
        return info.ebx[31];
    }

    static bool GetAVX512_VBMI() {
        CPUIDInfo info(7);
        return info.ecx[1];
    }

    static bool GetAVX512_VBMI2() {
        CPUIDInfo info(7);
        return info.ecx[6];
    }

    static bool GetAVX512_VNNI() {
        CPUIDInfo info(7);
        return info.ecx[11];
    }

    static bool GetAVX512_BITALG() {
        CPUIDInfo info(7);
        return info.ecx[12];
    }

    static bool GetAVX512_VPOPCNTDQ() {
        CPUIDInfo info(7);
        return info.ecx[14];
    }

    static bool GetAVX512_4VNNIW() {
        CPUIDInfo info(7);
        return info.edx[2];
    }

    static bool GetAVX512_4FMAPS() {
        CPUIDInfo info(7);
        return info.edx[3];
    }

    // static unsigned int GetPhysicsProcessorCount() {
    //     CPUIDInfo info(0);
    //     if (ebx == 0x68747541 && ecx == 0x444D4163 && edx == 0x69746E65) { // AMD
    //         CPUIDInfo info(0x80000008);
    //         return (ecx & 0xff) + 1;
    //     } else if (ebx == 0x756E6547 && ecx == 0x6C65746E &&
    //                edx == 0x49656E69) { // Intel
    //         CPUIDInfo info(4);
    //         return ((eax >> 26) & 0x3f) + 1;
    //     }
    //     return 0;
    // }

    static char *GetProcessorName() {
        static char pc_name[49] = {};
        CPUIDInfo info(0x80000002);
        std::memcpy(pc_name, info.data, 16);
        info(0x80000003);
        std::memcpy(pc_name + 16, info.data, 16);
        info(0x80000004);
        std::memcpy(pc_name + 32, info.data, 16);
        return pc_name;
    }
};
} // namespace ala

#endif // HEAD