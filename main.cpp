#include <bits/stdc++.h>
#include <D:\C++ Projects\QR-Gen\utils.cpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;
string convert_to_k_bit_binary(int i, int k)
{
    string bin = bitset<32>(i).to_string();
    return bin.substr(32 - k);
}
vector<int> gf_log(256);
vector<int> gf_exp(512); // allow wrap-around for mod 255

void init_galois_field()
{
    int primitive = 0x11d; // x^8 + x^4 + x^3 + x^2 + 1
    int x = 1;
    for (int i = 0; i < 255; i++)
    {
        gf_exp[i] = x;
        gf_log[x] = i;
        x <<= 1;
        if (x & 0x100)
        {
            x ^= primitive;
        }
    }
    for (int i = 255; i < 512; i++)
    {
        gf_exp[i] = gf_exp[i - 255];
    }
}

int gf_mul(int a, int b)
{
    if (a == 0 || b == 0)
        return 0;
    return gf_exp[gf_log[a] + gf_log[b]];
}

vector<int> generate_generator_poly(int degree)
{
    vector<int> gen = {1};
    for (int i = 0; i < degree; i++)
    {
        vector<int> next = {1, gf_exp[i]};
        vector<int> result(gen.size() + 1, 0);
        for (int j = 0; j < gen.size(); j++)
        {
            result[j] ^= gf_mul(gen[j], next[0]);
            result[j + 1] ^= gf_mul(gen[j], next[1]);
        }
        gen = result;
    }
    return gen;
}

