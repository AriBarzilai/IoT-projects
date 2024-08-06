import re
import pandas as pd
import os

def parse_log_file(file_path):
    data = []
    with open(file_path, 'r') as file:
        content = file.read()
        print(f"File content preview: {content[:200]}")  # Debug: Show first 200 characters
        lines = content.split('\n')
        for line in lines:
            if line.startswith('    uint64_t tRawData[]={'):
                raw_data = re.findall(r'0x[0-9A-Fa-f]+', line)
                raw_data_0 = raw_data[0]
                raw_data_1 = raw_data[1] if len(raw_data) > 1 else None
            elif line.startswith('    IrSender.sendPulseDistanceWidthFromArray'):
                params = re.findall(r'\d+', line)
                if len(params) >= 6:
                    header_mark = int(params[1])
                    header_space = int(params[2])
                    bit_mark = int(params[3])
                    one_space = int(params[4])
                    zero_space = int(params[5])
                    data.append([
                        raw_data_0,
                        raw_data_1,
                        header_mark,
                        header_space,
                        bit_mark,
                        one_space,
                        zero_space
                    ])
                else:
                    print(f"Warning: Insufficient parameters in line: {line}")
    print(f"Parsed {len(data)} rows from {file_path}")
    return data

def process_log_folder(input_folder):
    all_data = []
    print(f"Contents of {input_folder}: {os.listdir(input_folder)}")  # Debug: List folder contents
    
    for file in os.listdir(input_folder):
        if file.endswith('.log'):
            file_path = os.path.join(input_folder, file)
            print(f"Processing file: {file_path}")
            
            # Extract information from file name
            file_name = os.path.splitext(file)[0]
            match = re.match(r'Power (ON|OFF) Mode (\w+) Fan (\w+)', file_name)
            
            if match:
                power, mode, fan = match.groups()
                power = power.upper()
                mode = mode.upper()
                fan = f'FAN-{fan.upper()}'
                if fan == 'FAN-AUTO':
                    fan = 'FAN-A'
            else:
                print(f"Warning: Couldn't parse filename: {file_name}")
                power = "UNKNOWN"
                mode = "UNKNOWN"
                fan = "UNKNOWN"
            print(f"Extracted: Power={power}, Mode={mode}, Fan={fan}")
            
            parsed_data = parse_log_file(file_path)
            
            for row in parsed_data:
                all_data.append([power, mode, fan] + row)
    
    print(f"Total rows of data collected: {len(all_data)}")
    return all_data

def create_excel_file(data, output_file):
    print(f"Creating DataFrame with {len(data)} rows")
    df = pd.DataFrame(data, columns=[
        'POWER',
        'MODE',
        'FAN_MODE',
        'Raw Data 0',
        'Raw Data 1',
        'Header Mark',
        'Header Space',
        'Bit Mark',
        'One Space',
        'Zero Space'
    ])
    
    print(f"DataFrame shape: {df.shape}")
    print(df.head())
    
    # Sort the dataframe
    df = df.sort_values(['POWER', 'FAN_MODE'])
    
    # Ensure the output directory exists
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    
    df.to_excel(output_file, index=False, engine='openpyxl')
    print(f"Excel file '{output_file}' has been created successfully with {df.shape[0]} rows.")

# Usage
base_folder = r"C:\Users\avrah\OneDrive\Desktop\IoT-projects\smartHome\ac_command_data"
output_file = r"C:\Users\avrah\OneDrive\Desktop\IoT-projects\smartHome\ac_command_data.xlsx"

all_data = []

print(f"Contents of base folder: {os.listdir(base_folder)}")  # Debug: List base folder contents

for power_folder in ['ColdPowerON', 'ColdPowerOFF']:
    folder_path = os.path.join(base_folder, power_folder)
    if os.path.exists(folder_path):
        print(f"Processing power folder: {folder_path}")
        all_data.extend(process_log_folder(folder_path))
    else:
        print(f"Warning: Folder not found: {folder_path}")

if all_data:
    create_excel_file(all_data, output_file)
else:
    print("No data found to process.")