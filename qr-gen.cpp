#include <bits/stdc++.h>
#include <D:\C++ Projects\QR-Gen\qr-utils.cpp>
using namespace std;
vector<int> gf_log(256);
vector<int> gf_exp(512);
class qr
{

public:
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
    void Numeric() {}
    void AlphaNumeric()
    {
        QrUtils u;
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
            char_count_indicator = u.convert_to_k_bit_binary(char_count, 9);
        }
        else if (version >= 10 && version <= 26)
        {
            char_count_indicator = u.convert_to_k_bit_binary(char_count, 11);
        }
        else if (version <= 40)
        {
            char_count_indicator = u.convert_to_k_bit_binary(char_count, 13);
        }
        else
        {
            char_count_indicator = u.convert_to_k_bit_binary(char_count, 9);
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
                binary_message.push_back(u.convert_to_k_bit_binary(num, 11));
                i++;
            }
            else
            {
                int num = u.alphanum_table[message[i]];
                binary_message.push_back(u.convert_to_k_bit_binary(num, 6));
            }
        }

        vector<string> codewords = u.split_binary_message(binary_message, version, error_mode);

        vector<int> polynomial_coeff = u.get_polynomial_coeff(codewords);

        vector<vector<int>> poly_blocks = u.get_data_blocks(error_mode, polynomial_coeff, version);

        int num_ec_codewords = u.get_num_ec_codewords(error_mode, polynomial_coeff, version);

        vector<vector<int>> rs_blocks;
        for (const auto &block : poly_blocks)
        {
            vector<int> ec = generate_rs_codewords(block, num_ec_codewords);
            rs_blocks.push_back(ec);
        }

        vector<int> interleaved_message = u.interleave_blocks(poly_blocks, rs_blocks);

        string final_binary_message = u.get_binary_string(interleaved_message, version);

        int n = 21 + 4 * (version - 1);
        vector<vector<int>> arr(n, vector<int>(n, 0));
        set<pair<int, int>> reserved;

        u.place_finder_pattern(arr, 0, 0, reserved);
        u.place_finder_pattern(arr, 0, n - 7, reserved);
        u.place_finder_pattern(arr, n - 7, 0, reserved);

        u.check_and_place_alignment_pattern(version, n, arr, reserved);

        u.place_timing_patterns(arr, reserved);

        u.place_black_dot(version, arr, reserved);

        u.place_format_patterns(arr, reserved);

        u.place_data_modules(arr, final_binary_message, reserved);

        vector<pair<int, int>> all_penality = u.get_penalty_for_masks(arr, reserved);

        int mask_number = all_penality[0].second;

        vector<vector<int>> masked_arr = u.apply_mask(arr, mask_number, reserved);

        u.get_and_place_format_string(error_mode, mask_number, masked_arr);

        u.matrix_to_png(masked_arr, "qr_output2.png");
    }
    void Byte() {}
};
int main()
{
    qr q;
    q.init_galois_field();
    cout << "Enter Mode : \n1. Numeric\n2. Alphanumeric\n3. Byte" << endl;
    int mode;
    cin >> mode;
    cin.ignore();
    switch (mode)
    {
    case 1:
        q.Numeric();
        break;
    case 2:
        q.AlphaNumeric();
        break;
    case 3:
        q.Byte();
        break;
    default:
        cout << "Invalid Mode" << endl;
    }
    return 0;
}