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
        this->tries = tries;
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
            text = line;
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

    unordered_map<char, bool> used;
    unordered_map<char, char> protein_dict = {{'A', '\0'}, {'C', '\0'}, {'D', '\0'}, {'E', '\0'}, {'F', '\0'}, {'G', '\0'}, {'H', '\0'}, {'I', '\0'}, {'K', '\0'}, {'L', '\0'}, {'M', '\0'}, {'N', '\0'}, {'P', '\0'}, {'Q', '\0'}, {'R', '\0'}, {'S', '\0'}, {'T', '\0'}, {'V', '\0'}, {'W', '\0'}, {'Y', '\0'}, {'B', 'Z'}, {'Z', 'B'}, {'X', 'J'}, {'J', 'X'}, {'U', 'O'}, {'O', 'U'}};
    string content, line;

    unsigned int i = 0;

    while (getline(fasta, line))
        content += line;

    content = content.substr(p->offset);

    char letter = content[0];
    unsigned int ascii = (int)letter;

    // for each capital letter
    for (int i = 0; i < content.size(); i += ascii)
    {
        letter = content[i];

        // skip spaces
        if (letter == ' ' || letter == '\n' || letter == '\r')
            continue;

        // or a special character
        if (letter < 'A' || letter > 'Z')
            continue;

        for (char c = 'A'; c <= 'Z'; c++)
        {
            if (protein_dict[c] != '\0')
                continue;

            // safely check if the letter is already used, if it does not exists in the map
            if (used[letter])
                continue;

            protein_dict[c] = letter;
            used[letter] = true;
            break;
        }
    }

    // print the dictionary
    for (auto it = protein_dict.begin(); it != protein_dict.end(); it++)
        cout << it->first << " -> " << it->second << endl;

    // replace the text with the vocabulary
    for (int i = 0; i < p->text.size(); i++)
    {
        switch (p->text[i])
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
            cout << protein_dict[p->text[i]];
            break;
        }
    }

    cout << endl;

    fasta.close();
}

void Encrypt()
{

    // vector of parameter pointers
    vector<Parameters *> parameters;
    // Sets the vector with the parameters
    SetParametersVector(&parameters);
    // Create the vocabulary
    CreateVocabulary(parameters[0]);
}

int main()
{
    Encrypt();
    return 0;
}
