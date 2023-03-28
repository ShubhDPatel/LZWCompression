/*
    Shubh Patel, sdp66@uakron.edu
    Project 2 LZW for algorithms.

    Uses lzw algorithm to compress and decompress a input file.
*/
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector> 
#include <sys/stat.h>

/*
    This code is derived in parts from LZW@RosettaCode for UA CS435 
*/

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(const std::string &uncompressed, Iterator result);

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
std::string decompress(Iterator begin, Iterator end);

// converts into to binary string
std::string int2BinaryString(int c, int cl);

// converts binary string to int
int binaryString2Int(std::string p);

// writes a compressed file from the contents of vector parameter and uses the 2nd parameter to name file
void writeCompressed(std::vector<int>& compressed, std::string fileName);

// puts contents of compressed file into vector of ints and returns it
std::vector<int> getCompressedFile(std::string fileName);

// demo of how LZW works
int main(int argc, char* argv[]) 
{
    if (argc != 3)
    {
        std::cout << "ERROR: Not valid number of argument parameters!\n";
        return 0;
    }
    if (std::string(argv[1]) != "c" && std::string(argv[1]) != "e")
    {
        std::cout << "ERROR: Not valid argument parameter letters!\n";
        return 0;
    }

    std::vector<int> compressed;

    // Compress the file
    if(*argv[1] == 'c')
    {
        // read from file into message string
        std::string fileName = argv[2];

        // make ifstrea object
        std::ifstream readFrom(fileName);

        // put the contents of file into string
        std::cout << "Getting contents of file, this may take a bit of time ...\n";
        std::string message((std::istreambuf_iterator<char>(readFrom)), (std::istreambuf_iterator<char>()));

        // compress contents
        std::cout << "Compressing contents of file, this may take a bit of time ...\n";
        compress(message, std::back_inserter(compressed));

        // get the new name
        fileName = argv[2];
        fileName = fileName.substr(0, fileName.find_last_of("."));
        fileName += ".lzw2";

        // create new file
        std::cout << "Creating a new file with compressed contents, this may take a bit of time ...\n";
        writeCompressed(compressed, fileName);
        readFrom.close();
        std::cout << "Operation finished successfully!\n";
    }
    
    // Decompress the file
    if(*argv[1] == 'e')
    {
        std::cout << "Getting contents inside compressed file, this may take a bit of time ...\n";
        // gets the contents inside compressed file
        compressed = getCompressedFile(argv[2]);

        std::cout << "Decompressing the contents, this may take a bit of time\n";
        // decompress the contents
        std::string message = decompress(compressed.begin(), compressed.end());

        // get new name
        std::string fileName = argv[2];
        fileName = fileName.substr(0, fileName.find_last_of("."));
        fileName += ".2M";

        // write the contents into the file
        std::ofstream writeFile(fileName, std::ios::binary);
        std::cout << "Creating new file with decompressed contents, this may take a bit of time ...\n";
        if (writeFile.is_open())
        {
            writeFile.write(message.data(), message.size());
        }
        else
        {
            std::cout << ".2M file is not open!";
        }
        writeFile.close();
    }

    return 0;
}

// writes a compressed file from the contents of vector parameter and uses the 2nd parameter to name file
void writeCompressed(std::vector<int>& compressed, std::string fileName)
{
    //length of the code
    int bits = 9;

    // binary string and code
    std::string p;
    std::string bcode= "";

    int counter = 0;
    for (std::vector<int>::iterator it = compressed.begin() ; it != compressed.end(); ++it)
    {
        ++counter;
        
        if (counter < 512)
        {
            bits = 10;
        }
        else if (counter < 1024)
        {
            bits = 11;
        }
        else if (counter < 2048)
        {
            bits = 12;
        }
        else if (counter < 4096)
        {
            bits = 13;
        }
        else if (counter < 8192)
        {
            bits = 14;
        }
        else if (counter < 16384)
        {
            bits = 15;
        }
        else if (counter < 32768)
        {
            bits = 16;
        }
        
        p = int2BinaryString(*it, bits);
        //std::cout << "c=" << *it <<" : binary string (12bits) ="<<p<<"; back to code=" << binaryString2Int(p)<<"\n";
        bcode += p;
    }
    
    //writing to file
    //std::cout << "string 2 save : "<<bcode << "\n";
    std::ofstream myfile;
    myfile.open(fileName.c_str(), std::ios::binary);
    
    std::string zeros = "00000000";
    if (bcode.size() % 8 != 0) // make sure the length of the binary string is a multiple of 8
    {
        bcode += zeros.substr(0, 8 - bcode.size() % 8);
    }

    int b;
    for (int i = 0; i < bcode.size(); i +=8)
    {
        b = 1;
        for (int j = 0; j < 8; j++) 
        {
            b = b<<1;
            if (bcode.at(i+j) == '1')
            {
                b+=1;
            }
        }
        char c = (char) (b & 255); //save the string byte by byte
        myfile.write(&c, 1);  
    }
    myfile.close();
}

