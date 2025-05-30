import csv
import scipy.stats as stats

CSV_DIR = './DATA/CSV/'

# uncomment based on whether you want encryption stats or decryption stats.

def load_times(filename):
    path = f"{CSV_DIR}/{filename}"
    with open(path, 'r') as file:
        reader = csv.DictReader(file)
        return [float(row['Decryption Time']) for row in reader]
        # return [float(row['Encryption Time']) for row in reader]

def run_t_test(aes_file, ascon_file):
    aes_times = load_times(aes_file)
    ascon_times = load_times(ascon_file)

    # One-tailed t-test: H1 = ASCON is faster (mean lower) than AES
    t_stat, p_val_one_tailed = stats.ttest_ind(ascon_times,aes_times, equal_var=False, alternative='less')

    if p_val_one_tailed == 0:
        p_val_one_tailed = 2.2e-308

    print(f"\nT-Test: {ascon_file} < {aes_file}")
    print(f"  t-statistic: {t_stat:.5f}")
    print(f"  one-tailed p-value: {p_val_one_tailed:.2e}")

    if p_val_one_tailed < 0.05:
        print("Statistically significant: ASCON is faster (p < 0.05)")
    else:
        print("Not statistically significant (p ≥ 0.05)")

if __name__ == "__main__":
    # IoT
    run_t_test("AES-128-CTR.csv", "ASCON-128-0.csv")
    run_t_test("AES-128-CTR.csv", "ASCON-128a-0.csv")

    # Quantum
    run_t_test("AES-256-CTR.csv", "ASCON-80pq-0.csv")

    # AD
    run_t_test("AES-128-GCM-0.csv", "ASCON-128-0.csv")
    run_t_test("AES-128-GCM-0.csv", "ASCON-128a-0.csv")

    run_t_test("AES-128-GCM-16.csv", "ASCON-128-16.csv")
    run_t_test("AES-128-GCM-16.csv", "ASCON-128a-16.csv")

    run_t_test("AES-128-GCM-256.csv", "ASCON-128-256.csv")
    run_t_test("AES-128-GCM-256.csv", "ASCON-128a-256.csv")

    run_t_test("AES-256-GCM-0.csv", "ASCON-80pq-0.csv")
    run_t_test("AES-256-GCM-16.csv", "ASCON-80pq-16.csv")
    run_t_test("AES-256-GCM-256.csv", "ASCON-80pq-256.csv")

