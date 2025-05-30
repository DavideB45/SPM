import csv
from collections import defaultdict
import os

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

    for size in times_by_size:
        times_by_size[size].sort()
        if len(times_by_size[size]) > 4:
            times_by_size[size] = times_by_size[size][2:-2]
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
                x.append(int(row[0]))
                y.append(float(row[1]))
        plt.plot(x, y, label=path.split('_')[1].split('.')[0], marker='x')
    
    plt.xlabel('Array Size')
    plt.ylabel('Average Time (s)')
    plt.title('Average Time vs Array Size')
    plt.grid()
    #plt.xscale('log')
    #plt.yscale('log')
    plt.legend()
    plt.savefig('average_time_small.png', dpi=300)
    plt.show()

def plot_speedup(csv_path):
    import matplotlib.pyplot as plt
    x = [[], [], []]
    y = [[], [], []]
    for index, path in enumerate(csv_path):
        with open(path, 'r', newline='') as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                x[index].append(int(row[0]))
                y[index].append(float(row[1]))

    speedup_auto = []
    speedup_avx = []
    speedup_aa = []
    for i in range(len(x[0])):
        speedup_auto.append(y[0][i] / y[1][i])
        speedup_avx.append(y[0][i] / y[2][i])
        speedup_aa.append(y[1][i] / y[2][i])
    plt.plot(x[1], speedup_auto, label='Plain vs Auto', marker='o')
    plt.plot(x[2], speedup_avx , label='Plain vs AVX',  marker='o')
    plt.plot(x[1], speedup_aa, label='Auto vs AVX', marker='o')
    plt.xlabel('Array Size')
    plt.ylabel('Speedup')
    plt.title('Speedup vs Array Size')
    plt.grid()
    plt.legend()
    plt.show()

if __name__ == "__main__":
    #calculate_averages("plain_results_big.csv", "avg_plain.csv")
    #calculate_averages("auto_results_big.csv", "avg_auto.csv")
    #calculate_averages("avx_results_big.csv", "avg_avx.csv")
    calculate_averages("plain_results_small.csv", "avg_plain.csv")
    calculate_averages("auto_results_small.csv", "avg_auto.csv")
    calculate_averages("avx_results_small.csv", "avg_avx.csv")
    plot_average(["avg_plain.csv", "avg_auto.csv", "avg_avx.csv"])
    plot_speedup(["avg_plain.csv", "avg_auto.csv", "avg_avx.csv"])
    os.remove("avg_plain.csv")
    os.remove("avg_auto.csv")
    os.remove("avg_avx.csv")