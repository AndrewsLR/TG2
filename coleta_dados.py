import pandas as pd
import matplotlib.pyplot as plt

#Pega dados do arquivo saida
data = pd.read_csv('saida.txt', sep=" ", header=None)
data.columns = ["eq","gaps","lines","routable"]
gaps = data["gaps"].value_counts()
print(gaps)
lines = data["lines"].value_counts()
print(lines)
rotable = data["routable"].value_counts()

#print(data.query("lines == 0"))
#equacao = data.loc[2].at["eq"]
#count = sum([1 for c in equacao if c.isalpha()])

#Adiciona coluna inputs no dataframe
data["inputs"] = ""
#Preenche coluna com numero de entradas
for index, row in data.iterrows():
    data.at[index,"inputs"] = sum([1 for c in row["eq"] if c.isalpha()])
print(data)
data.to_csv('dados.csv')
inputs = data["inputs"].value_counts()
print(inputs)

plt.figure(figsize=(8,6))
plt.hist(data["gaps"], bins= range(0, 7))
plt.title('Histogram of Gap Number from genlib')
plt.xlabel('Number of Gaps')
plt.ylabel('Number of Equations')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.show()

for value in sorted(data["gaps"].unique()):  # Sorting ensures ordered histograms
    subset = data[data["gaps"] == value]  # Filter rows where inputs == value
    print(subset)
    plt.figure(figsize=(8,6))
    plt.hist(subset["inputs"], bins=range(1, 18), edgecolor="black")
    plt.title(f'Histogram for Gaps = {value}')
    plt.xlabel('Number of Inputs')
    plt.ylabel('Number of Equations')
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    counts, bins, _ = plt.hist(subset["inputs"], bins=range(1, 18), edgecolor="black")
    for i in range(len(counts)):
        plt.text(bins[i] + 0.5, counts[i] + 0.1, str(int(counts[i])), ha='right', va='bottom', fontsize=16, color='black')
    plt.show()
for index, row in data.iterrows():
    if(data.at[index,"gaps"] == 3 and data.at[index,"inputs"] == 16 and data.at[index,"lines"] == 7):
        print(data.at[index,"eq"])

plt.figure(figsize=(8,6))
counts, bins, patches = plt.hist(data["lines"], bins= range(1, 9))      #pega altura (counts) barra(bins) e as barras como objeto(patches)
for count, x in zip(counts, bins):                                      # conta junto counts e bins usando zip
    plt.text(x + 0.5, count + 0.1, str(int(count)),                     #coloca nas posições de x e count e escreve o valor de count
             ha='center', va='bottom', fontsize=10)
plt.title('Histogram of Line Number from genlib')
plt.xlabel('Number of Lines')
plt.ylabel('Number of Equations')
plt.grid(axis='y', linestyle='--', alpha=0.7)

plt.show()
