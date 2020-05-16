#include <iostream>
#include <string>
#include <vector>
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
    void printTrie()
    {
        cout << "Current state of trie:" << endl;

        queue <TreeNode*> queue;
        queue.push(this);

        while (!queue.empty())
        {
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
                cout << "\tParent: Root"<< endl;

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
    void insert(const string &str, size_t pos, size_t size)
    {
        auto curr = this;
        for (char c : str)
        {
            //если из текущей вершины по текущему символу не было создано перехода
            if (curr->children.find(c) == curr->children.end())
            {
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

        curr->substringEntries.emplace_back(pos, size);
    }

    vector <pair<size_t, size_t>> find(const char c)
    {
        // статическая переменная для хранения вершины, с которой необходимо начать следующий вызов
        static const TreeNode* curr = this;
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
                    // вектор пар, состоящих из начала безмасочной подстроки в маске и её длины
                    vector <pair<size_t, size_t>> found;
                    // обходим суффиксы, т.к. они тоже могут быть терминальными вершинами
                    for (auto temp = curr; temp->suffixLink; temp = temp->suffixLink)
                        for (auto el : temp->substringEntries)
                            found.push_back(el);
#ifdef DEBUG
                    cout << "Symbol '" << c << "' found" << endl;
#endif
                    return found;
                }

#ifdef DEBUG
            if (curr->suffixLink)
                cout << "Go to suffix link: " << (curr->suffixLink->dbgStr.empty() ? "Root" : curr->suffixLink->dbgStr) << endl;
#endif
        }
#ifdef DEBUG
        cout << "Symbol '" << c << "' not found" << endl;
#endif
        curr = this;
        return {};
    }

    void makeAutomaton()
    {
#ifdef DEBUG
        cout << "Automaton building: " << endl;
#endif
        // очередь для обхода в ширину
        queue <TreeNode*> queue;
        // закидываем потомков корня
        for (auto child : children)
            queue.push(child.second);

        while (!queue.empty())
        {
            // обрабатываем верхушку очереди
            auto curr = queue.front();
#ifdef DEBUG
            cout << curr->dbgStr << ':' << endl;
            if (curr->parent && curr->parent->value) {
                cout << "\tParent: " << curr->parent->dbgStr << endl;
            }
            else if (curr->parent) {
                cout << "\tParent: Root"<< endl;
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

private:
#ifdef DEBUG
    string dbgStr = "";
#endif
    char value;
    TreeNode *parent = nullptr;
    TreeNode *suffixLink = nullptr;
    vector <pair<size_t, size_t>> substringEntries;
    unordered_map <char, TreeNode*> children;
};

class Trie {
public:
    Trie() : root('\0') {}

    void insert(const string &str, size_t pos, size_t size)
    {
        root.insert(str, pos, size);
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

auto AhoCorasick(const string &text, const string &mask, char joker)
{
    Trie bor;
    vector <size_t> result;
    // массив для хранения кол-ва попаданий безмасочных подстрок в текст
    vector <size_t> midArr(text.size());
    string pattern;
    // кол-во безмасочных подстрок
    size_t numSubstrs = 0;

    // закидываем в бор все безмасочные подстроки маски
    for (size_t i = 0; i <= mask.size(); i++)
    {
        char c = (i == mask.size()) ? joker : mask[i];
        if (c != joker) {
            pattern += c;
        } else if (!pattern.empty()) {
            numSubstrs++;
            bor.insert(pattern, i - pattern.size(), pattern.size());
            pattern.clear();
        }
    }
    bor.makeAutomaton();

    for (size_t j = 0; j < text.size(); j++)
        for (auto pos : bor.find(text[j]))
        {
            // на найденной терминальной вершине вычисляем индекс начала маски в тексте
            int i = int(j) - int(pos.first) - int(pos.second) + 1;
            if (i >= 0 && i + mask.size() <= text.size())
                // и увеличиваем её значение на 1
                midArr[i]++;
        }

    for (size_t i = 0; i < midArr.size(); i++)
        // индекс, по которым промежуточный массив хранит кол-во
        // попаданий безмасочных подстрок в текст, есть индекс начала вхождения маски
        // в текст, при условии, что кол-во попаданий равно кол-ву подстрок б/м
        if (midArr[i] == numSubstrs)
        {
            result.push_back(i + 1);
#ifdef SKIP_INTERSECTIONS
            // для пропуска пересечений, после найденного индекса,
            // увеличиваем его на длину маски
            i += mask.size() - 1;
#endif
        }

    return result;
}

int main()
{
    string text, mask;
    char joker;
    cin >> text >> mask >> joker;

    for (auto ans : AhoCorasick(text, mask, joker))
        cout << ans << endl;

    return 0;
}
