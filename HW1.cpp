/* HW_1 / Scrambler*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct block
{
    std::vector<unsigned char> part; /*вектор блока, состоящий из символов*/
};

/*функция для конвертации строки в ввектор с заданными блоками*/
void constructor(std::string& input, std::vector<block>& to_vec, size_t size_of_block)
{

    int num_of_block = 0;

    if (input.size() % size_of_block != 0)                          /*определяем размерность массива*/
        to_vec.resize(input.size() / size_of_block + 1);
    else
        to_vec.resize(input.size() / size_of_block);

    while (!input.empty())
    {
        for (int i = 0; i < size_of_block; i++)
        {

            if (input.empty())                                      /*если больше нечего класть в вектор, реализуем заглушку*/
            {
                unsigned char go_to = std::rand() % 93 + 33;
                to_vec[num_of_block].part[i] = go_to;
            }
            else
            {
                to_vec[num_of_block].part.resize(size_of_block);
                to_vec[num_of_block].part[i] = input[0];
                input.erase(0, 1);
            }
        }
        num_of_block++;
    }
}

/*функция для создании гаммы*/
void create_gamma(block& gamma_container, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        unsigned char gamma = std::rand() % 31 + 33;
        gamma_container.part.push_back(gamma);
    }
}

/*опереатор перегрузки для гаммирования*/
block operator^(block& current, block& gamma)
{
    for (int i = 0; i < current.part.size(); i++)
        current.part[i] = current.part[i] ^ gamma.part[i];
    return current;
}

/*оператор перегрузки для циклического сдвига вправо*/
block operator>>(block& current, size_t num_of_shift)
{
    block result;                                          /*создаем блок, в которую будем класть сдвинутый текст*/
    result.part.resize(current.part.size());               /*задаем ему размерность*/

    std::vector<unsigned char> bytes(current.part.size()); /*создаем массив для записи в него символов, которые удалятся при сдвиге*/

    for (int i = 0; i < current.part.size(); i++)
    {
        bytes[i] = current.part[i] << (8 - num_of_shift);  /*записываем удаленные символы в созданный массив*/
        result.part[i] = current.part[i] >> num_of_shift;  /*производим сдвиг и записываем полученное в результирующий массив*/
    }

    for (int i = 1; i < current.part.size(); i++)          /*при помощи логического ИЛИ складываем результирующие биты с предыдущими уже*/
        result.part[i] |= bytes[i - 1];                    /*сдвинутыми для выполнения правильного сдвига*/

    result.part[0] |= bytes[current.part.size() - 1];      /*пишем отдельно, так как в цикл фор этот кусочек алгоритма не записался*/

    return result;                                         /*возвращаем результат*/
}

/*оператор перегрузки для циклического сдвига влево*/
block operator<<(block& current, size_t num_of_shift)
{
    block result;
    result.part.resize(current.part.size());
    std::vector<unsigned char> bytes(current.part.size());

    for (int i = 0; i < current.part.size(); i++)
    {
        bytes[i] = current.part[i] >> (8 - num_of_shift);
        result.part[i] = current.part[i] << num_of_shift;
    }

    for (int i = 0; i < current.part.size() - 1; i++)
        result.part[i] |= bytes[i + 1];

    result.part[current.part.size() - 1] |= bytes[0];

    return result;
}

/*реализация алгоритма шифрования*/
void do_encrypting(std::vector<block>& vec, size_t size_of_shift, std::string side, int& secret_key)
{
    for (int i = 0; i < vec.size(); i++)
    {
        std::srand(secret_key);

        block current;                                   /*создаю временный блок для реализации перегрузки оператора ^*/
        for (int j = 0; j < vec[i].part.size(); j++)
            current.part.push_back(vec[i].part[j]);      /*перевожу все значения блока во временный блок*/

        block gamma;
        create_gamma(gamma, vec[i].part.size());

        current = current ^ gamma;                       /*шифрую блок методом гаммирования*/

        gamma.part.clear();

        if (side == "right")
            current = current >> size_of_shift;          /*выполняю циклический сдвиг*/
        else
            current = current << size_of_shift;              

        for (int k = 0; k < vec[i].part.size(); k++)     /*заменяю значения исходного блока на значения запишфрованного блока*/
            vec[i].part[k] = current.part[k];            
    }

}

