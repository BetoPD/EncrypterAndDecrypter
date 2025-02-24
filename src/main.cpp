#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <chrono>
// if windows use <algorithm> instead of <bits/stdc++.h> using preprocessor directives
#ifdef _WIN32
#include <algorithm> // Para Windows
#include <cctype>
#endif

using namespace std;

struct Parameters
{
    string fasta;
    string text;
    unsigned int offset;
    unsigned int tries;
    unsigned int thirdParameter;
    unordered_map<char, char> protein_dict;

    Parameters(string fasta, unsigned int offset, unsigned int tries, string text)
    {
        this->fasta = fasta;
        this->offset = offset;
        this->tries = tries;
        this->text = text;
        this->protein_dict = {{'A', '\0'}, {'C', '\0'}, {'D', '\0'}, {'E', '\0'}, {'F', '\0'}, {'G', '\0'}, {'H', '\0'}, {'I', '\0'}, {'K', '\0'}, {'L', '\0'}, {'M', '\0'}, {'N', '\0'}, {'P', '\0'}, {'Q', '\0'}, {'R', '\0'}, {'S', '\0'}, {'T', '\0'}, {'V', '\0'}, {'W', '\0'}, {'Y', '\0'}, {'B', 'Z'}, {'Z', 'B'}, {'X', 'J'}, {'J', 'X'}, {'U', 'O'}, {'O', 'U'}};
    }

    Parameters()
    {
        this->fasta = "";
        this->offset = 0;
        this->tries = 0;
        this->text = "";
        this->protein_dict = {{'A', '\0'}, {'C', '\0'}, {'D', '\0'}, {'E', '\0'}, {'F', '\0'}, {'G', '\0'}, {'H', '\0'}, {'I', '\0'}, {'K', '\0'}, {'L', '\0'}, {'M', '\0'}, {'N', '\0'}, {'P', '\0'}, {'Q', '\0'}, {'R', '\0'}, {'S', '\0'}, {'T', '\0'}, {'V', '\0'}, {'W', '\0'}, {'Y', '\0'}, {'B', 'Z'}, {'Z', 'B'}, {'X', 'J'}, {'J', 'X'}, {'U', 'O'}, {'O', 'U'}};
        this->thirdParameter = 0;
    }
};

void CleanText(string &text)
{
    // Convert all characters to uppercase.
    transform(text.begin(), text.end(), text.begin(), ::toupper);

    // Remove characters that are not A-Z and not white space.
    text.erase(
        remove_if(text.begin(), text.end(),
                  [](char c)
                  {
                      // Keep the character if it is an uppercase letter or a white space.
                      return !((c >= 'A' && c <= 'Z') || isspace(c));
                  }),
        text.end());
}

void GetParameters(string text, Parameters *p)
{
    if (!text.empty() && text[0] == '#')
        text.erase(0, 1); // Remove first character if it's '#'

    stringstream ss(text);
    string token;
    unsigned int pos = 0;

    while (getline(ss, token, ','))
    {
        if (pos == 0)
        {
            p->fasta = "../instructions/" + token;
        }
        else if (pos == 1)
        {
            p->offset = stoi(token);
        }
        else if (pos == 2)
        {
            p->tries = stoi(token);
        }
        else
        {
            p->thirdParameter = stoi(token);
        }

        pos++;
    }
}

void SetParametersVector(vector<Parameters *> *p, string filePath = "../encrypt/Original_document.txt")
{
    cout << "Reading file..." << endl;
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file!" << endl;
        return;
    }

    // Variables
    string line, rule, text;

    // Pointer to a Parameters object
    Parameters *params;

    while (getline(file, line))
    {
        if (line[0] == '#')
        {
            params = new Parameters();
            rule = line;
            // Extract the parameters
            GetParameters(rule, params);
            // Set text to an empty string again
            p->push_back(params);
        }
        else
        {
            text = line;
            // Clean text
            CleanText(text);
            // Add the text to the parameters
            params->text += text;
        }
    }

    file.close();
}

void CreateVocabulary(Parameters *p, unsigned int tries = 0)
{
    ifstream fasta(p->fasta);

    if (!fasta.is_open())
        return;

    unordered_map<char, bool> used;
    string line;

    unsigned int asciiOffset = 0;
    bool fileFinished = true;

    while (getline(fasta, line))
    {
        // remove \r and \n
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        if (line[0] != '>')
            continue;

        if (line.size() < p->offset)
            continue;

        asciiOffset = (int)line[p->offset];
        fileFinished = false;
        break;
    }

    if (fileFinished)
    {
        if (tries < p->tries)
        {
            return CreateVocabulary(p, tries + 1);
        }
        // raise an error
        throw std::runtime_error("Could't build vocabulary");
        return;
    }

    // move to the top of the file
    fasta.clear();
    fasta.seekg(0, ios::beg);
    unsigned int currentOffset = asciiOffset;
    char letter;

    const char amino_acids[] = {'A', 'T', 'W', 'N', 'F', 'D', 'L', 'V', 'Y', 'C',
                                'P', 'H', 'E', 'M', 'G', 'Q', 'R', 'I', 'S', 'K'};

    while (getline(fasta, line))
    {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        if (line[0] == '>')
            continue;

        if (line.size() < currentOffset)
        {
            currentOffset -= line.size();
            continue;
        }

        letter = line[currentOffset - 1];

        if (used[letter] == true)
            continue;

        for (char aa : amino_acids)
        {
            if (p->protein_dict[aa] != '\0')
                continue;

            // safely check if the letter is already used, if it does not exists in the map
            p->protein_dict[aa] = letter;
            used[letter] = true;
            break;
        }

        currentOffset = asciiOffset;

        if (used.size() == 20)
        {
            fileFinished = false;
            break;
        }
    }

    if (fileFinished)
    {
        if (tries < p->tries)
        {
            return CreateVocabulary(p, tries + 1);
        }
        // raise an error
        throw std::runtime_error("Could't build vocabulary");
        return;
    }

    fasta.close();
}

