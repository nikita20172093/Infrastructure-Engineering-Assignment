#include <bits/stdc++.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;


map<string, vector<string> > mymap,
    empMymap; // this map contains folder name(as per mimeType) and their files
map<string, string> undoMap, empUndo;
map<string, string>::iterator mit;
map<string, string> dirNameMapping,
    empDirName; // if folder already exist they this map is used to map a mimeType with folder name


////*this function classify mime type of a file
// this functon takes extension as input and returns the category of particular file
string getKey(string mimeType)
{
    transform(mimeType.begin(), mimeType.end(), mimeType.begin(), ::tolower);
    if (mimeType == "pdf")
        return "pdf";

    if (mimeType == "jpg" || mimeType == "jpeg" || mimeType == "png" || mimeType == "gif"
        || mimeType == "webp" || mimeType == "bmp")
    {
        return "image";
    }

    if (mimeType == "mp4" || mimeType == "mp3" || mimeType == "flv" || mimeType == "m3u8"
        || mimeType == ".ts" || mimeType == "wmv" || mimeType == "avi" || mimeType == "mov"
        || mimeType == "3gp" || mimeType == "mkv")
        return "video";

    if (mimeType == "c" || mimeType == "c++" || mimeType == "h" || mimeType == "cpp"
        || mimeType == "sh" || mimeType == "py" || mimeType == "java" || mimeType == "class")
        return "prog";
    if (mimeType == "doc" || mimeType == "txt" || mimeType == "docx" || mimeType == "ppt"
        || mimeType == "odt" || mimeType == "xls" || mimeType == "html" || mimeType == "css")
        return "doc";

    return "other";
}


