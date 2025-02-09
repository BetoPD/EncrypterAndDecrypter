#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>

using namespace std;

struct Parameters
{
    string fasta;
    string text;
    unsigned int offset;
    unsigned int tries;

    Parameters(string fasta, unsigned int offset, unsigned int tries, string text)
    {
        this->fasta = fasta;
        this->offset = offset;
        this->offset = offset;
        this->text = text;
    }

    Parameters()
    {
        fasta = "";
        offset = 0;
        tries = 0;
        text = "";
    }
};

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

void GetParameters(string text, Parameters *p)
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
            p->offset = stoi(token);
        }
        else if (pos == 2)
        {
            p->tries = stoi(token);
        }
        else
        {
            p->fasta = "../instructions/" + token;
        }

        pos++;
    }
}

void SetParametersVector(vector<Parameters *> *p)
{
    const string filePath = "../encrypt/Original_document.txt";
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
            // Clear the text
            text = "";
        }
        else
        {
            text += line;
            // Clean text
            CleanText(text);
            // Add the text to the parameters
            params->text += text;
        }
    }

    file.close();
}

void CreateVocabulary(Parameters *p)
{
    ifstream fasta(p->fasta);

    if (!fasta.is_open())
        return;

    unordered_map<char, char> protein_dict = {{'A', '\0'}, {'C', '\0'}, {'D', '\0'}, {'E', '\0'}, {'F', '\0'}, {'G', '\0'}, {'H', '\0'}, {'I', '\0'}, {'K', '\0'}, {'L', '\0'}, {'M', '\0'}, {'N', '\0'}, {'P', '\0'}, {'Q', '\0'}, {'R', '\0'}, {'S', '\0'}, {'T', '\0'}, {'V', '\0'}, {'W', '\0'}, {'Y', '\0'}, {'B', 'Z'}, {'Z', 'B'}, {'X', 'J'}, {'J', 'X'}, {'U', 'O'}, {'O', 'U'}};
    string line;

    int offset = p->offset;
    unsigned int ascii_value = 0;
    bool first = true;

    while (getline(fasta, line))
    {
        if (offset > line.size())
        {
            offset -= line.size();
            continue;
        }

        char letter = line[offset];
        // get the ascii value of the letter
        ascii_value = (int)letter;
        // for each letter in the abecedary, only capital letters
        for (char c = 'A'; c <= 'Z'; c++)
        {
            // except for the letters B, X, U, Z, J, O
            if (c == 'B' || c == 'X' || c == 'U' || c == 'Z' || c == 'J' || c == 'O')
                continue;

            // if the ascii value of the letter is the same as the ascii value of the current letter break the loop
            if (ascii_value == (int)c)
                break;

            // if the letter already has a value in the dictionary, continue
            if (protein_dict[c] != '\0')
                continue;

            // add the value to the dictionary
            protein_dict[c] = letter;
            break;
        }

        if (first)
        {
            offset = ascii_value;
            first = false;
        }
    }

    fasta.close();
}

void Encrypt()
{

    // vector of parameter pointers
    vector<Parameters *> parameters;
    // Sets the vector with the parameters
    SetParametersVector(&parameters);
}

int main()
{
    Encrypt();
    return 0;
}
