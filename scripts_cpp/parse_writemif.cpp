#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum colorspace_e {
	RGB,
	YUV422,
	YUV444,
};

int clip(int value, int min, int max) {
    return std::max(min, std::min(value, max));
}

void reverse_data(const std::string& input_bin_file, const std::string& output_bin_file, int width, int height, float bytes_per_pixel) {
    std::ifstream infile(input_bin_file, std::ios::binary);
    std::ofstream outfile(output_bin_file, std::ios::binary);

	if (!infile.is_open()) {
		throw std::runtime_error("Error: The file '" + input_bin_file + "' was not found.");
    }
    if (!outfile.is_open()) {
        throw std::runtime_error("Error: Could not open the output file '" + output_bin_file + "'");
    }
 
    const int bytes_per_block = 8;
    const int total_blocks = (width * height * bytes_per_pixel) / 8;
    for (size_t i = 0; i < total_blocks; ++i) {
        std::vector<char> yuv_bytes(bytes_per_block);
        if (!infile.read(yuv_bytes.data(), bytes_per_block)) {
            if (infile.eof()) {
                infile.clear();
                throw std::ios_base::failure("Error: Not enough data in '" + input_bin_file + "' to fill the image.");
            } else {
                throw std::ios_base::failure("Error: An I/O error occurred while reading the file.");
            }
        }
 
        std::vector<char> reversed_bytes(bytes_per_block);
        for (int j = 0; j < bytes_per_block; ++j) {
            reversed_bytes[j] = yuv_bytes[bytes_per_block - 1 - j];
        }
 
        outfile.write(reversed_bytes.data(), bytes_per_block);
    }

 	infile.close();
	outfile.close();
}

void read_rgb_10_data_and_create_bin(const std::string& input_bin_file, const std::string& output_bin_file, uint32_t width, uint32_t height) {
    std::ifstream infile(input_bin_file, std::ios::binary);
    std::ofstream outfile(output_bin_file, std::ios::binary);

    if (!infile.is_open()) {
        throw std::runtime_error("Error: The file '" + input_bin_file + "' was not found.");
    }

    if (!outfile.is_open()) {
        throw std::runtime_error("Error: Could not open the output file '" + output_bin_file + "'");
    }

    uint8_t buffer[4];
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
			infile.read(reinterpret_cast<char*>(buffer), 4);
            uint16_t byte1 = buffer[0];
            uint16_t byte2 = buffer[1];
            uint16_t byte3 = buffer[2];
            uint16_t byte4 = buffer[3];

            uint16_t r = ((byte1 << 4) | (byte2 >> 4)) >> 2;
            uint16_t g = (((byte2 & 0x0F) << 6) | (byte3 >> 2)) >> 2;
            uint16_t b = (((byte3 & 0x03) << 8) | byte4) >> 2;

            outfile.put(static_cast<char>(r & 0xFF));
            outfile.put(static_cast<char>(g & 0xFF));
            outfile.put(static_cast<char>(b & 0xFF));
        }
    }

    infile.close();
    outfile.close();
}

void readRgbDataAndCreateImage(const std::string& inputBinFile, const std::string& outputImageFile, int width, int height) {
    std::ifstream binFile(inputBinFile, std::ios::binary);
    if (!binFile.is_open()) {
        std::cerr << "Error: The file '" << inputBinFile << "' was not found." << std::endl;
        return;
    }

    cv::Mat image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

    std::vector<uint8_t> rgbBuffer(3);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!binFile.read(reinterpret_cast<char*>(rgbBuffer.data()), 3)) {
                std::cerr << "Error: Not enough data in '" << inputBinFile << "' to fill the image." << std::endl;
                return;
            }
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(rgbBuffer[2], rgbBuffer[1], rgbBuffer[0]);
        }
    }

    if (!cv::imwrite(outputImageFile, image)) {
        std::cerr << "Error: Could not save the image to '" << outputImageFile << "'." << std::endl;
    }

    binFile.close();
}

