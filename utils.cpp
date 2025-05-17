#include <bits/stdc++.h>
using namespace std;
struct BlockInfo
{
    int num_blocks;
    int ecc_codewords_per_block;
    vector<int> data_codewords_per_block; // varies when blocks are unequal
};

class Utils
{
public:
    unordered_map<char, int> alphanum_table = {
        {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9}, {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'G', 16}, {'H', 17}, {'I', 18}, {'J', 19}, {'K', 20}, {'L', 21}, {'M', 22}, {'N', 23}, {'O', 24}, {'P', 25}, {'Q', 26}, {'R', 27}, {'S', 28}, {'T', 29}, {'U', 30}, {'V', 31}, {'W', 32}, {'X', 33}, {'Y', 34}, {'Z', 35}, {' ', 36}, {'$', 37}, {'%', 38}, {'*', 39}, {'+', 40}, {'-', 41}, {'.', 42}, {'/', 43}, {':', 44}};

    unordered_map<char, int> error_mode = {
        {'L', 1}, {'M', 0}, {'Q', 3}, {'H', 2}};

    unordered_map<int, int> capacity_L = {
        {1, 25}, {2, 47}, {3, 77}, {4, 114}, {5, 154}, {6, 195}, {7, 224}, {8, 279}, {9, 335}, {10, 395}};

    unordered_map<int, int> capacity_M = {
        {1, 20}, {2, 38}, {3, 61}, {4, 90}, {5, 122}, {6, 154}, {7, 178}, {8, 221}, {9, 262}, {10, 311}};

    unordered_map<int, int> capacity_Q = {
        {1, 16}, {2, 29}, {3, 47}, {4, 67}, {5, 87}, {6, 108}, {7, 125}, {8, 157}, {9, 189}, {10, 221}};

    unordered_map<int, int> capacity_H = {
        {1, 10}, {2, 20}, {3, 35}, {4, 50}, {5, 64}, {6, 84}, {7, 93}, {8, 122}, {9, 143}, {10, 174}};

    unordered_map<int, string> mode_indicator = {
        {0, "0001"}, // numeric
        {1, "0010"}, // alphanumeric
        {2, "0100"}  // byte
    };

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

    unordered_map<char, unordered_map<int, int>> data_codewords =
        {
            {'L', {{1, 19}, {2, 34}, {3, 55}, {4, 80}, {5, 108}}},
            {'M', {{1, 16}, {2, 28}, {3, 44}, {4, 64}, {5, 86}}},
            {'Q', {{1, 13}, {2, 22}, {3, 34}, {4, 48}, {5, 62}}},
            {'H', {{1, 9}, {2, 16}, {3, 26}, {4, 36}, {5, 46}}}
            // Extend as needed
    };

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
    unordered_map<int, vector<int>> alignment_pattern_positions = {
        {1, {}},
        {2, {6, 18}},
        {3, {6, 22}},
        {4, {6, 26}},
        {5, {6, 30}}};

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
};
