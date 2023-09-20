/* C++ program to compress input file using Huffman Coding */

#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

class HuffmanTreeNode {
   public:
    int freq;
    unsigned char data;
    HuffmanTreeNode *left;
    HuffmanTreeNode *right;

    HuffmanTreeNode(char data, int freq) {
        this->data = data;
        this->freq = freq;
        left = right = NULL;
    }

    ~HuffmanTreeNode() {
        delete left;
        delete right;
    }
};

struct cmp {
    bool operator()(HuffmanTreeNode* p1, HuffmanTreeNode* p2) {
        return p1->freq > p2->freq;
    }
};

int binaryToDecimal(string bin) {
    int dec = 0;

    for(int i = bin.size() - 1, base = 1; i >= 0; i--, base *= 2) {
        dec += (bin[i] - '0') * base;
    }

    return dec;
}

string decimalToBinary(int dec) {
    return bitset<8>(dec).to_string();
}

class Huffman {

    string text;
    HuffmanTreeNode* root;
    int numOfBitsInSourceFile;
    int numOfBitsInCompressedFile;
    unordered_map<unsigned char, int> frequencyMap;
    unordered_map<unsigned char, string> huffmanCodes;
    unordered_map<string, unsigned char> invHuffmanCodes;

   public:
    Huffman(string file_name);

    void countFrequencies();

    void buildEncodingTree();

    void buildEncodingMap();

    void buildEncodingMap(HuffmanTreeNode* root, string path);

    void printHuffmanCodes();

    void compress();

    void decompress();

    void printResult();

    ~Huffman();
};

Huffman::Huffman(string file_name) {

    root = NULL;

    numOfBitsInSourceFile = numOfBitsInCompressedFile = 0;

    ifstream in_file;
    in_file.open(file_name);

    if(!in_file.is_open()) {
        cerr << "Input file does not exist" << endl;
        exit(0);
    }else {
        char ch;
        while(in_file.get(ch)) {
            text += ch;
        }
    }

}

void Huffman::countFrequencies() {
    char ch;

    for(int i = 0; i < text.size(); ++i) {
        frequencyMap[text[i]]++;
        numOfBitsInSourceFile++;
    }

    numOfBitsInSourceFile *= 8;
}

void Huffman::buildEncodingTree() {
    priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, cmp> pq;

    unordered_map<unsigned char, int>::iterator it_freqMap;
    for(it_freqMap = frequencyMap.begin(); it_freqMap != frequencyMap.end(); it_freqMap++) {
        HuffmanTreeNode* temp = new HuffmanTreeNode(it_freqMap->first, it_freqMap->second);
        pq.push(temp);
    }

    while(pq.size() != 1) {

        HuffmanTreeNode* leftNode = pq.top(); pq.pop();
        HuffmanTreeNode* rightNode = pq.top(); pq.pop();

        int sum = leftNode->freq + rightNode->freq;
        HuffmanTreeNode* internalNode = new HuffmanTreeNode('\0', sum);
        internalNode->left = leftNode;
        internalNode->right = rightNode;
        pq.push(internalNode);
    }

    this->root = pq.top(); pq.pop();
}

void Huffman::buildEncodingMap(HuffmanTreeNode* node, string path) {
    if(node == NULL) {
        return;
    }

    if(node->left == NULL && node->right == NULL) {
        huffmanCodes[node->data] = path;
        invHuffmanCodes[path] = node->data;
        return;
    }

    buildEncodingMap(node->left, path + "0");
    buildEncodingMap(node->right, path + "1");

    return;
}

void Huffman::buildEncodingMap() {
    buildEncodingMap(root, "");
}

void Huffman::printHuffmanCodes() {

    cout << "\n\nHuffman codes : ";
    cout << "\nCharacter\tFrequency\tHuffman Code\n";

    unordered_map<unsigned char, string>::iterator it_huffmanCode;

    for(it_huffmanCode = huffmanCodes.begin(); it_huffmanCode != huffmanCodes.end(); it_huffmanCode++) {
        cout << it_huffmanCode->first << "\t\t" << frequencyMap[it_huffmanCode->first] << "\t\t" << it_huffmanCode->second << endl;
    }

}

void Huffman::compress() {

    ofstream out_file("zipped.txt", ios::trunc);

    string encode;
    for(int i = 0; i < text.size(); i++) {
        encode += huffmanCodes[text[i]];
    }

    numOfBitsInCompressedFile = encode.size();

    cout << "\n\nEncoded Data : " << encode;

    for(int i = 0; i < encode.length(); i += 8) {

        string temp = encode.substr(i, 8);

        while(temp.size() != 8) {
            temp += '0';
        }

        int ascii = binaryToDecimal(temp);
        char ch = ascii;
        out_file.put(ch);
    }

    out_file.close();

}

void Huffman::decompress() {

    ifstream input("zipped.txt");
    ofstream output("unzipped.txt", ios::trunc);

    string decode;

    unsigned char ch;
    while(input >> ch) {
        int ascii = ch;
        decode += decimalToBinary(ascii);
    }

    decode = decode.substr(0, numOfBitsInCompressedFile);

    string temp;
    for(int i = 0; i < decode.size(); ++i) {
        temp += decode[i];
        if(invHuffmanCodes.find(temp) != invHuffmanCodes.end()) {
            output.put(invHuffmanCodes[temp]);
            temp.clear();
        }
    }

    input.close();
    output.close();
}

void Huffman::printResult() {
    cout << "\n\nSource file size = " << numOfBitsInSourceFile / 8 << " bytes";
    cout << "\n\nCompressed file size = " << numOfBitsInCompressedFile / 8 << " bytes";
    cout << "\n\nCompression percentage = " << ((float)numOfBitsInSourceFile - numOfBitsInCompressedFile) * 100 / numOfBitsInSourceFile<< " %";
}

Huffman::~Huffman() {
    delete root;
}

int main() {
    string file;
    cout << "\nEnter file name : ";
    cin >> file;

    Huffman h(file);

    h.countFrequencies();

    h.buildEncodingTree();

    h.buildEncodingMap();

    h.printHuffmanCodes();

    h.compress();

    h.decompress();

    h.printResult();
}
