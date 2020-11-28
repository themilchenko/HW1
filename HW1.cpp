/* HW_1 / Scrambler*/

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
    unsigned int size_of_block;
    std::cout << "Enter the size of your block: ";
    std::cin >> size_of_block;
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
    std::cout << "Intermediate text: " ;
    for (auto i : blocks)
        std::cout << i;
    std::ofstream intermediate("intermediate.txt");
    for (auto i : blocks)
        intermediate << i;
    intermediate.close();
    std::cout << std::endl;

    // 4)  Making the method "Cyclic shift"
    // doing shift on 5 bytes left

    std::string encrypted_text;
    for (int i = 0; i < blocks.size(); i++)
        for (int j = 0; j < blocks[i].size(); j++)
            encrypted_text += blocks[i][j];

    char shift = 5;
    char t1 = 0x00;
    char t2 = 0x00;
    for (int i = encrypted_text.size(); i >= 0; i--)
    {
        if (i == encrypted_text.size())
        {
            t1 = encrypted_text[i];
            encrypted_text[i] <<= shift;
        }
        else
        {
            t2 = encrypted_text[i];
            encrypted_text[i] <<= shift;
            encrypted_text[i] |= ((t1 & 0x1F >> (8 - shift)));
            t1 = t2;
        }
    }
    std::cout << "Shift cycling: ";
    for (auto i : blocks)
        std::cout << i;

    std::ofstream encrypted_file("encrypted.txt");
    for (auto i : encrypted_text)
        encrypted_file << i;
    encrypted_file.close();

    // And decrypting our text (Doing all in reversed algorithm)
    char t1 = 0x00;
    char t2 = 0x00;
    for (int i = 0; i <= encrypted_text.size(); i++)
    {
        if (i == 0)
        {
            t = encrypted_text[i];
            encrypted_text[i] >>= shift;
        }
        else
        {
            t2 = encrypted_text[i];
            encrypted_text[i] >>= shift;
            encrypted_text[i] |= ((t1 & 0x1F) << (8 - shift));

            if (i != encrypted_text.size())
                t1 = t2;
        }
    }
    std::ofstream afterintermidate("after.txt");
    for (auto i : encrypted_text)
        afterintermidate << i;
    afterintermidate.close();

    t = 0;
    std::vector<std::string> new_blocks;
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