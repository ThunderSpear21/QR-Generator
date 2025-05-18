#include <bits/stdc++.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;

class QrUtils
{
public:
    // To map AlphaNumeric characters to respective Integer encoding
    unordered_map<char, int> alphanum_table = {
        {'0', 0},
        {'1', 1},
        {'2', 2},
        {'3', 3},
        {'4', 4},
        {'5', 5},
        {'6', 6},
        {'7', 7},
        {'8', 8},
        {'9', 9},
        {'A', 10},
        {'B', 11},
        {'C', 12},
        {'D', 13},
        {'E', 14},
        {'F', 15},
        {'G', 16},
        {'H', 17},
        {'I', 18},
        {'J', 19},
        {'K', 20},
        {'L', 21},
        {'M', 22},
        {'N', 23},
        {'O', 24},
        {'P', 25},
        {'Q', 26},
        {'R', 27},
        {'S', 28},
        {'T', 29},
        {'U', 30},
        {'V', 31},
        {'W', 32},
        {'X', 33},
        {'Y', 34},
        {'Z', 35},
        {' ', 36},
        {'$', 37},
        {'%', 38},
        {'*', 39},
        {'+', 40},
        {'-', 41},
        {'.', 42},
        {'/', 43},
        {':', 44},
    };

    // To map error mode(L/M/Q/H) with respective Integer to be used in Format String
    unordered_map<char, int> error_mode = {
        {'L', 1}, {'M', 0}, {'Q', 3}, {'H', 2}};

    // 4 Capacity Maps for the Max. Length String that can be used for the given Version and Error Mode
    unordered_map<int, int> capacity_L = {
        {1, 25}, {2, 47}, {3, 77}, {4, 114}, {5, 154}, {6, 195}, {7, 224}, {8, 279}, {9, 335}, {10, 395}};

    unordered_map<int, int> capacity_M = {
        {1, 20}, {2, 38}, {3, 61}, {4, 90}, {5, 122}, {6, 154}, {7, 178}, {8, 221}, {9, 262}, {10, 311}};

    unordered_map<int, int> capacity_Q = {
        {1, 16}, {2, 29}, {3, 47}, {4, 67}, {5, 87}, {6, 108}, {7, 125}, {8, 157}, {9, 189}, {10, 221}};

    unordered_map<int, int> capacity_H = {
        {1, 10}, {2, 20}, {3, 35}, {4, 50}, {5, 64}, {6, 84}, {7, 93}, {8, 122}, {9, 143}, {10, 174}};

    // To map encoding mode to String
    unordered_map<int, string> mode_indicator = {
        {0, "0001"}, // numeric
        {1, "0010"}, // alphanumeric
        {2, "0100"}  // byte
    };

    // 4 Format String Maps for the Format String for a given Mask Number and Error Mode
    unordered_map<int, string> format_string_L = {
        {0, "111011111000100"},
        {1, "111001011110011"},
        {2, "111110110101010"},
        {3, "111100010011101"},
        {4, "110011000101111"},
        {5, "110001100011000"},
        {6, "110110001000001"},
        {7, "110100101110110"},
    };
    unordered_map<int, string> format_string_M = {
        {0, "101010000010010"},
        {1, "101000100100101"},
        {2, "101111001111100"},
        {3, "101101101001011"},
        {4, "100010111111001"},
        {5, "100000011001110"},
        {6, "100111110010111"},
        {7, "100101010100000"},
    };
    unordered_map<int, string> format_string_Q = {
        {0, "011010101011111"},
        {1, "011000001101000"},
        {2, "011111100110001"},
        {3, "011101000000110"},
        {4, "010010010110100"},
        {5, "010000110000011"},
        {6, "010111011011010"},
        {7, "010101111101101"},
    };
    unordered_map<int, string> format_string_H = {
        {0, "001011010001001"},
        {1, "001001110111110"},
        {2, "001110011100111"},
        {3, "001100111010000"},
        {4, "000011101100010"},
        {5, "000001001010101"},
        {6, "000110100001100"},
        {7, "000100000111011"},
    };

    // To map capacity of total Data Codewords for a given Version and Error Mode
    unordered_map<char, unordered_map<int, int>> data_codewords =
        {
            {'L', {{1, 19}, {2, 34}, {3, 55}, {4, 80}, {5, 108}}},
            {'M', {{1, 16}, {2, 28}, {3, 44}, {4, 64}, {5, 86}}},
            {'Q', {{1, 13}, {2, 22}, {3, 34}, {4, 48}, {5, 62}}},
            {'H', {{1, 9}, {2, 16}, {3, 26}, {4, 36}, {5, 46}}}
            // Extend as needed
    };