///////* listDir()
// this function takes root as input and keep file in appropriate folder
void listDir(string directory, string rootPath)
{
    DIR* dp;
    if(directory[directory.length()-1]!='/')
        directory=directory+"/";
    struct dirent* dirp;
    string dir = rootPath+"Documents/";
    if ((dp = opendir(directory.c_str())) == NULL)
    {
        // cout << "Error(" << errno << ") opening " << dir << endl;
        return;
    }

    vector<string> fileType, fileMap;

    ////////////mazor folder types that i assumed to take into consideration
    mymap.insert(make_pair("pdf", fileType));
    mymap.insert(make_pair("image", fileType));
    mymap.insert(make_pair("video", fileType));
    mymap.insert(make_pair("doc", fileType));
    mymap.insert(make_pair("prog", fileType));
    mymap.insert(make_pair("other", fileType));
    mymap.insert(make_pair("folder", fileType));

    fileMap.push_back("pdf");
    fileMap.push_back("image");
    fileMap.push_back("video");
    fileMap.push_back("doc");
    fileMap.push_back("prog");
    fileMap.push_back("other");
    fileMap.push_back("folder");


    /// This while loop says the files and their category
    // basically I used map to keep all files into map(data structure)
    while ((dirp = readdir(dp)) != NULL)
    {
        string name(dirp->d_name);
        if (name != "." && name != "..")
        {
            string test = directory + name;
            if (dirp->d_type == DT_DIR)
            {
                /// ignored all the folder that are present in desktop as they may be present
                // there for a specific purpose(assumption)
                string folder = directory + name; /// if folder needs to be moved***code here
            }
            else
            {
                int indexOfDot = test.find_last_of(".");
                string mimeType = test.substr(indexOfDot + 1);
                mimeType = mimeType.substr(0, mimeType.length());
                string key = getKey(mimeType);
                test = test.substr(0, test.length());
                mymap[key].push_back(test);
            }
        }
    }


    ////////////////////////
    // This loop is to select a folder name because there may be possibility that the folder
    // name we have chosen is already in existence.
    // In this case we need to check out a name which is not in desktop as well as appropriate
    // as per meaning of the files contained by that folder
    for (int index = 0; index < fileMap.size(); index++)
    {
        int flag = 0;
        int j = 1;
        string fName = fileMap[index];
        string fileNam = fName;
        while (flag != 1)
        {
            string folderr = dir + fName;
            struct stat sb;
            if (stat(folderr.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
            {
                fName.clear();
                fName = fileNam + "_" + to_string(j);
                j++;
            }
            else
            {
                dirNameMapping.insert(make_pair(fileMap[index], fName));
                flag = 1;
                break;
            }
        }
    }




    ////////////////////////


    ////All the folder chosen by us in last loop need to be created now
    /// if condition is to check if there is no file for any folder then that folder must not
    // get created
    for (auto& it : dirNameMapping)
    {
        string dirName = rootPath + "Documents/" + it.second;
        int status;
        if (mymap[it.first].size() > 0)
        {
            cout<<dirName<<"\n";
            if (it.first != "other")
                undoMap.insert(make_pair(dirName, directory));
            status = mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        dirName.clear();
    }


    for (auto& it : mymap)
    {
        if (it.first == "image")
        {
            vector<string> img = mymap[it.first];
            for (int index = 0; index < img.size(); index++)
            {
                string source = "\"" + img[index] + "\"";
                int indexOfSlash = source.find_last_of("/");
                string fileNm = source.substr(indexOfSlash);
                fileNm = fileNm.substr(0, fileNm.length()-1);
                string dest
                    = "\"" + rootPath + "Documents/" + dirNameMapping[it.first] + fileNm + "\"";
                string syntax = "mv " + source + " " + dest;
                system(syntax.c_str());
                syntax.clear();
            }
            if (mymap[it.first].size() > 0)
                cout << "All the images are in now " << dirNameMapping["image"]
                     << " folder\n";
        }
        if (it.first == "pdf")
        {
            vector<string> img = mymap[it.first];
            for (int index = 0; index < img.size(); index++)
            {
                string source = "\"" + img[index] + "\"";
                int indexOfSlash = source.find_last_of("/");
                string fileNm = source.substr(indexOfSlash);
                fileNm = fileNm.substr(0, fileNm.length()-1);
                string dest
                    = "\"" + rootPath + "Documents/" + dirNameMapping[it.first] + fileNm + "\"";
                string syntax = "mv " + source + " " + dest;
                system(syntax.c_str());
            }
            if (mymap[it.first].size() > 0)
                cout << "All the PDFs are in now " << dirNameMapping["pdf"] << " folder\n";
        }
        if (it.first == "video")
        {
            vector<string> img = mymap[it.first];
            for (int index = 0; index < img.size(); index++)
            {
                string source = "\"" + img[index] + "\"";
                int indexOfSlash = source.find_last_of("/");
                string fileNm = source.substr(indexOfSlash);
                fileNm = fileNm.substr(0, fileNm.length()-1);
                string dest
                    = "\"" + rootPath + "Documents/" + dirNameMapping[it.first] + fileNm + "\"";
                string syntax = "mv " + source + " " + dest;
                system(syntax.c_str());
            }
            if (mymap[it.first].size() > 0)
                cout << "All the videos are in now " << dirNameMapping["video"]
                     << " folder\n";
        }
        if (it.first == "doc")
        {
            vector<string> img = mymap[it.first];
            for (int index = 0; index < img.size(); index++)
            {
                string source = "\"" + img[index] + "\"";
                int indexOfSlash = source.find_last_of("/");
                string fileNm = source.substr(indexOfSlash);
                fileNm = fileNm.substr(0, fileNm.length()-1);
                string dest
                    = "\"" + rootPath + "Documents/" + dirNameMapping[it.first] + fileNm + "\"";
                string syntax = "mv " + source + " " + dest;
                system(syntax.c_str());
            }
            if (mymap[it.first].size() > 0)
                cout << "All the documents are in now " << dirNameMapping["doc"]
                     << " folder\n";
        }
        if (it.first == "prog")
        {
            vector<string> img = mymap[it.first];
            for (int index = 0; index < img.size(); index++)
            {
                string source = "\"" + img[index] + "\"";
                int indexOfSlash = source.find_last_of("/");
                string fileNm = source.substr(indexOfSlash);
                fileNm = fileNm.substr(0, fileNm.length()-1);
                string dest
                    = "\"" + rootPath + "Documents/" + dirNameMapping[it.first] + fileNm + "\"";
                string syntax = "mv " + source + " " + dest;
                system(syntax.c_str());
            }
            if (mymap[it.first].size() > 0)
                cout << "All the program files are in now " << dirNameMapping["prog"]
                     << " folder\n";
        }
        if (it.first == "other")
        {
            vector<string> img = mymap[it.first];
            for (int index = 0; index < img.size(); index++)
            {
                string source = "\"" + img[index] + "\"";
                ///////////////////////////////////
                int indexOfSlash = source.find_last_of("/");
                string fileNm = source.substr(indexOfSlash);
                fileNm = fileNm.substr(0, fileNm.length()-1);
                int indexOfDot = source.find_last_of(".");
                string newFolder;
                int flag = 0;
                if (indexOfDot < source.length())
                    newFolder = img[index].substr(indexOfDot);
                else
                    flag = 1;
                if (flag == 0)
                {
                    string dest
                        = rootPath + "Documents/" + dirNameMapping[it.first] + "/" + newFolder;
                    struct stat sb;
                    if (stat(dest.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
                    {
                        /// nothing if folder exists
                    }
                    else
                    {
                        int status;
                        status = mkdir(dest.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                        undoMap.insert(make_pair(dest, directory));
                    }
                    dest = "\"" + dest + "\"";
                    string syntax = "mv " + source + " " + dest;
                    system(syntax.c_str());
                }
                else
                {
                    string source = "\"" + img[index] + "\"";
                    int indexOfSlash = source.find_last_of("/");
                    string fileNm = source.substr(indexOfSlash);
                    fileNm = fileNm.substr(0, fileNm.length()-1);
                    string dest
                        = "\"" + rootPath + "Documents/" + dirNameMapping[it.first] + fileNm + "\"";
                    string syntax = "mv " + source + " " + dest;
                    system(syntax.c_str());
                }
            }
            if (mymap[it.first].size() > 0)
                cout << "All the miscellaneous files are in now " << dirNameMapping["other"]
                     << " folder\n";
        }
    }
    closedir(dp);


    cout << "\n\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            "\n";
    cout << "if you did not like arrangement you can undo it now, try it!!Press 'Y' or 'y'\n";
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    //cout<<"321 mapping :  "<<dirNameMapping["other"]<<"\n";
    char choice;
    cin >> choice;
    if (choice == 'Y' || choice == 'y')
    {
        for(mit=undoMap.begin();mit!=undoMap.end();mit++)
        {
           string fPath = mit->first;
           //cout<<fPath<<"  "<<mit->second<<"  path\n";
            if ((dp = opendir(fPath.c_str())) != NULL)
            {
                // cout << "Error(" << errno << ") opening " << dir << endl;
                while ((dirp = readdir(dp)) != NULL)
                {
                    string name(dirp->d_name);
                    if (name != "." && name != "..")
                    {
                        string test = dir + name + "\n";
                        if (dirp->d_type == DT_DIR)
                        {
                            /// ignored all the folder that are present in desktop as they may be
                            /// present
                            // there for a specific purpose(assumption)
                            string folder = dir + name; /// if folder needs to be moved***code here
                        }
                        else
                        {

                            string source = "\"" + fPath + "/" + name + "\"";
                            string dest = "\"" + mit->second + "\"";
                            string syntax = "mv " + source + " " + dest;
                            system(syntax.c_str());
                        }
                    }
                }
                string syntax = "rmdir " + fPath;
                system(syntax.c_str());
            }
        }
        ////////////
        string otherFolderName = rootPath+"Documents/"+ dirNameMapping["other"];
        if ((dp = opendir(otherFolderName.c_str())) != NULL)
        {
            while ((dirp = readdir(dp)) != NULL)
            {
                string name(dirp->d_name);
                if (name != "." && name != "..")
                {
                    string test = dir + name;
                    if (dirp->d_type == DT_DIR)
                    {
                        /// ignored all the folder that are present in desktop as they may be
                        /// present
                        // there for a specific purpose(assumption)
                        string folder = dir + name; /// if folder needs to be moved***code here
                    }
                    else
                    {
                        string source = "\"" + otherFolderName + "/" + name + "\"";
                        string dest = "\"" + directory + "\"";
                        string syntax = "mv " + source + " " + dest;
                        system(syntax.c_str());
                    }
                }
            }
            string syntax = "rmdir " + otherFolderName;
            system(syntax.c_str());

            cout<<"\n\n===============Undone Successfully===================\n\n";
        }

        ////////////////
    }

    return;
}


void getNamesOfFile(string dir,
    priority_queue<pair<int, string>, vector<pair<int, string> >, greater<pair<int, string> > >& pq)
{
    DIR* dp;
    struct dirent* dirp;
    if ((dp = opendir(dir.c_str())) == NULL)
    {
        // cout << "Error(" << errno << ") opening " << dir << endl;
        return;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        string name(dirp->d_name);
        if (name != "." && name != "..")
        {
            string test = dir + "/" + name;
            if (dirp->d_type == DT_DIR)
            {
                getNamesOfFile(test, pq);
            }
            else
            {
                struct stat filestatus;
                stat(test.c_str(), &filestatus);
                // cout << test<<"  "<<filestatus.st_size << " bytes\n";
                pq.push(make_pair(filestatus.st_size, test));
                if (pq.size() > 11)
                {
                    pq.pop();
                }
            }
        }
    }
    closedir(dp);
}


void getTopFiles()
{
    priority_queue<pair<int, string>, vector<pair<int, string> >, greater<pair<int, string> > > pq;
    string root = "/home";
    vector<string> files = vector<string>();
    getNamesOfFile(root, pq);
    cout << "size of queue " << pq.size();
    if (pq.size() > 10)
        pq.pop();
    for (int i = 0; i < 10; i++)
    {
        float c = pq.top().first / 1024.0;
        printf("%.2fMB %s\n", c / 1024, pq.top().second.c_str());
        pq.pop();
    }
    root.clear();
    char choice;
    cout << "Do you want to delete any of these file press y/Y!\n";
    cin >> choice;
    if (choice == 'y' || choice == 'Y')
    {
        cout << "copy absolute path here\n";
        cin.ignore();
        getline(cin, root);
        ifstream fp;
        fp.open(root.c_str());
        if (fp.good())
        {
            string syntax = "rm " + root;
            system(syntax.c_str());
            fp.close();
        }
        else
        {
            cout << "ERROR! File does not exist.\n";
            return;
        }
    }
    return;
}

void getDuplicateFile(string path)
{
    string Path;
    cout << "Please give absolute file path\n";
    cin.ignore();
    getline(cin, Path);
    string dupPath = "\"" + Path + "\"";
    string syntax = "fdupes -r " + dupPath;
    system(syntax.c_str());
}

void compressFile()
{
    string path;
    cout<<"please copy absolute path below"<<"\n";
    cin.ignore();
    getline(cin,path);
    path = "\"" + path + "\"";
    string syntax = "gzip " + path;
    system(syntax.c_str());
    syntax.clear();
    cout<<"\n\n==========compressed file Successfully=====================\n\n\n";
}

int main(int argc, char* argv[])
{
    char uniqFileName[32] = "myTmpFile-wH5sLq.txt";
    struct utsname sysinfo;
    if (uname(&sysinfo))
        exit(9);
    string os_name = sysinfo.sysname;

    ///////**************getUserName****************////////
    char* user_name = getenv("USER");
    //********************************************//

    string FolderPath = "/home/" + (string)user_name+"/";

    if (os_name == "Linux")
    {
        while (1)
        {
            char choice;
            cout << "*****========================================================================="
                    "======*****\n";
            cout << "To clean any folder,press 1.\nTo get top 10 files,press 2.\nTo see "
                    "duplicates files in a particular folder,press 3.\nTo compress a File,press "
                    "4.\nTo terminate program,press 5.\n";
            /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            +Make a choice-
            +Press 1->if user wants to clean desktop.
            +Press 2->If user wants to get top 10 biggest files in the system.
            +Press 3->Providing folder name, user can get information of duplicate file.
            +Press 4->If user wants to compress any file, He/She just need to provide absolute path.
            +Press 5->To terminate the program.
            ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
            mymap = empMymap;
            dirNameMapping = empDirName;
            undoMap = empUndo;
            cin >> choice;
            switch (choice)
            {
                case '1':
                {
            //*********************************If desktop cleaning you need*******************************//
                    string fPath;

                    cout<<"Give absolute file path :\n";
                    cin.ignore();
                    getline(cin,fPath);
                    string dir = string(fPath);
                    listDir(dir, FolderPath);
                    break;
                }
                case '2':
                {
            //*******************************Top 10 files in size*****************************************//
                    getTopFiles();
                    cout << "if want to compress,Press Y/y\n";
                    char ch;
                    cin>>ch;
                    if(ch=='Y' ||ch=='y')
                    compressFile();
                    break;
                }
                case '3':
                {
            //*********Provide a path and get all the duplicate files present in that folder**************//
                    getDuplicateFile(FolderPath);
                    break;
                }
                case '4':
                {
           //********************provide a file name with absolute path to compress it********************//
                    cout << "if want to compress, please copy absolute path below\n";
                    compressFile();
                    break;
                }
                case '5':
                {
            //***********************************Terminate the program***********************************//
                    return 0;
                }
                default:
                {
            //*******************************When user makes a wrong choice******************************//
                    cout << "make a appropriate choice\n";
                }
            }
        }
    }
    else
    {
        cout << "Does not support in windows!!"
             << "\n";
    }

    return 0;
}
