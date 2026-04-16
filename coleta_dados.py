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

<<<<<<< Updated upstream
plt.figure(figsize=(8,6))
plt.hist(data["gaps"], bins= range(0, 7))
plt.title('Histogram of Gap Number from genlib')
plt.xlabel('Number of Gaps')
plt.ylabel('Number of Equations')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.show()
=======
base = alt.Chart(data).mark_bar().encode(
    x=alt.X("inputs:O", title="Number of Inputs"),       # O = ordinal/categorical
    y=alt.Y("count():Q", title="Number of Functions")     # count occurrences
)
bars = base.mark_bar(color='#4C78A8', stroke="black", strokeWidth=1.0)
labels = base.mark_text(align="center", baseline="bottom", dy=-5, color="black").encode(text="count():Q")
chart1 = (bars + labels).properties(title="Distribution of Functions by Inputs")



base = alt.Chart(data).mark_bar().encode(
    x=alt.X("gaps:O", title="Number of Gaps"),       # O = ordinal/categorical
    y=alt.Y("count():Q", title="Number of Functions")     # count occurrences
)
bars = base.mark_bar(color='#4C78A8', stroke="black", strokeWidth=1.0)
labels = base.mark_text(align="center", baseline="bottom", dy=-5, color="black").encode(text="count():Q")
chart2 = (bars + labels).properties(title="Distribution of Functions by Number of Gaps")



combined = chart1 | chart2
combined.save("graphs/teste.png", scale_factor=4.0)

chart1 = chart1.configure(
    font="Liberation Serif"
).configure_title(
    font="Liberation Serif",
    fontSize=14
).configure_axis(
    labelFont="Liberation Serif",
    titleFont="Liberation Serif"
).configure_text(
    font="Liberation Serif"
)
chart1.save("graphs/input_number.png", scale_factor=4.0)

#chart2 = chart2.configure(
#    font="Liberation Serif"
#).configure_title(
#    font="Liberation Serif",
#    fontSize=14
#).configure_axis(
#    labelFont="Liberation Serif",
#    titleFont="Liberation Serif"
#).configure_text(
#    font="Liberation Serif"
#)
chart2.save("graphs/gap_number.png", scale_factor=4.0)

>>>>>>> Stashed changes

for value in sorted(data["gaps"].unique()):  # Sorting ensures ordered histograms
    subset = data[data["gaps"] == value]  # Filter rows where inputs == value
    print(subset)
<<<<<<< Updated upstream
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
=======
    base = alt.Chart(subset).mark_bar().encode(x=alt.X("inputs:O", title="Number of Inputs"),y=alt.Y("count():Q", title="Number of Functions"))
    bars = base.mark_bar(color='#4C78A8', stroke="black", strokeWidth=1.0)
    labels = base.mark_text(align="center", baseline="bottom", dy=-5, color="black").encode(text="count():Q")
    chart = (bars + labels).properties(title=f"Histogram of Input Number for Gaps = {value}")
    
    chart = chart.configure(
    font="Liberation Serif"
    ).configure_title(
    font="Liberation Serif",
    fontSize=14
    ).configure_axis(
    labelFont="Liberation Serif",
    titleFont="Liberation Serif"
    ).configure_text(
    font="Liberation Serif"
    )

    chart.save(f"graphs/gaps_{value}.png",scale_factor=4.0)
>>>>>>> Stashed changes

for index, row in data.iterrows():
    if(data.at[index,"gaps"] == 3 and data.at[index,"inputs"] == 16 and data.at[index,"lines"] == 7):
        print(data.at[index,"eq"])

<<<<<<< Updated upstream
plt.figure(figsize=(8,6))
counts, bins, patches = plt.hist(data["lines"], bins= range(1, 9))      #pega altura (counts) barra(bins) e as barras como objeto(patches)
for count, x in zip(counts, bins):                                      # conta junto counts e bins usando zip
    plt.text(x + 0.5, count + 0.1, str(int(count)),                     #coloca nas posições de x e count e escreve o valor de count
             ha='center', va='bottom', fontsize=10)
