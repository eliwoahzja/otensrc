#pragma once

bool (*orig_bypass)(void *ins);
bool hook_bypass(void *ins) {
    return false;
}

#if defined(__aarch64__)

inline const char *armFalse = "C0 03 5F D6";

#endif

struct range {
    uintptr_t Irt, Ind;
    struct Iter {
      uintptr_t val;
      bool operator!=(const Iter& it) const {
        return val <= it.val; 
      }
      uintptr_t operator*() const {
        return val; 
      }
      void operator++() {
        val += 4; 
      }
    };
    Iter begin() const { 
      return {Irt}; 
    }
    Iter end() const {
      return {Ind}; 
    }
};

inline void InitializeProtection() {
    


    for (auto offs : range{0x1, 0x1000}) {
        MemoryPatch::createWithHex("libanogs.so", offs, armFalse).Modify();
    }
    
}
    
    
