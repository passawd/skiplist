#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>

#define STORE_FILE "store/dumpFile"

using namespace std;

mutex mtx; // 互斥锁
string delimiter = ":";


template<typename K, typename V> 
class Node
{
public:
    Node(){}

    Node(K k,V v,int);

    ~Node();
    K get_key() const;

    V get_value() const;

    void set_value(V);

    // 指针数组，存放当前节点右侧不同层数中的节点
    Node<K, V> **forward;

    int node_level;

private:
    K key;
    V value;
};

template<typename K, typename V> 
Node<K,V>::Node(const K k,const V v ,int level)
{
    this->key=k;
    this->value=v;
    this->node_level=level;

    this->forward = new Node<K,V>*[level+1];

    memset(this->forward, 0, sizeof(Node<K, V>*)*(level+1));
}

template<typename K, typename V> 
Node<K, V>::~Node() 
{
    delete []forward;
};

template<typename K, typename V> 
K Node<K, V>::get_key() const 
{
    return key;
};

template<typename K, typename V> 
V Node<K, V>::get_value() const {


    return value;
};
template<typename K, typename V> 
void Node<K, V>::set_value(V value) 
{
    this->value=value;
};

template<typename K, typename V> 
class SkipList
{
public:
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K,V> *creat_node(K,V,int);
    int insert_element(K,V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    void get_key_value_from_string(const string& str, string* key, string* value);
    bool is_valid_string(const string& str);

private:
    //跳表最大层数
    int _max_level;

    //跳表实际层数
    int _skip_list_level;

    //跳表头结点
    Node<K,V> *_header;

    ofstream _file_writer;
    ifstream _file_reader;

    //跳表元素个数
    int _element_count;
};

template<typename K, typename V>
Node<K,V>* SkipList<K, V>::creat_node(const K k,const V v,int level)
{
    Node<K,V> *tmp=new Node<K,V>(k,v,level);
    return tmp;
}

template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value)
{

    mtx.lock();
    Node<K,V> *cur=this->_header;

    Node<K, V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));  

    for(int i=_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i]!=NULL &&cur->forward[i]->get_key() <key )
        {
            cur=cur->forward[i];
        }
        update[i]=cur;
    }

    cur=cur->forward[0];

    if(cur!=NULL &&cur->get_key()==key )
    {
        mtx.unlock();
        return 1;
    }

    if(cur==NULL || cur->get_key()!=key)
    {
        int random_level=get_random_level();
        
        if(random_level>_skip_list_level )
        {
            for(int i=_skip_list_level+1;i<=random_level;i++)
            {
                update[i]=_header;
            }
            _skip_list_level=random_level;
        }

        Node<K,V> *node = creat_node(key,value,random_level);

        for(int i=0;i<=random_level;i++)
        {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i]=node;

        }
        _element_count ++;
    }
    mtx.unlock();
}

template<typename K, typename V> 
void SkipList<K, V>::display_list()
{
    cout << "\n*****Skip List*****"<<"\n"; 
    for(int i=0;i<=_skip_list_level;i++)
    {
        Node<K,V> *node = this->_header->forward[i];
        cout << "Level " << i << ": ";
        while(node!=NULL)
        {
            cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        cout << endl;
    }
}

template<typename K, typename V> 
void SkipList<K, V>::dump_file()
{
    cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    Node<K, V> *node = this->_header->forward[0];  

    while (node != NULL) 
    {
        _file_writer << node->get_key() << ":" << node->get_value() << "\n";
        cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();

}

template<typename K, typename V> 
void SkipList<K, V>::load_file()
{
    _file_reader.open(STORE_FILE);
    cout << "load_file-----------------" << endl;
    string line;
    string* key = new string();
    string* value = new string();
    while (getline(_file_reader, line)) 
    {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) 
        {
            continue;
        }
        insert_element(*key, *value);
        cout << "key:" << *key << "value:" << *value << endl;
    }

    _file_reader.close();
}

template<typename K, typename V> 
int SkipList<K,V>::size()
{
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const string& str, string* key, string* value)
{
    if(!is_valid_string(str) )
    {
        return;
    }
    *key=str.substr(0,str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1,str.length() );

}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const string& str)
{
    if(str.empty())
    {
        return false;
    }

    if(str.find(delimiter) ==string::npos)
    {
        return false;
    }
    return true;
}

template<typename K, typename V> 
void SkipList<K, V>::delete_element(K key)
{
    mtx.lock();
    Node<K, V> *cur = this->_header;
    Node<K,V> *update[_max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    for(int i=_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i]!=NULL &&cur->forward[i]->get_key() <key )
        {
            cur=cur->forward[i];
        }
        update[i]=cur;
    }

    cur=cur->forward[0];
    if (cur != NULL && cur->get_key() == key)
    {
        for(int i=0;i<=_skip_list_level;i++)
        {
            if(update[i]->forward[i]!=cur)
            {
                break;
            }
            update[i]->forward[i]=cur->forward[i];
        }

        while(_skip_list_level >0 && _header->forward[_skip_list_level]==0)
        {
            _skip_list_level--;
        }
        cout << "Successfully deleted key "<< key << endl;
        _element_count--;
    }
    mtx.unlock();

}

template<typename K, typename V>
bool SkipList<K,V>::search_element(K key)
{
    Node<K,V> *cur=_header;
    for(int i=_skip_list_level;i>=0;i-- )
    {
        while(cur->forward[i]  &&cur->forward[i]->get_key()<key )
        {
            cur=cur->forward[i];
        }
    }

    cur=cur->forward[0];


    if(cur!=NULL && cur->get_key()==key)
    {
        return true;
    }
    return false;
}

template<typename K, typename V> 
SkipList<K, V>::SkipList(int max_level)
{

    this->_max_level=max_level;
     this->_skip_list_level = 0;
    this->_element_count = 0;


    // 创建头结点
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);

}

template<typename K, typename V> 
SkipList<K, V>::~SkipList() 
{

    if (_file_writer.is_open()) 
        _file_writer.close();

    if (_file_reader.is_open()) 
        _file_reader.close();
    
    delete _header;
}


template<typename K, typename V>
int SkipList<K, V>::get_random_level()
{

    int k = 1;
    while (rand() % 2) 
        k++;


    k = (k < _max_level) ? k : _max_level;
    return k;
}