plt.title('Histogram of Line Number from genlib')
plt.xlabel('Number of Lines')
plt.ylabel('Number of Equations')
plt.grid(axis='y', linestyle='--', alpha=0.7)
=======
base = alt.Chart(data).mark_bar().encode(x=alt.X("lines:O", title="Number of Lines"),y=alt.Y("count():Q", title="Number of Functions"))
bars = base.mark_bar(color='#4C78A8', stroke="black", strokeWidth=1.0)
labels = base.mark_text(align="center", baseline="bottom", dy=-5, color="black").encode(text="count():Q")
chart = (bars + labels).properties(title=f"Distribution of Functions by Number of Lines")

combine = chart | chart2
combine = combine.configure(
    font="Liberation Serif"
).configure_title(
    font="Liberation Serif",
    fontSize=14
).configure_axis(
    labelFont="Liberation Serif",
    titleFont="Liberation Serif"
).configure_text(
    font="Liberation Serif"
)
combine.save("graphs/gap_line.png",scale_factor=4.0)
chart.save("graphs/line_number.png",scale_factor=4.0)
>>>>>>> Stashed changes

plt.show()

for value in sorted(data["routable"].unique()):  # Sorting ensures ordered histograms
    subset = data[data["routable"] == value]  # Filter rows where inputs == value
    print(subset)
    plt.figure(figsize=(8,6))
    plt.hist(subset["inputs"], bins=range(1, 18), edgecolor="black")
    plt.title(f'Histogram for Routablility = {value}')
    plt.xlabel('Number of Inputs')
    plt.ylabel('Number of Equations')
    plt.grid(axis='y', linestyle='--', alpha=0.7)

<<<<<<< Updated upstream
    counts, bins, _ = plt.hist(subset["inputs"], bins=range(1, 18), edgecolor="black")
    for i in range(len(counts)):
        plt.text(bins[i] + 0.5, counts[i] + 0.1, str(int(counts[i])), ha='right', va='bottom', fontsize=16, color='black')
    plt.show()

for value in sorted(data["routable"].unique()):  # Sorting ensures ordered histograms
    subset = data[data["routable"] == value]  # Filter rows where inputs == value
    print(subset)
    plt.figure(figsize=(8,6))
    plt.hist(subset["lines"], bins=range(1, 9), edgecolor="black")
    plt.title(f'Histogram for Routablility = {value}')
    plt.xlabel('Number of Lines')
    plt.ylabel('Number of Equations')
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    counts, bins, _ = plt.hist(subset["lines"], bins=range(1, 9), edgecolor="black")
    for i in range(len(counts)):
        plt.text(bins[i] + 0.5, counts[i] + 0.1, str(int(counts[i])), ha='right', va='bottom', fontsize=16, color='black')
    plt.show()
=======
for number in range(1,8):
    line = routable_data["lines"].value_counts().get(number,0)
    print(f"Routable functions with {number} lines")
    percentage = line/lines_total[number]*100
    print(line)
    print(f"Representing {percentage} of total {number} lines\n")
print("----------------------------------------------------------------------------------------------------------------")
for number in range(1,17):
    line = routable_data["inputs"].value_counts().get(number,0)
    print(f"Routable functions with {number} inputs")
    print(line)
    percentage = line/inputs_total[number]*100
    print(f"Representing {percentage} of total {number} inputs\n")
print("----------------------------------------------------------------------------------------------------------------")
for number in range(0,4):
    line = routable_data["gaps"].value_counts().get(number,0)
    print(f"Routable functions with {number} gaps")
    print(line)
    percentage = line/gaps_total[number]*100
    print(f"Representing {percentage} of total {number} gaps\n")
print("----------------------------------------------------------------------------------------------------------------")
>>>>>>> Stashed changes
