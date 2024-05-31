import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('out/recibidos.csv')

df['timestamp'] = (df['timestamp'] - df['timestamp'].min()) / 1000.0

CB_color_cycle = (
    "#377eb8",
    "#ff7f00",
    "#4daf4a",
    "#f781bf",
    "#a65628",
    "#984ea3",
    "#999999",
    "#e41a1c",
    "#dede00",
)

# Crear los gráficos
plt.figure(figsize=(12, 6))

# Gráfico del output respecto al tiempo
plt.plot(df['timestamp'], df['value'], linestyle='-', color=CB_color_cycle[0], label='Intensidad')
plt.xlabel('Tiempo [S]')
plt.ylabel('Salida [%]')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