/*реализация алгоритма дешифрования*/
void do_decrypting(std::vector<block>& vec, size_t size_of_shift, std::string side, int& secret_key, int size_of_text)
{
    for (int i = 0; i < vec.size(); i++)
    {
        std::srand(secret_key);

        block current;

        for (int j = 0; j < vec[i].part.size(); j++)      /*копирую блок во временный массив для удобства в дешифровкe*/
            current.part.push_back(vec[i].part[j]);

        if (side == "right")                              /*сдвигаю байты обратно на свое место*/
            current = current << size_of_shift;
        else
            current = current >> size_of_shift;

        block gamma;                                      /*создаю гамму*/
        create_gamma(gamma, vec[i].part.size());

        current = current ^ gamma;                        /*произвожу обратное гаммирование*/

        gamma.part.clear();

        for (int k = 0; k < vec[i].part.size(); k++)
            vec[i].part[k] = current.part[k];
    }

    /*отрезвем заглушку*/
    int count = 0;
    for (int i = 0; i < vec.size(); i++)
        for (int j = 0; j < vec[i].part.size(); j++)
        {
            count++;
            if (size_of_text == count)
            {

                int k = j + 1;
                int delta = vec[i].part.size() - k;
                while (delta != 0)
                {
                    vec[i].part.erase(vec[i].part.begin() + k);
                    delta--;
                }
                break;
            }
        }

}

/*перегрузка оператора для считывания текста из файла*/
void operator>>(std::ifstream& input, std::string& result)
{
    if (!input.is_open())
    {
        std::cout << "File not found";
        exit(0);
    }
    else
    {
        std::string current;
        while (!input.eof()) /*пока не дойдет до конца файла*/
        {
            std::getline(input, current);
            result += current;
        }
    }
}

/*перегрузка оператора для записи текста в файл*/
std::ostream& operator<<(std::ofstream& output, std::vector<block>& vec)
{
    for (int i = 0; i < vec.size(); i++)
        for (int j = 0; j < vec[i].part.size(); j++)
            output << vec[i].part[j];
    return output;
}

/*запись текста в бинарный файл*/
void bin_out(std::ofstream& output, std::vector<block>& vec)
{
    for (int i = 0; i < vec.size(); i++)
        for (int j = 0; j < vec[i].part.size(); j++)
            output.write((char*)&vec[i].part[j], sizeof(vec[i].part[j]));
}

/*считывание текста из бинарного файла*/
void bin_in(std::ifstream& input, std::string& str)
{
    if (!input.is_open())
        std::cout << "File not found";
    else
    {
        unsigned char k;
        std::vector<unsigned char> vec;
        while (!input.eof())
        {
            input.read((char*)&k, sizeof(k));
            vec.push_back(k);
        }

        vec.erase(vec.begin() + vec.size() - 1);

        for (auto& i : vec)
            str += i;
    }
}

        /*arg_counts----arg_values*/
int main(int argc, char* argv[])
{

    /*===================================================================================================*/
    /* иниициализация */

    std::string input_file = argv[1];                                                           /*путь к файлу*/

    std::string command = argv[2];                                                              /*название команды (encrypt/decrypt)*/

    int size_of_text = std::stoi(argv[3]);                                                      /*размер шифруемого текста*/

    size_t size_of_block = std::stoi(argv[4]);                                                  /*размер блока*/

    std::string side = argv[5];                                                                 /*в какую сторону производится сдвиг при шифровании*/

    size_t size_of_shift = std::stoi(argv[6]);                                                  /*размер сдвига*/

    int secret_key = std::stoi(argv[7]);                                                        /*ключ криптосистемы*/

    /*===================================================================================================*/

    std::string get_text;                                                                       /*переменная для получения текста*/

    if (command == "encrypt")
    {

        /*считываю текст из файла*/
        std::ifstream InputFile(input_file, std::ios::in);
        InputFile >> get_text;

        if (InputFile.is_open())
            std::cout << "It's your text: " << get_text << std::endl;

        InputFile.close();

        /*конвертирую строчку в массив, одновременно разделяя их на блоки*/
        std::vector<block> input_vector;
        constructor(get_text, input_vector, size_of_block);

        /*осуществление процесса шифрования текста*/
        do_encrypting(input_vector, size_of_shift, side, secret_key);

        /*запись зашифрованного текста в файл*/
        std::ofstream OutputBinary("Plaintext.txt", std::ios::binary);
        bin_out(OutputBinary, input_vector);
        OutputBinary.close();
        
    }
    else
    {
        /*считывание файла для дешифрования*/
        std::string input;
        std::ifstream InputBinary(input_file, std::ios::binary);
        bin_in(InputBinary, input);
        InputBinary.close();

        /*конвертация полученной строки*/
        std::vector<block> input_vec;
        constructor(input, input_vec, size_of_block);

        /*дешифруем текст*/
        do_decrypting(input_vec, size_of_shift, side, secret_key, size_of_text);

        /*создаю файл для копирования расшифрованного сообщения*/
        std::ofstream Decrypted_text("Decrypted.txt", std::ios::out);
        Decrypted_text << input_vec;
        Decrypted_text.close();
    }
        
    return 0;
}
