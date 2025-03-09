import csv
from collections import defaultdict

def calculate_averages(csv_path, output_path=None):
    if output_path is None:
        output_path = f"avg_{csv_path}"
    times_by_size = defaultdict(list)

    with open(csv_path, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        next(reader)  # skip header
        for row in reader:
            size = row[0]
            time = float(row[2])
            times_by_size[size].append(time)

    for size, times in times_by_size.items():
        avg_time = sum(times) / len(times)
        print(f"{size},{avg_time}")
        with open(output_path, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow([size, avg_time])

def plot_average(csv_path):
    import matplotlib.pyplot as plt
    for path in csv_path:
        x = []
        y = []
        with open(path, 'r', newline='') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if not (int(row[0]) >= 100003):
                    continue
                x.append(int(row[0]))
                y.append(float(row[1]))

        plt.plot(x, y, label=path.split('_')[1].split('.')[0], marker='o')
    
    plt.xlabel('Array Size')
    plt.ylabel('Average Time (s)')
    plt.title('Average Time vs Array Size')
    plt.grid()
    plt.xscale('log')
    plt.yscale('log')
    plt.legend()
    plt.show()

def plot_speedup(csv_path):
    import matplotlib.pyplot as plt
    x = [[], [], []]
    y = [[], [], []]
    for index, path in enumerate(csv_path):
        with open(path, 'r', newline='') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                if not (int(row[0]) >= 100003):
                    continue
                x[index].append(int(row[0]))
                y[index].append(float(row[1]))

    speedup_auto = []
    speedup_avx = []
    for i in range(len(x[0])):
        speedup_auto.append(y[0][i] / y[1][i])
        speedup_avx.append(y[0][i] / y[2][i])
    plt.plot(x[1], speedup_auto, label='Auto', marker='o')
    plt.plot(x[2], speedup_avx , label='AVX',  marker='o')
    plt.xlabel('Array Size')
    plt.ylabel('Speedup')
    plt.title('Speedup vs Array Size')
    plt.grid()
    plt.legend()
    plt.show()

if __name__ == "__main__":
    calculate_averages("plain_results_20250309_195641.csv", "avg_plain.csv")
    calculate_averages("auto_results_20250309_195956.csv", "avg_auto.csv")
    calculate_averages("avx_results_20250309_200133.csv", "avg_avx.csv")
    plot_average(["avg_plain.csv", "avg_auto.csv", "avg_avx.csv"])
    plot_speedup(["avg_plain.csv", "avg_auto.csv", "avg_avx.csv"])