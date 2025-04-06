import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def load_csv() -> pd.DataFrame:
	"""
	Load the CSV files and return the data as a dictionary.
	"""
	csv_files = ["collatz_results_1.csv", "collatz_results_2.csv", "collatz_results_3.csv"]
	
	
	df1 = pd.read_csv(csv_files[0])
	df2 = pd.read_csv(csv_files[1])
	df3 = pd.read_csv(csv_files[2])

	# Make a single dataframe averaging the time column
	# keep all the other columns (threads,chunk_size,problem,dynamic,free)
	avg_df = pd.DataFrame()
	avg_df = df1[["threads", "chunk_size", "problem", "dynamic", "free"]].copy()
	avg_df["time"] = (df1["time"] + df2["time"] + df3["time"]) / 3

	return avg_df

data = load_csv()

def plot_best_time(data: pd.DataFrame) -> None:
	"""
	Plot the best time for each problem.
	"""
	times = {
		"dynamic": {
			"1": [],
			"2": [],
			"3": []
		},
		"static": {
			"1": [],
			"2": [],
			"3": []
		},
		"dynamic_lf": {
			"1": [],
			"2": [],
			"3": []
		},
	}

	sequential = {
		"1": [data[(data["threads"] == 0) & (data["problem"] == 1)]["time"].min()],
		"2": [data[(data["threads"] == 0) & (data["problem"] == 2)]["time"].min()],
		"3": [data[(data["threads"] == 0) & (data["problem"] == 3)]["time"].min()]
	}

	for problem in [1, 2, 3]:
		for n_threads in [1, 2, 4, 8, 16, 32]:
			times["dynamic"][str(problem)].append(
				sequential[str(problem)][0] /
				data[
					(data["problem"] == problem) &
					(data["threads"] == n_threads) & 
					(data["dynamic"] == 1) & 
					(data["free"] == 0)
				]["time"].min()
			)
			times["static"][str(problem)].append(
				sequential[str(problem)][0] /
				data[
					(data["problem"] == problem) & 
					(data["threads"] == n_threads) & 
					(data["dynamic"] == 0) & 
					(data["free"] == 0)
				]["time"].min()
			)
			times["dynamic_lf"][str(problem)].append(
				sequential[str(problem)][0] /
				data[
					(data["problem"] == problem) & 
					(data["threads"] == n_threads) & 
					(data["dynamic"] == 1) & 
					(data["free"] == 1)
				]["time"].min()
			)

	fig, ax = plt.subplots(1, 3, figsize=(15, 5))
	fig.suptitle("Speedup for Each Problem")
	fig.tight_layout(pad=3.0)
	colors = ["blue", "orange", "green"]
	problem_names = ["Problem 1", "Problem 2", "Problem 3"]
	for i, problem in enumerate(["1", "2", "3"]):
		ax[i].set_title(problem_names[i])
		ax[i].set_xlabel("Number of Threads")
		#ax[i].set_ylabel("Time (s)")
		ax[i].set_ylabel("Speedup")
		#ax[i].set_yscale("log")  # Set y-axis to log scale
		ax[i].set_xticks(range(6))
		ax[i].set_xticklabels([1, 2, 4, 8, 16, 32])
		ax[i].plot(times["dynamic"][problem], label="Dynamic", color=colors[0], marker='x')
		ax[i].plot(times["static"][problem], label="Static", color=colors[1], marker='x')
		ax[i].plot(times["dynamic_lf"][problem], label="Dynamic Lock Free", color=colors[2], marker='x')
		#ax[i].axhline(y=sequential[problem][0], color='red', linestyle='--', label="Sequential")
		ax[i].legend()
		ax[i].grid()
	fig.savefig("speedup.png", dpi=300)
	plt.show()
	
		
def plot_chunk_size(data: pd.DataFrame, dynamic:bool, lock_free:bool) -> None:
	"""
	Plot the chunk size for each problem.
	"""
	chunk_sizes = [1, 10, 50, 100, 1000]
	problems = ["1", "2", "3"]
	
	fig, ax = plt.subplots(1, 3, figsize=(15, 5))
	fig.suptitle("Chunk Size for Each Problem")
	fig.tight_layout(pad=3.0)
	for i, problem in enumerate(problems):
		for chunk_size in chunk_sizes:
			data_filtered = data[
				(data["problem"] == int(problem)) & 
				(data["chunk_size"] == chunk_size) &
				(data["threads"].isin([1, 2, 4, 8, 16, 32])) &
				(data["dynamic"] == int(dynamic)) &
				(data["free"] == int(lock_free))
			]
			if data_filtered.empty:
				continue
			data_filtered = data_filtered.groupby("threads").agg({"time": "mean"}).reset_index()
			ax[i].plot(data_filtered["threads"], data_filtered["time"], label=f"Chunk Size {chunk_size}", marker='x')
		ax[i].set_title(f"Problem {problem}")
		ax[i].set_xlabel("Number of Threads")
		ax[i].set_ylabel("Time (s)")
		ax[i].set_xticks([1, 2, 4, 8, 16, 32])
		ax[i].set_xticklabels([1, 2, 4, 8, 16, 32])
		ax[i].set_yscale("log")
		ax[i].legend()
		ax[i].grid()
	fig.savefig(f"cs_{'dynamic' if dynamic else 'static'}_{'lock_free' if lock_free else ''}_log.png", dpi=300)
	plt.show()

#plot_best_time(data)
plot_chunk_size(data, dynamic=True, lock_free=True)