    // 4 Distribution Maps for the number of Data Codewords in each block of each group for a given Version and Error Mode
    vector<vector<int>> level_L_matrix = {
        {7, 1, 19, 0, 0},
        {10, 1, 34, 0, 0},
        {15, 1, 55, 0, 0},
        {20, 1, 80, 0, 0},
        {26, 1, 108, 0, 0},
    };
    vector<vector<int>> level_M_matrix = {
        {10, 1, 16, 0, 0},
        {16, 1, 28, 0, 0},
        {26, 1, 44, 0, 0},
        {18, 2, 32, 0, 0},
        {24, 2, 43, 0, 0},
    };

    vector<vector<int>> level_Q_matrix = {
        {13, 1, 13, 0, 0},
        {22, 1, 22, 0, 0},
        {18, 2, 17, 0, 0},
        {26, 2, 24, 0, 0},
        {18, 2, 15, 2, 16},
    };
    vector<vector<int>> level_H_matrix = {
        {17, 1, 9, 0, 0},
        {28, 1, 16, 0, 0},
        {22, 2, 13, 0, 0},
        {16, 4, 9, 0, 0},
        {22, 2, 11, 2, 12},
    };

    // To map the indices used to center the Alignment Pattern for a given Version
    unordered_map<int, vector<int>> alignment_pattern_positions = {
        {1, {}},
        {2, {6, 18}},
        {3, {6, 22}},
        {4, {6, 26}},
        {5, {6, 30}}};

    // Function to find the Min. Version needed to fit the input string
    int find_min_version(char mode, int n)
    {
        unordered_map<int, int> *capacity_map = nullptr;
        switch (mode)
        {
        case 'L':
            capacity_map = &capacity_L;
            break;
        case 'M':
            capacity_map = &capacity_M;
            break;
        case 'Q':
            capacity_map = &capacity_Q;
            break;
        case 'H':
            capacity_map = &capacity_H;
            break;
        }
        for (int i = 1; i <= 10; i++)
        {
            if ((*capacity_map)[i] >= n)
                return i;
        }
        return -1;
    }

    // Function to split the given Polynomial Coeffs into the required number of Blocks and Groups for the given Version
    vector<vector<int>> split_into_blocks(const vector<int> &polynomial_coeffs, const vector<vector<int>> &level_matrix, int version)
    {
        vector<vector<int>> blocks;
        // version is 1-based, adjust for 0-based index:
        int idx = version - 1;

        int ec_codewords = level_matrix[idx][0]; // error correction codewords per block (not used here, but useful later)
        int group1_blocks = level_matrix[idx][1];
        int group1_data_codewords = level_matrix[idx][2];
        int group2_blocks = level_matrix[idx][3];
        int group2_data_codewords = level_matrix[idx][4];

        int pos = 0;

        // Group 1 blocks
        for (int i = 0; i < group1_blocks; i++)
        {
            vector<int> block;
            for (int j = 0; j < group1_data_codewords; j++)
            {
                block.push_back(polynomial_coeffs[pos++]);
            }
            blocks.push_back(block);
        }

        // Group 2 blocks (if any)
        for (int i = 0; i < group2_blocks; i++)
        {
            vector<int> block;
            for (int j = 0; j < group2_data_codewords; j++)
            {
                block.push_back(polynomial_coeffs[pos++]);
            }
            blocks.push_back(block);
        }

        return blocks;
    }

    // Function to convert given number to k-bit binary string
    string convert_to_k_bit_binary(int i, int k)
    {
        string bin = bitset<32>(i).to_string();
        return bin.substr(32 - k);
    }

    // Function to convert given Binary Matrix to PNG
    void matrix_to_png(const vector<vector<int>> &matrix, const string &filename, int border = 3, int scale = 10)
    {
        int orig_height = matrix.size();
        int orig_width = matrix[0].size();

        int new_height = (orig_height + 2 * border) * scale;
        int new_width = (orig_width + 2 * border) * scale;

        std::vector<unsigned char> image_data(new_width * new_height, 255); // default white

        for (int i = 0; i < orig_height; ++i)
        {
            for (int j = 0; j < orig_width; ++j)
            {
                unsigned char color = matrix[i][j] ? 0 : 255; // black or white
                // Fill the scaled block
                int start_row = (i + border) * scale;
                int start_col = (j + border) * scale;
                for (int y = 0; y < scale; ++y)
                {
                    for (int x = 0; x < scale; ++x)
                    {
                        image_data[(start_row + y) * new_width + (start_col + x)] = color;
                    }
                }
            }
        }

        stbi_write_png(filename.c_str(), new_width, new_height, 1, image_data.data(), new_width);
    }

