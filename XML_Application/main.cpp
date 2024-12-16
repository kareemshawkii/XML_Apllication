#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stack>
#include <sstream>
#include <algorithm>
#include <regex>
using namespace std;
//sameh
string readFile(const string& filePath) {
    ifstream file(filePath);
    if (!file) {
        cerr << "Error opening file!" << endl;
        return "";
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}
void writeFile(const string& filePath, const string& content) {
    ofstream file(filePath);
    if (!file) {
        cerr << "Error: Cannot open file " << filePath << endl;
        return;
    }
    file << content;
}
struct Tag {
    std::string name;
    int line;
};
bool isXMLBalanced(ifstream& file,ostringstream& log) {
    stack<Tag> tagStack;
    regex openTagPattern(R"(<([a-zA-Z0-9]+)>)");
    regex closeTagPattern(R"(</([a-zA-Z0-9]+)>)");
    string line;
    int lineNumber = 0;
    bool isBalanced = true;

    while (getline(file, line)) {
        lineNumber++;
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.find("<!--") == 0 || line.find("<?") == 0)
            continue;
        smatch match;
        while (regex_search(line, match, openTagPattern)) {
            tagStack.push({match[1].str(), lineNumber});
            line = match.suffix();
        }
        while (regex_search(line, match, closeTagPattern)) {
            if (tagStack.empty()) {
                log << "Error: Closing tag </" << match[1].str()
                << "> on line " << lineNumber << " has no matching opening tag.\n";
                isBalanced = false;
            } else {
                Tag lastOpenTag = tagStack.top();
                tagStack.pop();
                if (lastOpenTag.name != match[1].str()) {
                    log << "Error: Mismatched tag on line " << lineNumber
                        << ". Expected </" << lastOpenTag.name << "> but found </" << match[1].str() << ">.\n";
                    isBalanced = false;
                }
            }
            line = match.suffix();
        }
    }

    while (!tagStack.empty()) {
        Tag unmatchedTag = tagStack.top();
        tagStack.pop();
        log << "Error: Opening tag <" << unmatchedTag.name
            << "> on line " << unmatchedTag.line << " has no matching closing tag.\n";
        isBalanced = false;
    }

    return isBalanced;
}
//ehab
string prettifyXMLFun(const string& xmlContent) {
    stringstream result;
    stack<string> tagStack;
    int indentLevel = 0;
    const string indent = "    ";
    bool previousTag = false;
    size_t i = 0;
    while (i < xmlContent.size()) {
        if (xmlContent[i] == '<') {
            size_t endPos = xmlContent.find('>', i);
            if (endPos == string::npos) {
                cerr << "Error: Malformed XML, missing '>'." << endl;
                break;
            }
            string tag = xmlContent.substr(i, endPos - i + 1);
            if (tag[1] == '/') {
                indentLevel--;
                result << string(indentLevel * indent.size(), ' ') << tag << "\n";
                if (!tagStack.empty() && tag.substr(2, tag.size() - 3) == tagStack.top()) {
                    tagStack.pop();
                }
                previousTag = true;
            } else {
                if (previousTag) {
                    result << "\n";
                }
                result << string(indentLevel * indent.size(), ' ') << tag << "\n";
                tagStack.push(tag.substr(1, tag.size() - 2));
                indentLevel++;
                previousTag = true;
            }
            i = endPos + 1;
        } else {
            size_t endPos = xmlContent.find('<', i);
            string content = xmlContent.substr(i, endPos - i);
            content.erase(remove(content.begin(), content.end(), '\n'), content.end());
            if (!content.empty()) {
                result << string(indentLevel * indent.size(), ' ') << content << "\n";
            }
            i = endPos;
            previousTag = false;
        }
    }

    if (!tagStack.empty()) {
        cerr << "Error: Mismatched tags in XML file." << endl;
    }

    return result.str();
}
void prettifyXML(const string& xmlContent, const string& filename) {
    string prettifiedXML = prettifyXMLFun(xmlContent);
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening the file for saving!" << endl;
        return;
    }
    outputFile << prettifiedXML;
    outputFile.close();
    cout << "Prettified XML saved successfully to " << filename << "." << endl;
}
//kareem
struct Element {
    string tag;
    string value;
    vector<Element> children;
};
void toJSON(const Element& element, string& json, int indent = 0) {
    string indentation(indent, ' ');
    json += indentation + "\"" + element.tag + "\": ";
    if (element.children.empty()) {
        json += "\"" + element.value + "\"";
    }
    else {
        bool isArray = element.tag == "users" || element.tag == "posts" || element.tag == "followers"|| element.tag == "topics";
        json += isArray ? "[\n" : "{\n";
        for (size_t i = 0; i < element.children.size(); ++i) {
            toJSON(element.children[i], json, indent + 2);
            if (i < element.children.size() - 1) {
                json += ",";
            }
            json += "\n";
        }
        json += indentation + (isArray ? "]" : "}");
    }
}
void printToJSONFile(const Element& root, const string& filename) {
    string json;
    json += "{\n";
    toJSON(root, json, 2);
    json += "\n}";
    ofstream file(filename);
    if (file.is_open()) {
        file << json;
        file.close();
    } else {
        cerr << "Could not open file for writing.\n";
    }
    cout<<"XML successfully converted to Json file and saved to: "<<filename<<".json"<<endl;
}
//youssef
string processText(const string& s) {
    string processedText = "";
    bool insideTag = false;
    for (char c : s) {
        if (c == '<') {
            insideTag = true;
            processedText += c;
        } else if (c == '>') {
            insideTag = false;
            processedText += c;
        } else if (!insideTag && (c != ' ' && c != '\n' && c != '\t')) {
            processedText += c;
        } else if (insideTag) {
            processedText += c;
        }
    }

    return processedText;
}
void minifyXML(const string& inputPath, const string& outputPath) {
    ifstream inputFile(inputPath);
    if (!inputFile.is_open()) {
        cout << "Error opening the input file!" << endl;
        return;
    }
    stringstream buffer;
    buffer << inputFile.rdbuf();
    string xmlContent = buffer.str();
    string processedXML = processText(xmlContent);
    ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        cout << "Error opening the output file!" << endl;
        return;
    }
    outputFile << processedXML;
    outputFile.close();
    cout << "Processed XML saved to " << outputPath << " successfully." << endl;
}
//mohamed
struct Pair {
    string token;
    string pattern;
};
void buildDictionary(const string& content, vector<Pair>& dictionary) {
    size_t pos = 0;
    int tokenIndex = 1;
    while ((pos = content.find('<', pos)) != string::npos) {
        size_t end = content.find('>', pos);
        if (end != string::npos) {
            string tag = content.substr(pos, end - pos + 1);
            bool exists = false;
            for (const auto& entry : dictionary) {
                if (entry.pattern == tag) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                dictionary.push_back({ "T" + to_string(tokenIndex++), tag });
            }
            pos = end + 1;
        } else {
            break;
        }
    }
}
string replacePatternsWithTokens(const string& content, const vector<Pair>& dictionary) {
    string compressedContent = content;
    for (const auto& entry : dictionary) {
        size_t pos = 0;
        while ((pos = compressedContent.find(entry.pattern, pos)) != string::npos) {
            compressedContent.replace(pos, entry.pattern.length(), entry.token);
            pos += entry.token.length(); // Move past the replaced token
        }
    }

    return compressedContent;
}
void writeCompressedFile(const string& outputFile, const vector<Pair>& dictionary, const string& compressedContent) {
    ofstream output(outputFile, ios::out);
    if (!output.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return;
    }
    for (const auto& entry : dictionary) {
        output << entry.token << "->" << entry.pattern << "\n";
    }
    output << "---\n";
    output << compressedContent;
    output.close();
    cout << "Compression completed. Output saved to: " << outputFile << endl;
}
void compressXML(const string& inputFile, const string& outputFile) {
    ifstream input(inputFile, ios::in);
    if (!input.is_open()) {
        cerr << "Error: Unable to open input file." << endl;
        return;
    }
    string content, line;
    while (getline(input, line)) {
        content += line + "\n";
    }
    input.close();
    vector<Pair> dictionary;
    buildDictionary(content, dictionary);
    string compressedContent = replacePatternsWithTokens(content, dictionary);
    writeCompressedFile(outputFile, dictionary, compressedContent);
}
//hussam
void parseDictionary(ifstream& input, vector<Pair>& dictionary) {
    string line;
    while (getline(input, line)) {
        if (line == "---") break;
        size_t arrowPos = line.find("->");
        if (arrowPos != string::npos) {
            string token = line.substr(0, arrowPos);
            string pattern = line.substr(arrowPos + 2);
            dictionary.push_back({ token, pattern });
        }
    }
}
string replaceTokensWithPatterns(const string& content, const vector<Pair>& dictionary) {
    string decompressedContent = content;
    for (const auto& entry : dictionary) {
        size_t pos = 0;
        while ((pos = decompressedContent.find(entry.token, pos)) != string::npos) {
            decompressedContent.replace(pos, entry.token.length(), entry.pattern);
            pos += entry.pattern.length(); // Move past the replaced pattern
        }
    }

    return decompressedContent;
}
void decompressFile(const string& inputFile, const string& outputFile) {
    ifstream input(inputFile, ios::in);
    if (!input.is_open()) {
        cerr << "Error: Unable to open input file." << endl;
        return;
    }
    ofstream output(outputFile, ios::out);
    if (!output.is_open()) {
        cerr << "Error: Unable to open output file." << endl;
        return;
    }
    vector<Pair> dictionary;
    parseDictionary(input, dictionary);
    string line, compressedContent;
    while (getline(input, line)) {
        compressedContent += line + "\n";
    }
    string decompressedContent = replaceTokensWithPatterns(compressedContent, dictionary);
    output << decompressedContent;
    input.close();
    output.close();
    cout << "Decompression completed. Output saved to: " << outputFile << endl;
}
//kareem&Ehab
const int NULL_EDGE = 0;
template<class VertexType>class Graph {
private:
    int numVertices;
    int maxVertices;
    VertexType* vertices;
    int** edges;

    int IndexIs(VertexType* vertices, VertexType vertex) const;

public:
    Graph(int maxV);
    ~Graph();
    void AddVertex(VertexType vertex);
    void AddEdge(VertexType fromVertex, VertexType toVertex);
    void VisualizeGraph(const std::string& filename) const;
    void ConvertDotToJPG(const std::string& dotFile, const std::string& jpegFile) const;
};
template<class VertexType>Graph<VertexType>::Graph(int maxV) {
    numVertices = 0;
    maxVertices = maxV;
    vertices = new VertexType[maxV];
    edges = new int*[maxV];
    for (int i = 0; i < maxV; i++) {
        edges[i] = new int[maxV];
        for (int j = 0; j < maxV; j++) {
            edges[i][j] = NULL_EDGE;
        }
    }
}
template<class VertexType>Graph<VertexType>::~Graph() {
    delete[] vertices;
    for (int i = 0; i < maxVertices; i++) {
        delete[] edges[i];
    }
    delete[] edges;
}
template<class VertexType>void Graph<VertexType>::AddVertex(VertexType vertex) {
    vertices[numVertices] = vertex;
    for (int index = 0; index <= numVertices; index++) {
        edges[numVertices][index] = NULL_EDGE;
        edges[index][numVertices] = NULL_EDGE;
    }
    numVertices++;
}
template<class VertexType>void Graph<VertexType>::AddEdge(VertexType fromVertex, VertexType toVertex) {
    int row = IndexIs(vertices, fromVertex);
    int col = IndexIs(vertices, toVertex);
    if (row != -1 && col != -1) {
        edges[row][col] = 1;
    }
}
template<class VertexType>int Graph<VertexType>::IndexIs(VertexType* vertices, VertexType vertex) const {
    for (int i = 0; i < numVertices; i++) {
        if (vertices[i] == vertex) {
            return i;
        }
    }
    return -1;
}
template<class VertexType>void Graph<VertexType>::VisualizeGraph(const std::string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file for writing." << endl;
        return;
    }
    file << "digraph G {" << endl;
    for (int i = 0; i < numVertices; i++) {
        file << "  \"" << vertices[i] << "\";" << endl;
    }
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            if (edges[i][j] != NULL_EDGE) {
                file << "  \"" << vertices[i] << "\" -> \"" << vertices[j] << "\";" << endl;
            }
        }
    }
    file << "}" << endl;
    file.close();
    cout << "Graph written to " << filename << " in DOT format." << endl;
}
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}
Element parseXML(const string& xml) {
    Element root;
    stringstream ss(xml);
    string line;
    vector<Element*> stack;
    while (getline(ss, line)) {
        bool flag=false;
        size_t start = line.find('<') + 1;
        size_t end = line.find('>');
        if (start == string::npos || end == string::npos) {
            stack.back()->value=trim(line);
            continue;
        }
        string tag = line.substr(start, end - start);
        if (tag[0] == '/') {
            stack.pop_back();
        } else {
            Element element;
            element.tag = tag;
            start = line.find('>') + 1;
            end = line.find('<', start);
            if (end != string::npos) {
                flag=true;
                element.value = line.substr(start, end - start);
            }

            if (stack.empty()) {
                root = element;
                stack.push_back(&root);
            } else {
                stack.back()->children.push_back(element);
                if(!flag)stack.push_back(&stack.back()->children.back());
            }
        }
    }
    return root;
}
void generateGraphFromMap(const map<string, vector<string>>& userFollowersMap, Graph<string>& graph) {
    for (const auto& userEntry : userFollowersMap) {
        const string& userId = userEntry.first;
        graph.AddVertex(userId);
    }
    for (const auto& userEntry : userFollowersMap) {
        const string& userId = userEntry.first;
        const vector<string>& followers = userEntry.second;

        for (const string& follower : followers) {
            graph.AddEdge(follower, userId);
        }
    }
}
void toMap(const Element& element, map<string, vector<string>>& userFollowersMap) {
    if (element.tag == "user") {
        string userId;
        vector<string> followers;
        for (const auto& child : element.children) {
            if (child.tag == "id") {
                userId = child.value;
            } else if (child.tag == "followers") {
                for (const auto& follower : child.children) {
                    if (follower.tag == "follower") {
                        for (const auto& followerChild : follower.children) {
                            if (followerChild.tag == "id") {
                                followers.push_back(followerChild.value);
                            }
                        }
                    }
                }
            }
        }
        if (!userId.empty()) {
            userFollowersMap[userId] = followers;
            cout << endl;
        }
    }
    for (const auto& child : element.children) {
        toMap(child, userFollowersMap);
    }
}
template<class VertexType>void Graph<VertexType>::ConvertDotToJPG(const std::string& dotFile, const std::string& jpegFile) const {
    string command = "dot -Tjpeg " + dotFile + " -o " + jpegFile;
    int result = std::system(command.c_str());
    if (result != 0) {
        cerr << "Error: Failed to convert .dot to .jpeg. Ensure Graphviz is installed and the input file is valid." << endl;
    } else {
        cout << "Successfully converted " << dotFile << " to " << jpegFile << endl;
    }
}
//mohamed&hussam&youssef


