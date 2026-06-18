import pandas as pd
import matplotlib.pyplot as plt

# le os arquivos gerados pelo programa em C
graus = pd.read_csv("graus.csv")
componentes = pd.read_csv("componentes.csv")

# 1. gráfico das top 10 palavras com maior grau

top10 = graus.sort_values(by="grau", ascending=False).head(10)

plt.figure(figsize=(9, 5))
plt.barh(top10["palavra"], top10["grau"])
plt.gca().invert_yaxis()

plt.title("Top 10 palavras com maior grau")
plt.xlabel("Grau")
plt.ylabel("Palavra")

# mostra o valor do grau ao lado de cada barra
for i, valor in enumerate(top10["grau"]):
    plt.text(valor + 0.1, i, str(valor), va="center")

plt.tight_layout()
plt.savefig("top10_palavras_grau.png", dpi=300)
plt.show()

# 2. histograma dos tamanhos dos componentes conexos

plt.figure(figsize=(9, 5))
plt.hist(componentes["tamanho"], bins=20, edgecolor="black")

plt.title("Distribuição dos tamanhos dos componentes conexos")
plt.xlabel("Tamanho do componente")
plt.ylabel("Quantidade de componentes")

plt.tight_layout()
plt.savefig("histograma_componentes.png", dpi=300)
plt.show()