#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>

using namespace std;

void CleanText(string &text)
{
    // Text to upper case
    transform(text.begin(), text.end(), text.begin(), ::toupper);

    // clean special characters
    for (int i = 0; i < text.size(); i++)
    {

        // Finding the character whose
        // ASCII value fall under this
        // range
        if (text[i] < 'A' || text[i] > 'Z')
        {
            // erase function to erase
            // the character
            text.erase(i, 1);
            i--;
        }
    }
}

void SetParameters(string text, string &fasta, unsigned int &offset, unsigned int &tries)
{
    if (!text.empty() && text[0] == '#')
        text.erase(0, 1); // Remove first character if it's '#'

    stringstream ss(text);
    string token;
    unsigned int pos = 0;

    while (getline(ss, token, ','))
    {
        if (pos == 1)
        {
            offset = stoi(token);
        }
        else if (pos == 2)
        {
            tries = stoi(token);
        }
        else
        {
            fasta = token;
        }

        pos++;
    }
}

void Encrypt(const string &filePath)
{
    cout << "Reading file..." << endl;
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file!" << endl;
        return;
    }

    string line, rule, text, fasta;
    unsigned int offset = 0, tries = 0;
    unordered_map<char, char> protein_dict = {{'A', '\0'}, {'C', '\0'}, {'D', '\0'}, {'E', '\0'}, {'F', '\0'}, {'G', '\0'}, {'H', '\0'}, {'I', '\0'}, {'K', '\0'}, {'L', '\0'}, {'M', '\0'}, {'N', '\0'}, {'P', '\0'}, {'Q', '\0'}, {'R', '\0'}, {'S', '\0'}, {'T', '\0'}, {'V', '\0'}, {'W', '\0'}, {'Y', '\0'}, {'B', 'Z'}, {'Z', 'B'}, {'X', 'J'}, {'J', 'X'}, {'U', 'O'}, {'O', 'U'}};

    while (getline(file, line))
    {
        if (line[0] == '#')
        {
            rule = line;
            // Extract the parameters
            SetParameters(rule, fasta, offset, tries);
            // Set text to an empty string again
            text = "";
        }
        text += line;
        // Clean text
        CleanText(text);
        // Update the vocabulary only when there is a clean text;
    }

    file.close();
}

int main()
{
    const string filePath = "../encrypt/Original_document.txt";

    Encrypt(filePath);
    return 0;
}