void read_yuv422_8_data_and_convert_yuv444_bin(const std::string& input_bin_file, const std::string& output_bin_file, int width, int height) {
    std::ifstream infile(input_bin_file, std::ios::binary);
    if (!infile) {
        throw std::runtime_error("Error: The file '" + input_bin_file + "' was not found.");
    }
 
    std::ofstream outfile(output_bin_file, std::ios::binary);
    if (!outfile) {
        throw std::runtime_error("Error: Could not open output file '" + output_bin_file + "'");
    }
 
    std::vector<uint8_t> buffer(4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; x += 2) {
            if (!infile.read(reinterpret_cast<char*>(buffer.data()), 4)) {
				;
            }
            uint8_t y1_422 = buffer[0];
            uint8_t u = buffer[1];
            uint8_t y2_422 = buffer[2];
            uint8_t v = buffer[3];
 
            outfile.write(reinterpret_cast<const char*>(&y1_422), 1);
            outfile.write(reinterpret_cast<const char*>(&u), 1);
            outfile.write(reinterpret_cast<const char*>(&v), 1);
            outfile.write(reinterpret_cast<const char*>(&y2_422), 1);
            outfile.write(reinterpret_cast<const char*>(&u), 1);       
			outfile.write(reinterpret_cast<const char*>(&v), 1);       
 
		}
    }
 
    infile.close();
    outfile.close();
}

void read_yuv422_10_data_and_create_yuv444_bin(const std::string& input_bin_file, const std::string& output_bin_file, int width, int height) {
    std::ifstream infile(input_bin_file, std::ios::binary);
    std::ofstream outfile(output_bin_file, std::ios::binary);
 
    if (!infile.is_open()) {
        throw std::runtime_error("Error: The file '" + input_bin_file + "' was not found.");
    }
    if (!outfile.is_open()) {
        throw std::runtime_error("Error: Could not open the output file '" + output_bin_file + "'.");
    }
 
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; x += 2) {
            char yuv_bytes[5];
            infile.read(yuv_bytes, 5);

            uint8_t byte1 = yuv_bytes[0];
            uint8_t byte2 = yuv_bytes[1];
            uint8_t byte3 = yuv_bytes[2];
            uint8_t byte4 = yuv_bytes[3];
            uint8_t byte5 = yuv_bytes[4];
 
            uint16_t y1 = byte1;
            uint16_t u = (((byte2 & 0x3f) << 4) | (byte3 >> 4)) >> 2;
            uint16_t y2 = (((byte3 & 0x0f) << 6) | (byte4 >> 2)) >> 2;
            uint16_t v = (((byte4 & 0x03) << 8) | byte5) >> 2;
 
            outfile.write(reinterpret_cast<const char*>(&y1), 1);
            outfile.write(reinterpret_cast<const char*>(&u), 1); 
            outfile.write(reinterpret_cast<const char*>(&v), 1);
 
            outfile.write(reinterpret_cast<const char*>(&y2), 1);
            outfile.write(reinterpret_cast<const char*>(&u), 1);
            outfile.write(reinterpret_cast<const char*>(&v), 1);
        }
    }

 
    infile.close();
    outfile.close();
}

void read_yuv444_10_data_and_create_yuv444_bin(const std::string& input_bin_file, const std::string& output_bin_file, uint32_t width, uint32_t height) {
    std::ifstream infile(input_bin_file, std::ios::binary);
    std::ofstream outfile(output_bin_file, std::ios::binary);

    if (!infile.is_open()) {
        throw std::runtime_error("Error: The file '" + input_bin_file + "' was not found.");
    }

    if (!outfile.is_open()) {
        throw std::runtime_error("Error: Could not open the output file '" + output_bin_file + "'");
    }

    uint8_t buffer[4];

    for (uint32_t y_pos = 0; y_pos < height; ++y_pos) {
        for (uint32_t x_pos = 0; x_pos < width; ++x_pos) {
            if (!infile.read(reinterpret_cast<char*>(buffer), 4)) {
				;
            }

            uint16_t byte1 = buffer[0];
            uint16_t byte2 = buffer[1];
            uint16_t byte3 = buffer[2];
            uint16_t byte4 = buffer[3];

            uint8_t y = static_cast<uint8_t>((((byte1 << 4) | (byte2 >> 4)) >> 2) & 0xFF);
            uint8_t u = static_cast<uint8_t>(((((byte2 & 0x0F) << 6) | (byte3 >> 2)) >> 2) & 0xFF);
            uint8_t v = static_cast<uint8_t>(((((byte3 & 0x03) << 8) | byte4) >> 2) & 0xFF);

            outfile.put(y);
            outfile.put(u);
            outfile.put(v);
        }
    }

    infile.close();
    outfile.close();
}

