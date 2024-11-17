#!/usr/bin/env python
# encoding: utf-8

from PIL import Image  
import struct  

def split_and_convert_to_binary(input_file, output_file):  
    try:  
        with open(input_file, 'r') as infile, open(output_file, 'wb') as outfile:  
            for line in infile:  
                line = line.strip()  
                if len(line) == 4:  
                    hex1 = line[:2]  
                    hex2 = line[2:]   
                    int1 = int(hex1, 16)  
                    int2 = int(hex2, 16)   
                    binary1 = struct.pack('B', int2)  
                    binary2 = struct.pack('B', int1)   
                    outfile.write(binary1)  
                    outfile.write(binary2)  
                else:  
                    print(f"Warning: Line '{line}' does not match the expected format (length 4). Skipping.")  
    except FileNotFoundError:  
        print(f"Error: The file '{input_file}' was not found.")  
    except ValueError:  
        print(f"Error: Invalid hexadecimal data in file '{input_file}'.")  
    except Exception as e:  
        print(f"An error occurred: {e}")  

input_file = 'ddrdata.txt'  
output_file = 'ddrdata.bin'  
split_and_convert_to_binary(input_file, output_file)
