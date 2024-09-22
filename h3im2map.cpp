#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib> 
#include <iomanip>
#include <unordered_set>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include <map>
#include <tuple>
#include <random>

std::vector<unsigned char> readFileIntoArray(const std::string& filename) {
    // Open the file in binary mode and move the cursor to the end to determine the size
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open the file.");
    }

    // Read the entire file into a vector
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return buffer;
};



void writeArrayToFile(const std::string& filename, const std::vector<unsigned char>& buffer) {
    // Open the file in binary mode for writing
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open the file for writing.");
    }

    // Write the entire buffer to the file
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    file.close();
};

void modifyArrayAtOffset(std::vector<unsigned char>& buffer, const std::vector<unsigned char>& data, std::size_t offset) {
    // Ensure that the modification does not go beyond the buffer's size
    if (offset + data.size() > buffer.size()) {
        throw std::out_of_range("Error: Modification goes beyond the buffer's size.");
    }

    // Replace the elements in the buffer with the new data starting at the specified offset
    std::copy(data.begin(), data.end(), buffer.begin() + offset);
};

// Function that generates a random integer each time it is called
int generateRandomInt(int min = 0, int max = 20) {
    // Static objects to keep the generator and distribution alive across function calls
    static std::random_device rd;  // Seed generator
    static std::mt19937 gen(rd()); // Mersenne Twister generator
    std::uniform_int_distribution<> distrib(min, max); // Uniform distribution in range [min, max]

    return distrib(gen);
};
using RGB = std::tuple<unsigned char, unsigned char, unsigned char>;
std::map<RGB, unsigned char> rgbToByte = {
    {RGB(0x50, 0x3F, 0x0F), 0x00}, //Dirt        
    {RGB(0xDF, 0xCF, 0x8F), 0x01}, //Sand        
    {RGB(0x00, 0x40, 0x00), 0x02}, //Grass       
    {RGB(0xB0, 0xC0, 0xC0), 0x03}, //Snow        
    {RGB(0x4F, 0x80, 0x6F), 0x04}, //Swamp       
    {RGB(0x80, 0x70, 0x30), 0x05}, //Rough       
    {RGB(0x00, 0x80, 0x30), 0x06}, //Subterranean
    {RGB(0x4F, 0x4F, 0x4F), 0x07}, //Lava        
    {RGB(0x0F, 0x50, 0x90), 0x08}, //Water       
    {RGB(0x00, 0x00, 0x00), 0x09}, //Rock        
    {RGB(0xBD, 0x5A, 0x08), 0x0B}, //wasteland
    {RGB(0x29, 0x73, 0x18), 0x0A}  //highland
};

unsigned char getByteFromRGB(const RGB& color) {
    if (rgbToByte.find(color) != rgbToByte.end()) {
        return rgbToByte[color];
    } else {
        throw std::invalid_argument("RGB color not mapped to any byte value.");
    }
};


double colorDistance(const RGB& color1, const RGB& color2) {
    return std::sqrt(std::pow(std::get<0>(color1) - std::get<0>(color2), 2) +
                     std::pow(std::get<1>(color1) - std::get<1>(color2), 2) +
                     std::pow(std::get<2>(color1) - std::get<2>(color2), 2));
};

RGB findClosestColor(const RGB& color, const std::vector<RGB>& palette) {
    double minDistance = colorDistance(color, palette[0]);
    RGB closestColor = palette[0];

    for (const auto& paletteColor : palette) {
        double distance = colorDistance(color, paletteColor);
        if (distance < minDistance) {
            minDistance = distance;
            closestColor = paletteColor;
        }
    }
    return closestColor;
};

std::vector<std::vector<std::vector<unsigned char>>> ReadImage2D( const char *imageFile, int width, int height, int channels) {
    unsigned char *data = stbi_load(imageFile, &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cerr << "Failed to load image" << std::endl;
        return std::vector<std::vector<std::vector<unsigned char>>>();
    }
    std::vector<std::vector<std::vector<unsigned char>>> image2D(height, std::vector<std::vector<unsigned char>>(width, std::vector<unsigned char>(channels)));

    // Copy the data to the 2D array
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < channels; ++k) {
                
                image2D[i][j][k] = data[(i * width + j) * channels + k];
            }
        }
    }

    // Free the image memory
    stbi_image_free(data);
    return image2D;
};

