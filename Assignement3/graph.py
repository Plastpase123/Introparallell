import matplotlib.pyplot as plt

# Data
threads = [1, 2, 4, 8, 16]
execution_times_64_1000 = [0.113256, 0.069845, 0.047118, 0.041676, 0.041292]
execution_times_64_2000 = [0.216724, 0.133654, 0.086220, 0.074347, 0.077412]
execution_times_1024_1000 = [31.245364, 20.383762, 13.985352, 11.275253, 8.734737 ]
execution_times_1024_2000 = [62.352907, 40.282700, 25.358061 , 19.157259, 17.894570]
execution_times_4096_1000 = [449.151794, 328.555457, 201.409515, 173.124933, 136.097461]
execution_times_4096_2000 = [896.586425, 654.267667, 412.299804, 312.081321, 272.041739]


# Calculating speedup
speedup_64_1000 = [ execution_times_64_1000[0] / t for t in execution_times_64_1000]
speedup_64_2000 = [execution_times_64_2000[0] / t for t in execution_times_64_2000]
speedup_1024_1000 = [execution_times_1024_1000[0] / t for t in execution_times_1024_1000]
speedup_1024_2000 = [execution_times_1024_2000[0] / t for t in execution_times_1024_2000]
speedup_4096_1000 = [execution_times_4096_1000[0] / t for t in execution_times_4096_1000]
speedup_4096_2000 = [execution_times_4096_2000[0] / t for t in execution_times_4096_2000]
# Plot
plt.figure(figsize=(10,6))
plt.plot(threads, speedup_64_1000, marker='o', label='64x64, 1000 steps')
plt.plot(threads, speedup_64_2000, marker='o', label='64x64, 2000 steps')
plt.plot(threads, speedup_1024_1000, marker='o', label='1024x1024, 1000 steps')
plt.plot(threads, speedup_1024_2000, marker='o', label='1024x1024, 2000 steps')
plt.plot(threads, speedup_4096_1000, marker='o', label='4096x4096, 1000 steps')
plt.plot(threads, speedup_4096_2000, marker='o', label='4096x4096, 2000 steps')

# Log scale for the x-axis
# plt.xscale('log')

# Labels and Title
plt.xlabel('Number of Threads')
plt.ylabel('Speedup')
plt.title('Speedup vs Number of Threads')
plt.legend()

# Grid and Show plot
plt.grid(True, which="both", ls="--")
plt.show()
