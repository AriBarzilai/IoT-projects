import re
import pandas as pd
import os

def parse_log_file(file_path):
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            if line.startswith('    uint64_t tRawData[]={'):
                raw_data = re.findall(r'0x[0-9A-Fa-f]+', line)
                raw_data_0 = raw_data[0]
                raw_data_1 = raw_data[1] if len(raw_data) > 1 else None
            elif line.startswith('    IrSender.sendPulseDistanceWidthFromArray'):
                params = re.findall(r'\d+', line)
                frequency = int(params[0])
                header_mark = int(params[1])
                header_space = int(params[2])
                bit_mark = int(params[3])
                one_space = int(params[4])
                zero_space = int(params[5])
                data.append([
                    raw_data_0,
                    raw_data_1,
                    frequency,
                    header_mark,
                    header_space,
                    bit_mark,
                    one_space,
                    zero_space
                ])
    return data

def create_excel_file(data, output_file):
    df = pd.DataFrame(data, columns=[
        'Raw Data 0',
        'Raw Data 1',
        'Frequency',
        'Header Mark',
        'Header Space',
        'Bit Mark',
        'One Space',
        'Zero Space'
    ])
    df.to_excel(output_file, index=False, engine='openpyxl')
    print(f"Excel file '{output_file}' has been created successfully.")

def process_log_folder(input_folder, output_folder):
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    for filename in os.listdir(input_folder):
        if filename.endswith('.log'):
            input_file_path = os.path.join(input_folder, filename)
            output_file_name = os.path.splitext(filename)[0] + '.xlsx'
            output_file_path = os.path.join(output_folder, output_file_name)

            parsed_data = parse_log_file(input_file_path)
            create_excel_file(parsed_data, output_file_path)

# Usage
input_folder = r"C:\Users\avrah\OneDrive\Desktop\logs\ColdPowerOFF"
output_folder = r"C:\Users\avrah\OneDrive\Desktop\logs\ColdPowerOFF_Excel"

process_log_folder(input_folder, output_folder)
