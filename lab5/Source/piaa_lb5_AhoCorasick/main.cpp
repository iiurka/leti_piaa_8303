#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <unordered_map>
#define DEBUG
// для индивидуализации необходимо объявить макрос SKIP_INTERSECTIONS
#define SKIP_INTERSECTIONS

using namespace std;

class TreeNode {
public:
    explicit TreeNode(char val) : value(val) {}

#ifdef DEBUG

    void printTrie() {
        cout << "Current state of trie:" << endl;

        queue<TreeNode *> queue;
        queue.push(this);

        while (!queue.empty()) {
            auto curr = queue.front();
            if (!curr->value)
                cout << "Root:" << endl;
            else
                cout << curr->dbgStr << ':' << endl;
            if (curr->suffixLink)
                cout << "\tSuffix Link: " << (curr->suffixLink == this ? "Root" : curr->suffixLink->dbgStr) << endl;
            if (curr->parent && curr->parent->value)
                cout << "\tParent: " << curr->parent->dbgStr << endl;
            else if (curr->parent)
                cout << "\tParent: Root" << endl;

            if (!curr->children.empty())
                cout << "\tChildren: ";
            for (auto child : curr->children) {
                cout << child.second->value << ' ';
                queue.push(child.second);
            }
            queue.pop();
            cout << endl;
        }
        cout << endl;

    }

#endif

    void insert(const string &str) {
        auto curr = this;
        static size_t countPatterns = 0;
        //пробегаемся по строке
        for (char c : str) {
            //если из текущей вершины по текущему символу не было создано перехода
            if (curr->children.find(c) == curr->children.end()) {
                //создаём переход по символу
                curr->children[c] = new TreeNode(c);
                curr->children[c]->parent = curr;
#ifdef DEBUG
                curr->children[c]->dbgStr += curr->dbgStr + c;
#endif
            }
            //двигаемся "вниз" по дереву
            curr = curr->children[c];
        }
#ifdef DEBUG
        cout << "Inserting string: " << str << endl;
        printTrie();
#endif
        // маркер терминальной вершины, значение которого равно порядковому номеру добавления шаблона
        curr->numOfPattern = ++countPatterns;
    }

    vector<size_t> find(const char c) {
        // статическая переменная для хранения вершины, с которой необходимо начать следующий вызов
        static const TreeNode *curr = this;
#ifdef DEBUG
        cout << "Find '" << c << "' from: " << (curr->dbgStr.empty() ? "Root" : curr->dbgStr) << endl;
#endif

        for (; curr != nullptr; curr = curr->suffixLink) {
            // обходим потомков, если искомого символа среди потомков найдено не будет, то переходим по суффиксной ссылке, для дальнейшего поиска
            for (auto child : curr->children)
                // если символ потомка равен искомому
                if (child.first == c) {
                    // значение текущей вершины переносим на этого потомка
                    curr = child.second;
                    // вектор номеров найденных терминальных вершин
                    vector<size_t> found;
#ifdef SKIP_INTERSECTIONS
                    // для пропуска пересечений, после нахождения терминальной вершины
                    if (curr->numOfPattern) {
                        // добавляем к найденным эту вершину
                        found.push_back(curr->numOfPattern - 1);
                        // и переходим в корень
                        curr = this;
                    }
#else
                    // обходим суффиксы, т.к. они тоже могут быть терминальными вершинами
                    for (auto temp = curr; temp->suffixLink; temp = temp->suffixLink)
                        if (temp->numOfPattern)
                            found.push_back(temp->numOfPattern - 1);
#endif
#ifdef DEBUG
                    cout << "Symbol '" << c << "' found" << endl;
#endif
                    return found;
                }
#ifdef DEBUG
            if (curr->suffixLink)
                cout << "Go to suffix link: " << (curr->suffixLink->dbgStr.empty() ? "Root" : curr->suffixLink->dbgStr)
                     << endl;
#endif
        }
#ifdef DEBUG
        cout << "Symbol '" << c << "' not found" << endl;
#endif
        curr = this;
        return {};
    }

    void makeAutomaton() {
#ifdef DEBUG
        cout << "Automaton building: " << endl;
#endif
        // очередь для обхода в ширину
        queue<TreeNode *> queue;
        // закидываем потомков корня
        for (auto child : children)
            queue.push(child.second);

        while (!queue.empty()) {
            // обрабатываем верхушку очереди
            auto curr = queue.front();
#ifdef DEBUG
            cout << curr->dbgStr << ':' << endl;
            if (curr->parent && curr->parent->value) {
                cout << "\tParent: " << curr->parent->dbgStr << endl;
            } else if (curr->parent) {
                cout << "\tParent: Root" << endl;
            }

            if (!curr->children.empty()) {
                cout << "\tChildren: ";
            }
#endif
            // закидываем потомков текущей верхушки
            for (auto child : curr->children) {
#ifdef DEBUG
                cout << child.second->value << ' ';
#endif
                queue.push(child.second);
            }

#ifdef DEBUG
            if (!curr->children.empty())
                cout << endl;
#endif

            queue.pop();
            // ссылка на родителя обрабатываемой вершины
            auto p = curr->parent;
            // значение обрабатываемой вершины
            char x = curr->value;
            // если родитель не nullptr, то переходим по суффиксной ссылке
            if (p) p = p->suffixLink;

            // пока можно переходить по суфф ссылке или же пока не найдем переход в символ обрабатываемой вершины
            while (p && p->children.find(x) == p->children.end()) {
                p = p->suffixLink;
            }

            // суффиксная ссылка для текущей вершины равна корню, если не смогли найти переход в дереве по символу тек вершины
            // иначе найденной вершине
            curr->suffixLink = p ? p->children[x] : this;
#ifdef DEBUG
            cout << "\tSuffix Link: " << (curr->suffixLink == this ? "Root" : curr->suffixLink->dbgStr) << endl << endl;
#endif
        }
#ifdef DEBUG
        cout << endl;
        printTrie();
#endif
    }

    ~TreeNode()
    {
        for (auto child : children)
            delete child.second;
    }

private:
#ifdef DEBUG
    string dbgStr = "";
#endif
    char value;
    size_t numOfPattern = 0;
    TreeNode *parent = nullptr;
    TreeNode *suffixLink = nullptr;
    unordered_map <char, TreeNode*> children;
};

class Trie {
public:
    Trie() : root('\0') {}

    void insert(const string &str)
    {
        root.insert(str);
    }

    auto find(const char c)
    {
        return root.find(c);
    }

    void makeAutomaton()
    {
        root.makeAutomaton();
    }

private:
    TreeNode root;
};

auto AhoCorasick(const string &text, const vector <string> &patterns)
{
    Trie bor;
    set <pair<size_t, size_t>> result;

    // закидываем паттерны в бор
    for (const auto &pattern : patterns)
        bor.insert(pattern);
    //делаем автомат из полученного дерева, путём добавления суффиксных ссылок
    bor.makeAutomaton();

    for (size_t j = 0; j < text.size(); j++)
        for (auto pos : bor.find(text[j]))
            result.emplace(j - patterns[pos].size() + 2, pos + 1);

    return result;
}

int main()
{
    string text;
    size_t n;
    cin >> text >> n;
    vector <string> patterns(n);

    for (size_t i = 0; i < n; i++)
        cin >> patterns[i];

    for (auto ans : AhoCorasick(text, patterns))
        cout << ans.first << ' ' << ans.second << endl;

    return 0;
}
