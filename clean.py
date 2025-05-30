import os
import re

RAW_DIR = './DATA/RAW/'
CSV_DIR = './DATA/CSV/'

os.makedirs(CSV_DIR, exist_ok=True)

def clean_line(line):
    """Remove '#' and strip whitespace."""
    return line.replace('#', '').strip()

def parse_title(raw_title):
    """
    Convert a raw title like 'AES-128-GCM AD_SIZE:0' to 'AES-128-GCM-0'
    """
    match = re.match(r"([A-Za-z0-9\-]+)\s+AD_SIZE:(\d+)", raw_title)
    if match:
        algo, size = match.groups()
        return f"{algo}-{size}"
    else:
        # fallback: remove spaces and punctuation
        return re.sub(r'\s+', '-', raw_title.strip())

def process_files():
    for filename in os.listdir(RAW_DIR):
        if filename.endswith('-log.txt'):
            raw_path = os.path.join(RAW_DIR, filename)

            with open(raw_path, 'r') as file:
                lines = [clean_line(line) for line in file if clean_line(line)]

            i = 0
            while i < len(lines):
                if ',' not in lines[i] and len(lines[i]) > 0:
                    title = parse_title(lines[i])

                    if i + 1000 >= len(lines):
                        print(f"Skipping block {title}: not enough lines after title.")
                        break

                    csv_filename = f"{title}.csv"
                    csv_path = os.path.join(CSV_DIR, csv_filename)

                    with open(csv_path, 'w') as out_file:
                        out_file.write('Iteration,Encryption Time,Decryption Time\n')
                        for line in lines[i+1:i+1001]:
                            out_file.write(line + '\n')

                    print(f"Extracted: {csv_filename}")
                    i += 1001
                else:
                    i += 1

if __name__ == "__main__":
    process_files()