    // Function to Column-wise interleave Data codewords and EC codewords
    vector<int> interleave_blocks(const vector<vector<int>> &data_blocks, const vector<vector<int>> &rs_blocks)
    {
        vector<int> result;

        // Interleave data codewords
        size_t max_data_len = 0;
        for (const auto &block : data_blocks)
            max_data_len = max(max_data_len, block.size());

        for (size_t i = 0; i < max_data_len; ++i)
        {
            for (const auto &block : data_blocks)
            {
                if (i < block.size())
                    result.push_back(block[i]);
            }
        }

        // Interleave error correction codewords
        size_t max_ec_len = 0;
        for (const auto &block : rs_blocks)
            max_ec_len = max(max_ec_len, block.size());

        for (size_t i = 0; i < max_ec_len; ++i)
        {
            for (const auto &block : rs_blocks)
            {
                if (i < block.size())
                    result.push_back(block[i]);
            }
        }

        return result;
    }

    // Function to place Finder Pattern at 3 corners
    void place_finder_pattern(vector<vector<int>> &arr, int x, int y, set<pair<int, int>> &reserved)
    {
        int r = arr.size();
        int c = arr[0].size();
        // Outer 7x7 black border
        for (int i = 0; i < 7; i++)
        {
            for (int j = 0; j < 7; j++)
            {
                int row = x + i;
                int col = y + j;
                arr[row][col] = 1;
                reserved.insert({row, col});
            }
        }

        // Inner 5x5 white square (1..5, 1..5 inside the 7x7)
        for (int i = 1; i < 6; i++)
        {
            for (int j = 1; j < 6; j++)
            {
                int row = x + i;
                int col = y + j;
                arr[row][col] = 0;
                reserved.insert({row, col});
            }
        }

        // Inner 3x3 black square (2..4, 2..4 inside the 7x7)
        for (int i = 2; i < 5; i++)
        {
            for (int j = 2; j < 5; j++)
            {
                int row = x + i;
                int col = y + j;
                arr[row][col] = 1;
                reserved.insert({row, col});
            }
        }

        // Separator: 1-pixel white border around the finder (8x8 area with finder in center)
        for (int i = -1; i <= 7; i++)
        {
            for (int j = -1; j <= 7; j++)
            {
                int row = x + i;
                int col = y + j;
                if (row >= 0 && row < r && col >= 0 && col < c)
                {
                    // Outside the finder pattern area (on the border)
                    if (i == -1 || i == 7 || j == -1 || j == 7)
                    {
                        arr[row][col] = 0; // white
                        reserved.insert({row, col});
                    }
                }
            }
        }
    }

