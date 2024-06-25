from cpufreq import cpuFreq
from collections import Counter

cpu= cpuFreq()
cpu.reset()
cpu.disable_hyperthread()

freqs= cpu.get_frequencies()
print(freqs)