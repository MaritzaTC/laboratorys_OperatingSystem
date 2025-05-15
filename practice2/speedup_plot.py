import matplotlib.pyplot as plt

# Número de procesos (para cada conjunto de datos)
processes = [1, 2, 3, 4]

# Speedup para matrices 12x15 y 15x10 (ejemplo)
speedup_small = [0.05, 0.03, 0.02, 0.02]

# Speedup para matrices 2x3 y 3x2 (ejemplo)
speedup_very_small = [0.05, 0.02, 0.02, 0.02]

# Speedup para matrices 400x360 y 360x400
speedup_large = [26.99, 41.26, 26.34, 20.33]

plt.figure(figsize=(8,5))

plt.plot(processes, speedup_small, marker='o', label='12x15 x 15x10')
plt.plot(processes, speedup_very_small, marker='o', label='2x3 x 3x2')
plt.plot(processes, speedup_large, marker='o', label='400x360 x 360x400')

plt.title('Speedup vs Número de Procesos')
plt.xlabel('Número de Procesos')
plt.ylabel('Speedup')
plt.xticks(processes)
plt.legend()
plt.grid(True)

plt.show()