    // Function to place Alignment Pattern
    void place_alignment_pattern(vector<vector<int>> &arr, int x, int y, set<pair<int, int>> &reserved)
    {
        int r = arr.size();
        int c = arr[0].size();

        // Outer 5x5 black square
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                int row = x + i;
                int col = y + j;
                if (row >= 0 && row < r && col >= 0 && col < c)
                {
                    arr[row][col] = 1;
                    reserved.insert({row, col});
                }
            }
        }

        // Inner 3x3 white square
        for (int i = 1; i < 4; i++)
        {
            for (int j = 1; j < 4; j++)
            {
                int row = x + i;
                int col = y + j;
                if (row >= 0 && row < r && col >= 0 && col < c)
                {
                    arr[row][col] = 0;
                    reserved.insert({row, col});
                }
            }
        }

        // Single black pixel in center
        int center_row = x + 2;
        int center_col = y + 2;
        if (center_row >= 0 && center_row < r && center_col >= 0 && center_col < c)
        {
            arr[center_row][center_col] = 1;
            reserved.insert({center_row, center_col});
        }
    }

    // Function to place Timer Pattern
    void place_timing_patterns(vector<vector<int>> &arr, set<pair<int, int>> &reserved)
    {
        int n = arr.size();

        // Horizontal timing pattern (row = 6, cols from 8 to n-9)
        for (int col = 8; col <= n - 9; ++col)
        {
            int value = (col % 2 == 0) ? 1 : 0; // alternate black and white
            arr[6][col] = value;
            reserved.insert({6, col});
        }

        // Vertical timing pattern (col = 6, rows from 8 to n-9)
        for (int row = 8; row <= n - 9; ++row)
        {
            int value = (row % 2 == 0) ? 1 : 0; // alternate black and white
            arr[row][6] = value;
            reserved.insert({row, 6});
        }
    }

    // Function to reserve Format Pattern space
    void place_format_patterns(vector<vector<int>> &arr, set<pair<int, int>> &reserved)
    {
        int n = arr.size();

        for (int i = 0; i < 8; i++)
        {
            if (i == 6)
                continue;
            reserved.insert({i, 8});
        }
        for (int j = 0; j <= 8; j++)
        {
            if (j == 6)
                continue;
            reserved.insert({8, j});
        }
        for (int j = n - 8; j < n; j++)
        {
            reserved.insert({8, j});
        }
        for (int i = n - 7; i < n; i++)
        {
            reserved.insert({i, 8});
        }
    }

    // Function to place Data Modules
    void place_data_modules(vector<vector<int>> &arr, const string &final_binary_message, const set<pair<int, int>> &reserved)
    {
        int n = arr.size();
        int dir = -1; // -1 for up, +1 for down
        int bit_index = 0;
        int total_bits = final_binary_message.size();

        for (int col = n - 1; col > 0; col -= 2)
        {
            if (col == 6)
                col--; // Skip timing column

            int row = (dir == -1) ? n - 1 : 0;

            while (row >= 0 && row < n)
            {
                for (int offset = 0; offset < 2; offset++)
                {
                    int current_col = col - offset;
                    int current_row = row;

                    if (reserved.find({current_row, current_col}) == reserved.end())
                    {
                        if (bit_index < total_bits)
                        {
                            arr[current_row][current_col] = (final_binary_message[bit_index] == '1') ? 1 : 0;
                            bit_index++;
                        }
                        else
                        {
                            arr[current_row][current_col] = 0; // Padding zeroes if we run out of bits
                        }
                    }
                }
                row += dir;
            }

            dir *= -1; // Flip direction
        }
    }

    // Function to evaluate Penalty of given Mask
    int evaluate_penalty(const vector<vector<int>> &arr)
    {
        int n = arr.size();
        int penalty = 0;

        // Rule 1: Consecutive same modules in row and column
        for (int i = 0; i < n; i++)
        {
            int row_count = 1, col_count = 1;
            for (int j = 1; j < n; j++)
            {
                // Row check
                if (arr[i][j] == arr[i][j - 1])
                    row_count++;
                else
                {
                    if (row_count >= 5)
                        penalty += 3 + (row_count - 5);
                    row_count = 1;
                }

                // Column check
                if (arr[j][i] == arr[j - 1][i])
                    col_count++;
                else
                {
                    if (col_count >= 5)
                        penalty += 3 + (col_count - 5);
                    col_count = 1;
                }
            }
            if (row_count >= 5)
                penalty += 3 + (row_count - 5);
            if (col_count >= 5)
                penalty += 3 + (col_count - 5);
        }

        // Rule 2: 2x2 blocks
        for (int i = 0; i < n - 1; i++)
        {
            for (int j = 0; j < n - 1; j++)
            {
                int val = arr[i][j];
                if (arr[i + 1][j] == val &&
                    arr[i][j + 1] == val &&
                    arr[i + 1][j + 1] == val)
                {
                    penalty += 3;
                }
            }
        }

        // Rule 3: Finder-like patterns in rows and columns
        vector<int> pattern = {1, 0, 1, 1, 1, 0, 1};
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= n - 7; j++)
            {
                // Row pattern
                bool match = true;
                for (int k = 0; k < 7; k++)
                    if (arr[i][j + k] != pattern[k])
                        match = false;
                if (match)
                {
                    // Look for whitespace around pattern
                    if ((j >= 4 && all_of(arr[i].begin() + j - 4, arr[i].begin() + j, [](int x)
                                          { return x == 0; })) ||
                        (j + 7 + 4 <= n && all_of(arr[i].begin() + j + 7, arr[i].begin() + j + 11, [](int x)
                                                  { return x == 0; })))
                    {
                        penalty += 40;
                    }
                }

                // Column pattern
                match = true;
                for (int k = 0; k < 7; k++)
                    if (arr[j + k][i] != pattern[k])
                        match = false;
                if (match)
                {
                    bool white_before = true, white_after = true;
                    if (j >= 4)
                    {
                        for (int k = j - 4; k < j; k++)
                            if (arr[k][i] != 0)
                                white_before = false;
                    }
                    if (j + 11 <= n)
                    {
                        for (int k = j + 7; k < j + 11; k++)
                            if (arr[k][i] != 0)
                                white_after = false;
                    }
                    if (white_before || white_after)
                        penalty += 40;
                }
            }
        }

        // Rule 4: Dark module percentage
        int dark = 0;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (arr[i][j] == 1)
                    dark++;

        int total = n * n;
        int percent = (100 * dark) / total;
        int k = abs(percent - 50) / 5;
        penalty += k * 10;

        return penalty;
    }

    // Function to apply given Mask Number
    vector<vector<int>> apply_mask(vector<vector<int>> arr, int mask_number, const set<pair<int, int>> &reserved)
    {
        int n = arr.size();
        vector<vector<int>> arr_new = arr;
        auto mask_condition = [&](int i, int j) -> bool
        {
            switch (mask_number)
            {
            case 0:
                return (((i + j) % 2) == 0);
            case 1:
                return ((i % 2) == 0);
            case 2:
                return ((j % 3) == 0);
            case 3:
                return (((i + j) % 3) == 0);
            case 4:
                return (((int)(floor(i / 2) + floor(j / 3)) % 2) == 0);
            case 5:
                return (((i * j) % 2 + (i * j) % 3) == 0);
            case 6:
                return ((((i * j) % 2 + (i * j) % 3) % 2) == 0);
            case 7:
                return ((((i + j) % 2 + (i * j) % 3) % 2) == 0);
            default:
                return false;
            }
        };

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (reserved.find({i, j}) != reserved.end())
                    continue; // Skip functional patterns
                if (mask_condition(i, j))
                    arr_new[i][j] ^= 1; // Apply XOR (flip)
            }
        }
        return arr_new;
    }

    // Function to place Format String
    void place_format_string(const string &format_bits, vector<vector<int>> &arr)
    {
        int n = arr.size();
        // Top-left horizontal (skip col=6)
        int idx = 0;
        for (int col = 0; col <= 8; col++)
        {
            if (col == 6)
                continue; // timing pattern column
            arr[8][col] = format_bits[idx++] - '0';
        }
        // Top-left vertical (skip row=6)
        for (int row = 7; row >= 0; row--)
        {
            if (row == 6)
                continue; // timing pattern row
            arr[row][8] = format_bits[idx++] - '0';
        }

        idx = 0;
        // Bottom-left vertical
        for (int row = n - 1; row >= n - 7; row--)
        {
            arr[row][8] = format_bits[idx++] - '0';
        }

        // Top-right horizontal
        for (int col = n - 8; col < n; col++)
        {
            arr[8][col] = format_bits[idx++] - '0';
        }
    }

    // Function to split given Binary Message String into 8 bit Codewords of desired length
    vector<string> split_binary_message(vector<string> binary_message, int version, int error_mode)
    {
        string bit_stream = "";
        for (string s : binary_message)
            bit_stream += s;

        int max_bits = data_codewords[error_mode][version] * 8;
        int remaining_bits = max_bits - bit_stream.size();

        if (remaining_bits > 0)
            bit_stream += string(min(4, remaining_bits), '0');

        while (bit_stream.size() % 8 != 0)
            bit_stream += "0";

        vector<string> codewords;
        for (int i = 0; i < bit_stream.size(); i += 8)
            codewords.push_back(bit_stream.substr(i, 8));

        vector<string> pad_bytes = {"11101100", "00010001"};
        int required_codewords = data_codewords[error_mode][version];

        int pad_index = 0;
        while (codewords.size() < required_codewords)
        {
            codewords.push_back(pad_bytes[pad_index]);
            pad_index = 1 - pad_index;
        }

        return codewords;
    }

    // Function to covert Binary Codewords into Polynomial Coeffs
    vector<int> get_polynomial_coeff(vector<string> codewords)
    {
        vector<int> polynomial_coeff;
        for (auto s : codewords)
        {
            int val = 0;
            for (char c : s)
            {
                val = (val << 1) + (c - '0'); // shift left and add bit
            }
            polynomial_coeff.push_back(val);
        }
        return polynomial_coeff;
    }

    // Function to split the Polynomial Coeffs into desired number of blocks and groups
    vector<vector<int>> get_data_blocks(int error_mode, vector<int> polynomial_coeff, int version)
    {
        vector<vector<int>> poly_blocks;
        int num_ec_codewords;
        switch (error_mode)
        {
        case 'L':
            poly_blocks = split_into_blocks(polynomial_coeff, level_L_matrix, version);
            num_ec_codewords = level_L_matrix[version - 1][0];
            break;
        case 'M':
            poly_blocks = split_into_blocks(polynomial_coeff, level_M_matrix, version);
            num_ec_codewords = level_M_matrix[version - 1][0];
            break;
        case 'Q':
            poly_blocks = split_into_blocks(polynomial_coeff, level_Q_matrix, version);
            num_ec_codewords = level_Q_matrix[version - 1][0];
            break;
        case 'H':
            poly_blocks = split_into_blocks(polynomial_coeff, level_H_matrix, version);
            num_ec_codewords = level_H_matrix[version - 1][0];
            break;
        }
        return poly_blocks;
    }

    // Function to split the Polynomial Coeffs into desired number of blocks and groups
    int get_num_ec_codewords(int error_mode, vector<int> polynomial_coeff, int version)
    {
        vector<vector<int>> poly_blocks;
        int num_ec_codewords;
        switch (error_mode)
        {
        case 'L':
            poly_blocks = split_into_blocks(polynomial_coeff, level_L_matrix, version);
            num_ec_codewords = level_L_matrix[version - 1][0];
            break;
        case 'M':
            poly_blocks = split_into_blocks(polynomial_coeff, level_M_matrix, version);
            num_ec_codewords = level_M_matrix[version - 1][0];
            break;
        case 'Q':
            poly_blocks = split_into_blocks(polynomial_coeff, level_Q_matrix, version);
            num_ec_codewords = level_Q_matrix[version - 1][0];
            break;
        case 'H':
            poly_blocks = split_into_blocks(polynomial_coeff, level_H_matrix, version);
            num_ec_codewords = level_H_matrix[version - 1][0];
            break;
        }
        return num_ec_codewords;
    }

    // Function to convert given Integer array to Binary String
    string get_binary_string(vector<int> interleaved_message, int version)
    {
        string final_binary_message = "";
        for (auto it : interleaved_message)
        {
            string sbin = convert_to_k_bit_binary(it, 8);
            final_binary_message += sbin;
        }

        if (version != 1)
        {
            final_binary_message += "0000000";
        }
        return final_binary_message;
    }

    // Function to check for need of Alignment Pattern and place wherever necessary
    void check_and_place_alignment_pattern(int version, int n, vector<vector<int>> &arr, set<pair<int, int>> &reserved)
    {
        if (version != 1)
        {
            auto positions = alignment_pattern_positions[version];

            for (int x : positions)
            {
                for (int y : positions)
                {
                    // Skip the finder patterns' centers
                    if ((x == 6 && y == 6) ||
                        (x == 6 && y == n - 7) ||
                        (x == n - 7 && y == 6))
                    {
                        continue;
                    }
                    // place alignment pattern centered at (x, y)
                    place_alignment_pattern(arr, x - 2, y - 2, reserved);
                }
            }
        }
    }

    // Function to place black dot at fixed place as per standard
    void place_black_dot(int version, vector<vector<int>> &arr, set<pair<int, int>> &reserved)
    {
        arr[4 * version + 9][8] = 1;
        reserved.insert({4 * version + 9, 8});
    }

    // Function to calculate Penalty for every Mask Number
    vector<pair<int, int>> get_penalty_for_masks(vector<vector<int>> &arr, set<pair<int, int>> &reserved)
    {
        vector<pair<int, int>> all_penality;
        for (int i = 0; i < 8; i++)
        {
            all_penality.push_back({evaluate_penalty(apply_mask(arr, i, reserved)), i});
        }
        sort(all_penality.begin(), all_penality.end());
        return all_penality;
    }

    // Function to get and place Format String
    void get_and_place_format_string(int error_mode, int mask_number, vector<vector<int>> &masked_arr)
    {
        string complete_format = "";
        switch (error_mode)
        {
        case 'L':
            complete_format = format_string_L[mask_number];
            break;
        case 'M':
            complete_format = format_string_M[mask_number];
            break;
        case 'Q':
            complete_format = format_string_Q[mask_number];
            break;
        case 'H':
            complete_format = format_string_H[mask_number];
            break;
        }
        
        place_format_string(complete_format, masked_arr);
    }
};
