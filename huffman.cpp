#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

// ================= NODE =================
struct Node {
    int data, freq;
    Node *left, *right;

    Node(int d, int f) {
        data = d;
        freq = f;
        left = right = NULL;
    }
};

// ================= MIN HEAP =================
struct compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// ================= READ CSV =================
vector<int> readCSV(string filename) {
    vector<int> data;
    ifstream file(filename);

    if (!file) {
        cout << "Error: Cannot open CSV file\n";
        return data;
    }

    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string val;

        while (getline(ss, val, ',')) {
            data.push_back(stoi(val));
        }
    }

    return data;
}

// ================= BUILD TREE =================
Node* buildTree(vector<int>& data) {
    unordered_map<int,int> freq;

    for(int v : data)
        freq[v]++;

    priority_queue<Node*, vector<Node*>, compare> pq;

    for(auto p : freq)
        pq.push(new Node(p.first, p.second));

    while(pq.size() > 1) {
        Node* l = pq.top(); pq.pop();
        Node* r = pq.top(); pq.pop();

        Node* n = new Node(-1, l->freq + r->freq);
        n->left = l;
        n->right = r;

        pq.push(n);
    }

    return pq.top();
}

// ================= GENERATE CODES =================
void generateCodes(Node* root, string s, unordered_map<int,string>& code) {
    if(!root) return;

    if(!root->left && !root->right) {
        code[root->data] = s;
    }

    generateCodes(root->left, s + "0", code);
    generateCodes(root->right, s + "1", code);
}

// ================= ENCODE =================
string encode(vector<int>& data, unordered_map<int,string>& code) {
    string res = "";

    for(int v : data)
        res += code[v];

    return res;
}

// ================= DECODE =================
vector<int> decode(string s, Node* root) {
    vector<int> out;
    Node* cur = root;

    for(char c : s) {
        if(c == '0')
            cur = cur->left;
        else
            cur = cur->right;

        if(!cur->left && !cur->right) {
            out.push_back(cur->data);
            cur = root;
        }
    }

    return out;
}

// ================= MAIN =================
int main() {

    cout << "Reading CSV...\n";

    // 🔥 Correct path
    vector<int> pixels = readCSV("static/outputs/pixels.csv");

    if(pixels.empty()) {
        cout << "Error: No data found!\n";
        return 0;
    }

    cout << "Building Huffman Tree...\n";

    Node* root = buildTree(pixels);

    unordered_map<int,string> code;
    generateCodes(root, "", code);

    cout << "Encoding...\n";

    string encoded = encode(pixels, code);

    cout << "Decoding...\n";

    vector<int> decoded = decode(encoded, root);

    // 🔥 SAVE decoded in correct folder
    ofstream out("static/outputs/decoded.txt");

    if (!out) {
        cout << "Error: Cannot write decoded file\n";
        return 0;
    }

    for(int v : decoded)
        out << v << " ";

    out.close();

    cout << "✅ Huffman Done. decoded.txt created!\n";

    return 0;
}