int main() {
    // 1. Read XML content from file
    string filename;
    cout << "Enter the path to the XML file: ";//C:\Users\kimok\Downloads\sample.xml
    cin >> filename;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    string xmlContent = buffer.str();
    file.close();

    // 2. Parse XML
    Element root = parseXML(xmlContent);
    cout << "XML Parsed Successfully." << endl;

    // 3. Convert XML to JSON
    string json;
    toJSON(root, json);
    printToJSONFile(root,"toJson");

    // 4. Minify XML
    string minifiedPath = "minified.xml";
    minifyXML(filename, minifiedPath);

    // 5. Compress XML
    string compressedPath = "compressed.xml";
    compressXML(filename, compressedPath);

    // 6. Decompress XML
    string decompressedPath = "decompressed.xml";
    decompressFile(compressedPath, decompressedPath);

    // 7. Prettify XML
    prettifyXML(xmlContent,"Prettified.xml");

    // 8. Generate user-followers map
    map<string, vector<string>> userFollowersMap;
    toMap(root, userFollowersMap);
    cout << "User-Followers Map Generated." << endl;

    // 9. Create a graph from the map
    Graph<string> graph(100);
    generateGraphFromMap(userFollowersMap, graph);

    // 10. Visualize the graph
    string dotFile = "graph.dot";
    string jpegFile = "graph.jpg";
    graph.VisualizeGraph(dotFile);
    graph.ConvertDotToJPG(dotFile, jpegFile);

    //11

    //12

    //13

    //14

    //15

    //16

    return 0;
}