void printColorAsHex(const RGB& color) {
        std::cout << "#"
        << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(std::get<0>(color))
        << std::setw(2) << std::setfill('0') << static_cast<int>(std::get<1>(color))
        << std::setw(2) << std::setfill('0') << static_cast<int>(std::get<2>(color))
        << std::dec << std::endl; // Reset to decimal mode after printing
};
int main(int argc, char *argv[]) {
    
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <file_id>" << " <source_image>" <<" <output_file>"<< std::endl;
        return 1;
    }
    std::string filePath = "./map_templates/hota_";
    std::unordered_set<std::string> stringSet = {"S", "M", "L", "XL", "H", "XH", "G"};
    std::string file_id = argv[1];
    std::string imageFile = argv[2];
    std::string outputFile = argv[3];

    std::vector<RGB> palette;
    for (const auto& pair : rgbToByte) {
        palette.push_back(pair.first);
    }

    // Check if file_id is not in stringSet
    if (stringSet.find(file_id) == stringSet.end()) {
        std::cout << file_id << " is not in the list of possible file sizes: S, M, L, XL, H, XH, G"<< std::endl;
    } else {
        filePath += file_id;
        std::cout << "File is: " << filePath << std::endl;
    }
   
    

    

    // Define the offset and number of bytes to read
    
    std::streampos offset = 0x68D;  // Start reading at address 0x68D

    //get data from template file
    std::vector<unsigned char> buffer = readFileIntoArray(filePath);


    //generate sizes 
    int height, width, channels = 3;
    if (file_id == "S") {
        height = 36;
        width = 36;
    }
    if (file_id == "M") {
        height = 72;
        width = 72;
    }

    if (file_id == "L") {
        height = 108;
        width = 108;
    }

    if (file_id == "XL") {
        height = 144;
        width = 144;
    }

    if (file_id == "H") {
        height = 180;
        width = 180;
    }

    if (file_id == "XH") {
        height = 216;
        width = 216;
    }

    if (file_id == "G") {
        height = 252;
        width = 252;
    }

    std::vector<unsigned char> byte_buffer(height * width * 7);
    std::vector<std::vector<unsigned char>> byte_data(height, std::vector<unsigned char>(width));
    std::vector<std::vector<std::vector<unsigned char>>> image_data = ReadImage2D(imageFile.c_str(), width, height, channels);
    //convert color to bytes
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            /*std::cout << i << j << std::endl;
            std::cout << "color from image: ("
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(image_data[i][j][0]) << ", "
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(image_data[i][j][1]) << ", "
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(image_data[i][j][2]) << ")"
                  << std::dec << std::endl;
           */
            //std::cout << "byte from image" << getByteFromRGB(RGB(image_data[i][j][0], image_data[i][j][1], image_data[i][j][2]));
            try {
                RGB pixel = RGB(RGB(static_cast<int>(image_data[i][j][0]), static_cast<int>(image_data[i][j][1]), static_cast<int>(image_data[i][j][2])));
                RGB closestColor = findClosestColor(pixel, palette);
                unsigned char byteValue = getByteFromRGB(closestColor);
                /*std::cout << "byte from image: 0x"
                      << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byteValue)
                      << std::dec << std::endl;*/
                byte_data[i][j] = byteValue;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            
        }
    }
    int tile_style_count = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {  
            tile_style_count += 1;
            
            byte_buffer[i * (width*7) + j*7 + 0] = byte_data[i][j]; //tile type
            if (byte_data[i][j] != 0x00) byte_buffer[i * (width*7) + j*7 + 1] = generateRandomInt(0,20);
            if (byte_data[i][j] == 0x00) byte_buffer[i * (width*7) + j*7 + 1] = generateRandomInt(0,40);
            //std::cout << i << " " << j<<" "<<i * (width-1) + j*7 + 0 <<" "<<byte_data[i][j]<<std::endl; 
            //byte_buffer[i * (width*7) + j*7 + 1] = tile_style_count;//generateRandomInt(); //tile stile
            byte_buffer[i * (width*7) + j*7 + 2] = 0;
            byte_buffer[i * (width*7) + j*7 + 3] = 0;
            byte_buffer[i * (width*7) + j*7 + 4] = 0;
            byte_buffer[i * (width*7) + j*7 + 5] = 0;
            byte_buffer[i * (width*7) + j*7 + 6] = 0x00;
            if ((j > 0) && (j < width-1) && (i > 0) && (i < height-1))  {
                if (
                    (byte_data[i][j - 1] == byte_data[i][j]) && 
                    (byte_data[i ][j] != byte_data[i][j + 1]) &&
                    (byte_data[i][j] != 0x09)
                ) {
                    byte_buffer[i * width * 7 + 7*j + 6] += 0x01;                        
                }
                if (
                    (byte_data[i][j - 1] ==  byte_data[i][j]) &&
                    (byte_data[i][j + 1] ==  byte_data[i][j]) &&
                    (byte_data[i - 1][j] ==  byte_data[i][j]) &&
                    (byte_data[i + 1][j] ==  byte_data[i][j]) &&
                    (byte_data[i + 1][j - 1] != byte_data[i][j])&&
                    (byte_data[i][j] != 0x09)                 
                ){
                    byte_buffer[i * width * 7 + 7*j + 6] = 0x01;
                }
                if (
                    (byte_data[i - 1][j] == byte_data[i][j]) && 
                    (byte_data[i + 1][j] != byte_data[i][j])&&
                    (byte_data[i][j] != 0x09)
                ) {
                    byte_buffer[i * width * 7 + 7*j + 6] += 0x02;
                }
                if (
                    (byte_data[i][j + 1] == 8) || 
                    (byte_data[i][j - 1] == 8) ||
                    (byte_data[i + 1][j] == 8) ||
                    (byte_data[i - 1][j] == 8) &&
                    (byte_data[i][j] != 0x09)
                ) {
                    byte_buffer[i * width * 7 + 7*j + 6] += 0x40;
                }
               
            } 
            /*std::cout << "bytes full: ("
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte_buffer[i* width + j + 0]) << ", "
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte_buffer[i* width + j + 1]) << ", "
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte_buffer[i* width + j + 6]) << ")"
                  << std::dec << std::endl;
              */  
        } 
            
       
    }
    std::cout<<std::endl;
   /* for(int i = 0; i < height * width * 7; ++i) {
        std::cout<< std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte_buffer[i])<<" ";
        
        if ((i+1) % 7 == 0) {std::cout<<std::endl;}
    }*/
    modifyArrayAtOffset(buffer, byte_buffer, offset);
    writeArrayToFile(outputFile, buffer);
    std::string command = "7z a -tgzip " + outputFile + ".h3m " + outputFile;
    int result = std::system(command.c_str());

    // Check if the command was successful
    if (result == 0) {
        std::cout << "File compressed successfully into " << outputFile <<".h3m " << std::endl;
    } else {
        std::cerr << "Failed to compress the file." << std::endl;
    }

    return 0;
}