vector<int> generate_rs_codewords(const vector<int> &data_block, int num_ec_codewords)
{
    vector<int> gen_poly = generate_generator_poly(num_ec_codewords);
    vector<int> msg(data_block);
    msg.resize(data_block.size() + num_ec_codewords, 0);

    for (int i = 0; i < data_block.size(); i++)
    {
        int coef = msg[i];
        if (coef != 0)
        {
            for (int j = 0; j < gen_poly.size(); j++)
            {
                msg[i + j] ^= gf_mul(gen_poly[j], coef);
            }
        }
    }

    vector<int> ec_codewords(msg.end() - num_ec_codewords, msg.end());
    return ec_codewords;
}
void matrix_to_png(
    const std::vector<std::vector<int>> &matrix,
    const std::string &filename,
    int border = 3, // border width in modules
    int scale = 10  // scale factor for each module
)
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
void alphanumeric_mode()
{
    Utils u;
    cout << "Input String (UPPERCASE AND DIGIT ONLY)" << endl;
    string message;
    getline(cin, message);

    string mode_indicator = u.mode_indicator[1];

    int char_count = message.length();

    cout << "Input Error Mode (L/M/Q/H)" << endl;
    char error_mode;
    cin >> error_mode;

    int version = u.find_min_version(error_mode, char_count);
    string char_count_indicator = "";
    if (version >= 1 && version <= 9)
    {
        char_count_indicator = convert_to_k_bit_binary(char_count, 9);
    }
    else if (version >= 10 && version <= 26)
    {
        char_count_indicator = convert_to_k_bit_binary(char_count, 11);
    }
    else if (version <= 40)
    {
        char_count_indicator = convert_to_k_bit_binary(char_count, 13);
    }
    else
    {
        char_count_indicator = convert_to_k_bit_binary(char_count, 9);
    }

    vector<string> binary_message;
    binary_message.push_back(mode_indicator);
    binary_message.push_back(char_count_indicator);

    for (int i = 0; i < char_count; i++)
    {
        if (i + 1 < char_count)
        {
            int num1 = u.alphanum_table[message[i]];
            int num2 = u.alphanum_table[message[i + 1]];
            int num = 45 * num1 + num2;
            binary_message.push_back(convert_to_k_bit_binary(num, 11));
            i++;
        }
        else
        {
            int num = u.alphanum_table[message[i]];
            binary_message.push_back(convert_to_k_bit_binary(num, 6));
        }
    }
    // cout << "binary message : ";
    // for (auto s : binary_message)
    // {
    //     cout << s << " ";
    // }
    // cout << endl;

    // 1. Join all binary segments into one long bit stream
    string bit_stream = "";
    for (string s : binary_message)
        bit_stream += s;

    // 2. Add terminator bits if needed (up to 4 bits, or less if it exceeds limit)
    // int version = u.find_min_version(error_mode, message.length());
    int max_bits = u.data_codewords[error_mode][version] * 8;
    int remaining_bits = max_bits - bit_stream.size();

    if (remaining_bits > 0)
        bit_stream += string(min(4, remaining_bits), '0');

    // 3. Make sure bit_stream length is multiple of 8 (add 0s if needed)
    while (bit_stream.size() % 8 != 0)
        bit_stream += "0";

    // 4. Split into 8-bit codewords
    vector<string> codewords;
    for (int i = 0; i < bit_stream.size(); i += 8)
        codewords.push_back(bit_stream.substr(i, 8));

    // 5. Add pad bytes alternately if total codewords are less than required
    vector<string> pad_bytes = {"11101100", "00010001"};
    int required_codewords = u.data_codewords[error_mode][version];

    int pad_index = 0;
    while (codewords.size() < required_codewords)
    {
        codewords.push_back(pad_bytes[pad_index]);
        pad_index = 1 - pad_index; // Alternate pad byte
    }

    // cout << "\nFinal 8-bit Codewords:\n";
    // for (int i = 0; i < codewords.size(); ++i)
    // {
    //     cout << codewords[i] << " ";
    //     if ((i + 1) % 8 == 0)
    //         cout << endl;
    // }

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

    // for (auto i : polynomial_coeff)
    // {
    //     cout << i << " ";
    // }
    // cout << endl;

    vector<vector<int>> poly_blocks;
    int num_ec_codewords;
    switch (error_mode)
    {
    case 'L':
        poly_blocks = u.split_into_blocks(polynomial_coeff, u.level_L_matrix, version);
        num_ec_codewords = u.level_L_matrix[version - 1][0];
        break;
    case 'M':
        poly_blocks = u.split_into_blocks(polynomial_coeff, u.level_M_matrix, version);
        num_ec_codewords = u.level_M_matrix[version - 1][0];
        break;
    case 'Q':
        poly_blocks = u.split_into_blocks(polynomial_coeff, u.level_Q_matrix, version);
        num_ec_codewords = u.level_Q_matrix[version - 1][0];
        break;
    case 'H':
        poly_blocks = u.split_into_blocks(polynomial_coeff, u.level_H_matrix, version);
        num_ec_codewords = u.level_H_matrix[version - 1][0];
        break;
    }

    vector<vector<int>> rs_blocks;
    for (const auto &block : poly_blocks)
    {
        vector<int> ec = generate_rs_codewords(block, num_ec_codewords);
        rs_blocks.push_back(ec);
    }
    // cout << "reed-solomon error codewords :\n";
    // for (auto it : rs_blocks)
    // {
    //     for (auto itt : it)
    //     {
    //         cout << itt << " ";
    //     }
    //     cout << endl;
    // }

    vector<int> interleaved_message = interleave_blocks(poly_blocks, rs_blocks);

    // for (auto it : interleaved_message)
    // {
    //     cout << it << " ";
    // }
    // cout << endl;

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

    // cout << "final binary message :" << final_binary_message << endl;

    int n = 21 + 4 * (version - 1);
    vector<vector<int>> arr(n, vector<int>(n, 0));
    set<pair<int, int>> reserved;

    place_finder_pattern(arr, 0, 0, reserved);
    place_finder_pattern(arr, 0, n - 7, reserved);
    place_finder_pattern(arr, n - 7, 0, reserved);

    if (version != 1)
    {
        auto positions = u.alignment_pattern_positions[version];

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

    place_timing_patterns(arr, reserved);

    arr[4 * version + 9][8] = 1;
    reserved.insert({4 * version + 9, 8});

    place_format_patterns(arr, reserved);

    place_data_modules(arr, final_binary_message, reserved);

    vector<pair<int, int>> all_penality;
    for (int i = 0; i < 8; i++)
    {
        all_penality.push_back({evaluate_penalty(apply_mask(arr, i, reserved)), i});
    }
    sort(all_penality.begin(), all_penality.end());

    // for (auto it : all_penality)
    // {
    //     cout << it.second << " " << it.first << endl;
    // }

    int mask_number = all_penality[0].second;
    // cout << mask_number << endl;

    vector<vector<int>> masked_arr = apply_mask(arr, mask_number, reserved);

    // for (auto i : masked_arr)
    // {
    //     for (auto j : i)
    //     {
    //         cout << j << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    string complete_format = "";
    switch (error_mode)
    {
    case 'L':
        complete_format = u.format_string_L[mask_number];
        break;
    case 'M':
        complete_format = u.format_string_M[mask_number];
        break;
    case 'Q':
        complete_format = u.format_string_Q[mask_number];
        break;
    case 'H':
        complete_format = u.format_string_H[mask_number];
        break;
    }

    place_format_string(complete_format, masked_arr);

    // for (auto i : masked_arr)
    // {
    //     for (auto j : i)
    //     {
    //         cout << j << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    matrix_to_png(masked_arr, "qr_output.png");
}
int main()
{
    init_galois_field();
    alphanumeric_mode();
    return 0;
}