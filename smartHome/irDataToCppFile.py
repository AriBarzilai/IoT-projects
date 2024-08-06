import pandas as pd
import os

# Assuming the Excel file is in the same directory as the script
file_path = r"C:\Users\avrah\OneDrive\Desktop\IoT-projects\smartHome\ac_command_data.xlsx"

# Check if the file exists
if not os.path.exists(file_path):
    print(f"Error: The file '{file_path}' does not exist.")
    exit(1)

try:
    df = pd.read_excel(file_path)
except PermissionError:
    print(f"Error: Unable to access '{file_path}'. The file might be open in another program.")
    print("Please close the file and try again.")
    exit(1)
except Exception as e:
    print(f"An unexpected error occurred while trying to read the file: {e}")
    exit(1)

# Define the C++ struct template
cpp_struct_template = """
AcCommand {array_name}[] = {{
    {structs}
}};
"""

# Define the C++ struct entry template
cpp_struct_entry_template = """{{{header_mark}, {header_space}, {bit_mark}, {one_space}, {zero_space}, {{0x{raw_data_0}, 0x{raw_data_1}}}}}"""

# Define a function to generate the C++ code for a given command set
def generate_cpp_code(df, power, mode, fan_mode, array_name):
    filtered_df = df[(df['POWER'] == power) & (df['MODE'] == mode) & (df['FAN_MODE'] == fan_mode)]
    structs = []

    # Ensure we have exactly 15 entries
    for i in range(15):
        if i < len(filtered_df):
            row = filtered_df.iloc[i]
            struct_entry = cpp_struct_entry_template.format(
                header_mark=int(row['Header Mark']),
                header_space=int(row['Header Space']),
                bit_mark=int(row['Bit Mark']),
                one_space=int(row['One Space']),
                zero_space=int(row['Zero Space']),
                raw_data_0=row['Raw Data 0'].split('x')[1],
                raw_data_1=row['Raw Data 1'].split('x')[1]
            )
        else:
            # If we don't have enough entries, pad with a default struct
            struct_entry = "{0, 0, 0, 0, 0, {0x0, 0x0}}"
        structs.append(struct_entry)
    
    return cpp_struct_template.format(array_name=array_name, structs=",\n    ".join(structs))

# Generate the C++ code for each command set
command_sets = [
    ("OFF", "COLD", "FAN-1", "OFF_COLD_FAN1"),
    ("OFF", "COLD", "FAN-2", "OFF_COLD_FAN2"),
    ("OFF", "COLD", "FAN-3", "OFF_COLD_FAN3"),
    ("OFF", "COLD", "FAN-A", "OFF_COLD_FANA"),
    ("ON", "COLD", "FAN-1", "ON_COLD_FAN1"),
    ("ON", "COLD", "FAN-2", "ON_COLD_FAN2"),
    ("ON", "COLD", "FAN-3", "ON_COLD_FAN3"),
    ("ON", "COLD", "FAN-A", "ON_COLD_FANA"),
]

cpp_code = ""

for power, mode, fan_mode, array_name in command_sets:
    cpp_code += generate_cpp_code(df, power, mode, fan_mode, array_name)
    cpp_code += "\n"

# Save the generated C++ code to a file
output_path = r"C:\Users\avrah\OneDrive\Desktop\IoT-projects\smartHome\ac_command_data.cpp"
try:
    with open(output_path, 'w') as f:
        f.write(cpp_code)
    print(f"C++ code has been generated and saved to '{output_path}'")
except PermissionError:
    print(f"Error: Unable to write to '{output_path}'. You might not have write permissions for this location.")
except Exception as e:
    print(f"An unexpected error occurred while trying to write the file: {e}")

# Print a summary of the generated commands
print("\nGenerated command sets:")
for power, mode, fan_mode, array_name in command_sets:
    print(f"- {array_name}")

print("\nPlease check the output file to ensure all command sets are present.")