void PrintText(Parameters *p)
{
    for (int i = 0; i < p->text.size(); i++)
    {
        char c = p->text[i];

        switch (c)
        {
        case ' ':
            cout << ' ';
            break;
        case '\n':
            cout << '\n';
            break;
        case '\r':
            cout << '\r';
            break;
        default:
            cout << p->protein_dict[c];
            break;
        }
    }
}

void PrintVocabulary(Parameters *p)
{
    for (auto it = p->protein_dict.begin(); it != p->protein_dict.end(); it++)
    {
        cout << it->first << " -> " << it->second << endl;
    }
}

void Encrypt()
{
    std::cout << "Encrypting " << std::endl;
    // vector of parameter pointers
    vector<Parameters *> parameters;
    // Sets the vector with the parameters
    SetParametersVector(&parameters);
    // Create the vocabulary
    for (Parameters *p : parameters)
    {
        CreateVocabulary(p);
        // PrintVocabulary(p);
        // PrintText(p);
        std::cout << std::endl;
    }

    ofstream encryptedFile("../decrypt/Encrypted_document.cod");
    if (!encryptedFile.is_open())
    {
        cerr << "Error: Could not open file!" << endl;
        return;
    }

    for (Parameters *p : parameters)
    {
        // remove the ../instructions
        // encryptedFile << "#" << p->fasta << "," << p->offset << "," << p->tries << endl;
        encryptedFile << "#";
        string fasta = p->fasta;
        fasta.erase(0, 16);
        encryptedFile << fasta << "," << p->offset << "," << p->tries << endl;

        for (int i = 0; i < p->text.size(); i++)
        {
            char c = p->text[i];

            switch (c)
            {
            case ' ':
                encryptedFile << ' ';
                break;
            case '\n':
                encryptedFile << '\n';
                break;
            case '\r':
                encryptedFile << '\r';
                break;
            default:
                encryptedFile << p->protein_dict[c];
                break;
            }
        }
        encryptedFile << endl;
    }

    encryptedFile.close();
}

void Decrypt()
{
    std::cout << "Decrypting " << std::endl;
    vector<Parameters *> parameters;
    SetParametersVector(&parameters, "../decrypt/Encrypted_document.cod");

    for (Parameters *p : parameters)
    {
        CreateVocabulary(p);
        // PrintText(p);
        // PrintVocabulary(p);
        std::cout << std::endl;
    }

    ofstream decryptedFile("../solution/Decrypted_document.txt");
    if (!decryptedFile.is_open())
    {
        cerr << "Error: Could not open file!" << endl;
        return;
    }

    // invert the protein_dict
    for (Parameters *p : parameters)
    {
        unordered_map<char, char> inverted_dict;
        for (auto it = p->protein_dict.begin(); it != p->protein_dict.end(); it++)
        {
            inverted_dict[it->second] = it->first;
        }
        p->protein_dict = inverted_dict;
    }

    for (Parameters *p : parameters)
    {
        decryptedFile << "#";
        string fasta = p->fasta;
        fasta.erase(0, 16);
        decryptedFile << fasta << "," << p->offset << "," << p->tries << endl;

        for (int i = 0; i < p->text.size(); i++)
        {
            char c = p->text[i];

            switch (c)
            {
            case ' ':
                decryptedFile << ' ';
                break;
            case '\n':
                decryptedFile << '\n';
                break;
            case '\r':
                decryptedFile << '\r';
                break;
            default:
                decryptedFile << p->protein_dict[c];
                break;
            }
        }
        decryptedFile << endl;
    }
}

// receive parameters from the command line
int main(int argc, char *argv[])
{
    // start time
    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
    // 1 for encrypt, 2 for decrypt
    if (argc != 2)
    {
        cerr << "Error: Invalid number of arguments!" << endl;
        return 1;
    }

    string mode = argv[1];
    if (mode == "1")
    {
        cout << "Encrypting..." << endl;
        Encrypt();
    }
    else if (mode == "2")
    {
        cout << "Decrypting..." << endl;
        Decrypt();
    }
    else
    {
        cerr << "Error: Invalid argument!" << endl;
        return 1;
    }
    // end time
    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    // calculate the time
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end - start);

    cout << "Time: " << duration.count() << " microseconds" << endl;

    return 0;
}
