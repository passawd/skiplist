#include <iostream>
#include "skiplist.h"
#define FILE_PATH "./store/dumpFile"

using namespace std;

int main()
{

    SkipList<int, std::string> skipList(3);
	skipList.display_list();
	skipList.insert_element(1, "数据1"); 
	skipList.display_list();
	skipList.insert_element(3, "数据2"); 
	skipList.insert_element(7, "数据3"); 
	skipList.insert_element(8, "数据4"); 
	skipList.insert_element(9, "数据6"); 
	skipList.insert_element(19, "数据7"); 
	skipList.insert_element(19, "数据8"); 

    cout << "skipList size:" << skipList.size() << endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search_element(9);
    skipList.search_element(18);


    skipList.display_list();

    skipList.delete_element(3);
    skipList.delete_element(7);

    cout << "skipList size:" << skipList.size() << endl;

    skipList.display_list();








    return 0;


}