// puts contents of compressed file into vector of ints and returns it
std::vector<int> getCompressedFile(std::string fileName)
{
    std::ifstream myfile2;
    myfile2.open (fileName.c_str(),  std::ios::binary);
    
    struct stat filestatus;
    stat(fileName.c_str(), &filestatus);
    long fsize = filestatus.st_size; //get the size of the file in bytes
    
    std::string zeros = "00000000";
    int bits = 16;
    char c2[fsize];
    myfile2.read(c2, fsize);
    
    std::string s = "";
    long count = 0;
    while(count < fsize)
    {
        unsigned char uc =  (unsigned char) c2[count];
        std::string p = ""; //a binary string
        for (int j = 0; j < 8 && uc > 0; j++)
        {
            if (uc % 2 == 0)
                p = "0" + p;
            else
                p = "1" + p;
            uc = uc>>1;
        }
        p = zeros.substr(0, 8 - p.size()) + p; //pad 0s to left if needed
        s += p;
        count++;
    } 
    myfile2.close();
    //std::cout << " saved string : "<<s << "\n";

    std::vector<int> content;
    if (s.size() % bits != 0)
    {
        s = std::string(s.data(), (s.size() / bits) * bits);
    }
    
    for (int i = 0; i < s.length(); i += bits)
    {
        content.push_back(binaryString2Int(s.substr(i, bits)));
    }
    return content;
}

// converts int to binary string
std::string int2BinaryString(int c, int cl)
{
    std::string p = ""; //a binary code string with code length = cl
    int code = c;
    while (c > 0) 
    {         
        if (c % 2 == 0)
        p = "0" + p;
        else
        p = "1" + p;
        c = c>>1;
    }
    int zeros = cl - p.size();
    if (zeros < 0)
    {
        std::cout << "\nWarning: Overflow. code " << code <<" is too big to be coded by " << cl <<" bits!\n";
        p = p.substr(p.size()-cl);
    }
    else 
    {
        for (int i=0; i<zeros; i++) //pad 0s to left of the binary code if needed
        {
            p = "0" + p;
        }
    }
    return p;
}

// converts binary string to int
int binaryString2Int(std::string p)
{
    int code = 0;
    if (p.size() > 0) 
    {
        if (p.at(0) == '1')
        {
            code = 1;
        }
        p = p.substr(1);
        while (p.size()>0) 
        { 
            code = code << 1; 
            if (p.at(0) == '1')
            {
                code++;
            }
            p = p.substr(1);
        }
    }
    return code;
}

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(const std::string &uncompressed, Iterator result)
{
    // Build the dictionary, start with 256.
    int dictSize = 256;
    std::map<std::string,int> dictionary;
    for (int i = 0; i < dictSize; i++)
    {
        dictionary[std::string(1, i)] = i;
    }

    std::string w;
    for (std::string::const_iterator it = uncompressed.begin();
        it != uncompressed.end(); ++it)
    {
        char c = *it;
        std::string wc = w + c;
        if (dictionary.count(wc))
        {
            w = wc;
        }
        else 
        {
            *result++ = dictionary[w];
            // 16 bits is 65536
            if (dictionary.size() < 65536)
            {
                dictionary[wc] = dictSize++;
            }
            w = std::string(1, c);
        }
    }

    // Output the code for w.
    if (!w.empty())
    *result++ = dictionary[w];
    return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
std::string decompress(Iterator begin, Iterator end)
{
    // Build the dictionary.
    int dictSize = 65536;
    std::map<int,std::string> dictionary;
    for (int i = 0; i < dictSize; i++)
    {
        dictionary[i] = std::string(1, i);
    }

    std::string w(1, *begin++);
    std::string result = w;
    //std::cout << "\ndecompressed: " << result <<";";
    std::string entry;
    for ( ; begin != end; begin++) 
    {
        int k = *begin;
        if (dictionary.count(k))
        {
            entry = dictionary[k];
        }
        else if (k == dictSize)
        {
            entry = w + w[0];
        }
        else
        {
            throw "Bad compressed k";
        }

        result += entry;
        //std::cout << "\ndecompressed: " << result <<";";

        // Add w+entry[0] to the dictionary. 4096 = 2^12
        if (dictionary.size() < 65536)
        {
            dictionary[dictSize++] = w + entry[0];
        }

        w = entry;
    }
    return result;
}
