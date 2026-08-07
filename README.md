*Read this in other languages: [Русский](#russian-version).*

# Meltdown

A basic version of Meltdown utilizing TSX (Transactional Synchronization Extensions). Unfortunately, Intel decided to deprecate and remove this extension, so it is no longer supported on many newer processor iterations. 

You might also notice a "BURN" loop. While it theoretically does nothing, it proved to be useful in practice. Most likely, a single iteration of "BURN" is sufficient, and there is no overall downside to keeping it.

Regarding the cache miss threshold, one of the papers mentioned that 120 cycles is the optimal value. I decided not to argue with that or benchmark it myself.

It is also worth noting that this implementation specifically targets the L1 and L2 caches. We read bytes (due to the small cache size) and pinned the process to a specific core using CPU affinity (since L1 and L2 caches are private to each core). However, papers often argue that targeting the L3 cache is a superior approach due to its larger capacity and because it is shared across all cores.

This is a fairly lightweight, simplified version. In a real-world scenario, `/proc/read_addr` would need to be modified and fine-tuned.

### References
* Meltdown - https://meltdownattack.com/meltdown.pdf
* Flush+Reload (includes threshold details) - https://eprint.iacr.org/2013/448.pdf

---

<a id="russian-version"></a>
# Meltdown (Русский)

Базовая версия Meltdown с TSX (Transactional Synchronization Extensions). К сожалению, это расширение Intel решил удалить, поэтому далеко не во всех версиях оно поддерживается. 

При этом также можно заметить "BURN", который фактически ничего не дает, но на практике оказался полезным. Скорее всего, 1 итерации "BURN" достаточно, глобального минуса от него нет. 

Насчет threshold: в какой-то статье писалось, что 120 — самый оптимальный вариант, я не стал спорить и тестировать.

Стоит также отметить, что реализация была сделана для L1, L2 кэша. Т.е. мы читаем байты (так как у нас память кэша маленькая) и зафиксировали процесс к ядру (так как L1, L2 кэши — свои для каждого ядра). Стоит отметить, что в статьях утверждалось, что L3 кэш — лучший вариант из-за своего размера и того, что он общий для всех ядер. 

Это достаточно легкая версия, в реальности `/proc/read_addr` нужно менять и допиливать.

### Статьи
* Meltdown - https://meltdownattack.com/meltdown.pdf
* Flush+Reload (там же threshold) - https://eprint.iacr.org/2013/448.pdf