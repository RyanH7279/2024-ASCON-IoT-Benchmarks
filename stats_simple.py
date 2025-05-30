import os
import csv
import statistics
import math

CSV_DIR = './DATA/CSV/'
SUMMARY_FILE = './DATA/summary.txt'

def compute_stats(data):
    stats = {}
    stats['Total'] = sum(data)
    stats['us/256B'] = round(sum(data)/len(data),0)
    stats['us/B'] = sum(data)/(256 * len(data))
    stats['B/s'] = (256 * len(data))/(sum(data)/1_000_000)
    stats['Min'] = min(data)
    stats['Max'] = max(data)
    stats['Mean'] = statistics.mean(data)
    stats['Variance'] = statistics.variance(data)
    stats['Std Dev'] = statistics.stdev(data)
    stats['Std Dev R'] = round(stats['Std Dev'],1)
    stats['Std Error'] = stats['Std Dev'] / math.sqrt(len(data))
    stats['95% CI'] = 1.96 * stats['Std Error']
    return stats

def print_stats():
    with open(SUMMARY_FILE, 'w') as summary:
        for filename in os.listdir(CSV_DIR):
            if filename.endswith('.csv'):
                base_name = filename[:-4]  # strip '.csv'
                csv_path = os.path.join(CSV_DIR, filename)

                encryption_times = []
                decryption_times = []

                with open(csv_path, 'r') as file:
                    reader = csv.DictReader(file)
                    for row in reader:
                        encryption_times.append(float(row['Encryption Time']))
                        decryption_times.append(float(row['Decryption Time']))

                # summary.write(f"\nStats for {base_name}:\n")
                # summary.write("Encryption Time:\n")
                # enc_stats = compute_stats(encryption_times)
                # for k, v in enc_stats.items():
                #     summary.write(f"  {k}: {v:.6f}\n")

                summary.write(f"\nStats for {base_name}:\n")
                summary.write("Encryption Time:\n")
                enc_stats = compute_stats(encryption_times)
                for k, v in enc_stats.items():
                    summary.write(f"  {k}: {v}\n")

                # summary.write("Decryption Time:\n")
                # dec_stats = compute_stats(decryption_times)
                # for k, v in dec_stats.items():
                #     summary.write(f"  {k}: {v:.6f}\n")
                
                summary.write("Decryption Time:\n")
                dec_stats = compute_stats(decryption_times)
                for k, v in dec_stats.items():
                    summary.write(f"  {k}: {v}\n")

    print(f"Stats written to {SUMMARY_FILE}")

if __name__ == "__main__":
    print_stats()
