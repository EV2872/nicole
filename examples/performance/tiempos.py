import matplotlib.pyplot as plt

# Datos de ejemplo: tiempos en segundos para 4 pruebas
pruebas = [1, 2, 3, 4]
tiempos = {
    "Nicole jit": [2.751, 2.824, 2.852, 2.646],
    "Nicole jit optimizado": [2.561, 2.990, 2.729, 2.796],
    "Nicole compilado": [2.621, 2.508, 2.604, 2.744],
    "Nicole compilado optimizado": [1.615, 1.730, 2.361, 2.658],
    "C++": [5.084, 4.885, 4.817, 4.916],
    "C++ optimizado": [4.492, 4.362, 2.438, 3.531],
    "Python": [14.478, 14.100, 11.877, 11.474],
    "JavaScript": [33.348, 32.916, 32.584, 34.897]
}

plt.figure()
# Puedes añadir más colores si tienes más series
colores = ["blue", "red", "green", "orange", "purple", "brown", "cyan", "magenta"]
for (lang, tiempos_lang), color in zip(tiempos.items(), colores):
    plt.plot(pruebas, tiempos_lang, label=lang, color=color, marker='o')

plt.xticks(pruebas, ["Prueba 1", "Prueba 2", "Prueba 3", "Prueba 4"])
plt.xlabel("Prueba")
plt.ylabel("Tiempo (s)")           # <-- etiqueta ajustada a segundos
plt.title("Comparativa de tiempos de ejecución")
plt.legend()
plt.tight_layout()
plt.show()
