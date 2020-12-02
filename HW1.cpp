/* HW_1 / Scrambler*/

#include <algorithm>
#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string binary(unsigned int value)
{
    std::string result;
    while (value != 0)
    {
        result += std::to_string(value % 2);
        value /= 2;
    }
    while ((result.size() % 8) != 0)
        result.push_back('0');
    std::reverse(result.begin(), result.end());
    return result;
}

std::string lblock_shift(std::string& txt, int shift)
{
    std::string piece = txt.substr(0, shift);
    std::bitset<8 * 49> block(txt);
    block <<= shift;
    std::string new_txt = block.to_string();
    new_txt.erase(txt.size() - shift, piece.size());
    new_txt.insert(txt.size() - shift, piece);
    return new_txt;
}

std::string rblock_shift(std::string& txt, int shift)
{
    std::string piece = txt.substr(txt.size() - shift, shift);
    std::bitset<8 * 49> block(txt);
    block >>= shift;
    std::string new_txt = block.to_string();
    new_txt.erase(0, shift);
    new_txt.insert(0, piece);
    return new_txt;
}

std::string decimal(std::string& txt)
{
    std::string result = "";
    unsigned char a;
    for (int i = 0; i < txt.size(); i += 8)
    {
        a = 0;
        int eps = 7;
        for (int j = i; j < i + 8; j++)
        {
            if (txt[j] == '1')
                a += pow(2, eps);
            eps--;
        }
        result += a;
    }
    return result;
}

std::string binary(std::string& text)
{
    std::string binary_text;
    std::string current;
    for (int i = 0; i < text.size(); i++)
    {
        while (text[i] != 0)
        {
            current += text[i] % 2;
            text[i] /= 2;
        }
        std::reverse(current.begin(), current.end());
        binary_text += current;
    }   
    return binary_text;
}

int main(int argc, char *argv[])
{

    // 1) Creating text which needs to encrypt 
    //    and add its in file "input.txt"

    std::string text;
    text = "Hello, my name is Vanya! Let's encrypt this text.";
    std::ofstream plaintext("input.txt", std::ios::app);
    plaintext << text;
    plaintext.close();

    // 2) Creating blocks with value "size_of_block". 
    //    It's mean that size of one element in our vector 
    //    will be equal "size_of_block".

    unsigned int size_of_block = 12;
    std::vector<std::string> blocks;
    int t = 0;
    if ((text.size() % size_of_block) == 0)
        while (text.size() != t)
        {
                blocks.push_back(text.substr(t, size_of_block));
                t += size_of_block;
        }
    else
    {
        while ((t != text.size() - text.size() % size_of_block))
        {
            blocks.push_back(text.substr(t, size_of_block));
            t += size_of_block;
        }
        blocks.push_back(text.substr(t, text.size() % size_of_block));
    }
   
    // 3) Creating gamma and encrypting text by this gamma

    std::srand(std::time(nullptr));
    std::vector<char> keys(blocks.size());
    for (int i = 0; i < blocks.size(); i++)
    {
        int gamma = std::rand() % 158 + 33;
        keys[i] = static_cast<char>(gamma);
        for (int j = 0; j < blocks[i].size(); j++)
            blocks[i][j] ^= keys[i];
    }

    // 4)  Making the method "Cyclic shift"
    //     doing shift on 5 bytes left

    std::string encrypted_text;
    for (int i = 0; i < blocks.size(); i++)
        for (int j = 0; j < blocks[i].size(); j++)
            encrypted_text += blocks[i][j];

    // Transform text to binary code

    std::string new_encrypted_text;
    for (int i = 0; i < encrypted_text.size(); i++)
        new_encrypted_text.insert(i * 8, binary(encrypted_text[i]));
    encrypted_text = "";

    // Doing shift on 5 bytes left

    int shift = 5;
    std::string current_encrypted_text = lblock_shift(new_encrypted_text, shift);
    new_encrypted_text = "";

    // Transform text to decimal code

    std::string finally_encrypted_text = decimal(current_encrypted_text);
    current_encrypted_text = "";

    // Putting our final encrypted version in file

    std::ofstream encrypted_file("encrypted.txt");
        encrypted_file << finally_encrypted_text;
    encrypted_file.close();

    // And decrypting our text (Doing all in reversed algorithm)

    for (int i = 0; i < finally_encrypted_text.size(); i++)
        current_encrypted_text.insert(i * 8, binary(encrypted_text[i]));

    new_encrypted_text = rblock_shift(current_encrypted_text, shift);

    encrypted_text = decimal(new_encrypted_text);

    t = 0;
    std::vector<std::string> new_blocks(new_encrypted_text.size());
    for (int i = 0; i < encrypted_text.size(); i++)
        new_blocks[i] = encrypted_text[i];

    if ((encrypted_text.size() % size_of_block) == 0)
        while (text.size() != t)
        {
            new_blocks.push_back(encrypted_text.substr(t, size_of_block));
            t += size_of_block;
        }
    else
    {
        while ((t != encrypted_text.size() - encrypted_text.size() % size_of_block))
        {
            new_blocks.push_back(encrypted_text.substr(t, size_of_block));
            t += size_of_block;
        }
        new_blocks.push_back(encrypted_text.substr(t, text.size() % size_of_block));
    }

    for (int i = 0; i < new_blocks.size(); i++)
        for (int j = 0; j < new_blocks[i].size(); j++)
            new_blocks[i][j] ^= keys[i];

    std::ofstream decrypted("decrypted.txt");
    for (auto i : new_blocks)
        decrypted << i;
    decrypted.close();
    return 0;
}
