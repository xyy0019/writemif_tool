from PIL import Image  
import struct  
import numpy as np
import sys

def reverse_data(input_bin_file, output_bin_file, width, height):  
    flag = 1;
    try:  
        with open(input_bin_file, 'rb') as infile, open(output_bin_file, 'wb') as outfile: 
            for i in range(int(height * width * 2.5 / 8)):   
                yuv_bytes = infile.read(8)  
                if len(yuv_bytes) < 8:  
                    raise EOFError("Unexpected end of file while reading RGB data.")    
                byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8 = struct.unpack('BBBBBBBB', yuv_bytes)
                binary1 = struct.pack('B', byte1)  
                binary2 = struct.pack('B', byte2)
                binary3 = struct.pack('B', byte3)  
                binary4 = struct.pack('B', byte4)
                binary5 = struct.pack('B', byte5)  
                binary6 = struct.pack('B', byte6)
                binary7 = struct.pack('B', byte7)  
                binary8 = struct.pack('B', byte8)
                outfile.write(binary8)  
                outfile.write(binary7)
                outfile.write(binary6)  
                outfile.write(binary5)
                outfile.write(binary4)  
                outfile.write(binary3)
                outfile.write(binary2)  
                outfile.write(binary1)
    except FileNotFoundError:  
        print(f"Error: The file '{input_bin_file}' was not found.")  
    except EOFError:  
        print(f"Error: Not enough data in '{input_bin_file}' to fill the image.")  
    except Exception as e:  
        print(f"An error occurred: {e}")  

def main():
    input_bin_file = 'input.bin'  
    output_bin_file = 'output.bin'  
    width = int(sys.argv[1])  
    height = int(sys.argv[2])
    reverse_data(input_bin_file, output_bin_file, width, height)
if __name__ == "__main__":
    main()