void yuv444_to_rgb_and_save(const std::string& input_bin_file, const std::string& output_image_file, int width, int height) {
    std::ifstream yuv_file(input_bin_file, std::ios::binary);
    if (!yuv_file.is_open()) {
        throw std::runtime_error("Error opening YUV file: " + input_bin_file);
    }

    std::vector<uint8_t> yuv_data((width * height * 3), 0);
    if (!yuv_file.read(reinterpret_cast<char*>(yuv_data.data()), yuv_data.size())) {
        throw std::runtime_error("Error reading YUV data");
    }
    yuv_file.close();

    cv::Mat rgb_image(height, width, CV_8UC3);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width * 3 + x * 3;

            uint8_t Y = yuv_data[index + 0];
            uint8_t U = yuv_data[index + 1];
            uint8_t V = yuv_data[index + 2];

            int R = clip(1.164 * (Y - 16) + 1.793 * (V - 128), 0, 255);
            int G = clip(1.164 * (Y - 16) - 0.534 * (V - 128) - 0.213 * (U - 128), 0, 255);
            int B = clip(1.164 * (Y - 16) + 2.115 * (U - 128), 0, 255);

            rgb_image.at<cv::Vec3b>(y, x) = cv::Vec3b(B, G, R);
        }
    }

    // 保存图像
    if (!cv::imwrite(output_image_file, rgb_image)) {
        throw std::runtime_error("Error saving RGB image: " + output_image_file);
    }
}

int main(int argc, char* argv[]) {
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	int colorspace = atoi(argv[4]);
	int colordepth = atoi(argv[5]);
	int reverse = atoi(argv[6]);
	float bytes_per_pixel;

    std::string input_file = argv[1]; // 替换为你的输入二进制文件名
	std::string output_file = "vdin_dump_reverse.bin";
    std::string outputImageFile = "output.bmp"; // 替换为你想要保存的图像文件名

	if (colorspace == RGB) {
		if (colordepth == 0) {
			bytes_per_pixel = 3;
			if (reverse)
				reverse_data(input_file, output_file, width, height, bytes_per_pixel);
			readRgbDataAndCreateImage(reverse == 0 ? input_file : output_file, outputImageFile, width, height);
		} else if (colordepth == 1) {
			bytes_per_pixel = 4;
			if (reverse)
				reverse_data(input_file, output_file, width, height, bytes_per_pixel);
			read_rgb_10_data_and_create_bin(reverse == 0 ? input_file : output_file, "output.bin", width, height);
			readRgbDataAndCreateImage("output.bin", outputImageFile, width, height);
		}
	} else if (colorspace == YUV444) {
		if (colordepth == 0) {
			bytes_per_pixel = 3;
			if (reverse)
				reverse_data(input_file, output_file, width, height, bytes_per_pixel);
			yuv444_to_rgb_and_save(reverse == 0 ? input_file : output_file, outputImageFile, width, height);
		} else if (colordepth == 1) {
			bytes_per_pixel = 4;
			if (reverse)
				reverse_data(input_file, output_file, width, height, bytes_per_pixel);
			read_yuv444_10_data_and_create_yuv444_bin(reverse == 0 ? input_file : output_file, "output.bin", width, height);
			yuv444_to_rgb_and_save("output.bin", outputImageFile, width, height);
		}
	} else if (colorspace == YUV422) {
		if (colordepth == 0) {
			bytes_per_pixel = 2;
			if (reverse)
				reverse_data(input_file, output_file, width, height, bytes_per_pixel);
			read_yuv422_8_data_and_convert_yuv444_bin(reverse == 0 ? input_file : output_file, "output.bin", width, height);
			yuv444_to_rgb_and_save("output.bin", outputImageFile, width, height);
		} else if (colordepth == 1) {
			bytes_per_pixel = 2.5;
			if (reverse)
				reverse_data(input_file, output_file, width, height, bytes_per_pixel);
			read_yuv422_10_data_and_create_yuv444_bin(reverse == 0 ? input_file : output_file, "output.bin", width, height);
			yuv444_to_rgb_and_save("output.bin", outputImageFile, width, height);
		}
	}
    return 0;
}
