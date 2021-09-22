#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <iostream>
#include <windows.h>
#include <string>
#include <omp.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <zip.h>


#define SIZE_MASSIVE 100

#define PathFolderBad      L"E:\\Bad\\"
#define PathFolderBackup   L"E:\\Backup\\"

#define KEY_FIND_ERROR     L"Пустое значение параметра: номер маршрута"

std::wstring* FindFolder(std::wstring NameFolder[], std::wstring path);                                    // Поиск папок
std::wstring* FindFilesInFolder(std::wstring NamesFiles[], std::wstring NameFolder, std::wstring path);    // Поиск файлов в папке
std::wstring* FindFilesInFolder(std::wstring NamesFiles[], std::wstring NameFolder, std::wstring keyFind, std::wstring path);    // Поиск файлов в папке
std::wstring  CheckFirstLine(std::wstring AllPath);                                     // Проверка первой строки файла
int CountFolder(std::wstring path);                                                     // Количество найденных папок
bool ComparingSubstrings(std::wstring first, std::wstring second);

std::wstring currentFolder; // Текущая папка
std::wstring foundFile;     // Название найденного файла

int main()
{
	ShowWindow(GetConsoleWindow(), SW_SHOW); 
    std::locale system("");
    std::locale::global(system);


    int countFolderBad = CountFolder(PathFolderBad);
    std::wstring* NameFolderBad = new std::wstring[CountFolder(PathFolderBad) + 2];
    std::wstring  NameFilesBad[SIZE_MASSIVE];

    int countFolderBackup = CountFolder(PathFolderBackup);
    std::wstring* NameFolderBackup = new std::wstring[CountFolder(PathFolderBackup) + 2];
    std::wstring  NameFilesBackup[SIZE_MASSIVE];

    std::wstring PathTo;      // Откуда
    std::wstring PathFrom;    // Куда

    FindFolder(NameFolderBad, PathFolderBad);


    // Z/Bad
    FindFilesInFolder(NameFilesBad, NameFolderBad[countFolderBad - 1], PathFolderBad);

    bool FileIsFound = false;
    int bad_files = 0;
    for (unsigned int j = 0; j < NameFilesBad[j].size(); j++)
    {
        std::wstring FLine = CheckFirstLine(PathFolderBad + NameFolderBad[countFolderBad - 1] + L"\\" + NameFilesBad[j]);
        std::wcout << "First line: " << FLine << "\n";
        /*if (FLine == KEY_FIND_ERROR)
        { 
            std::wstring tmpPath = PathFolderBad + NameFolderBad[countFolder - 1] + L"\\" + NameFilesBad[j];
            std::wstring tmpPathCopy = PathForFolderCopy + NameFilesBad[j];
            if (!(MoveFile(tmpPath.c_str(), tmpPathCopy.c_str())))   // Откуда -> Куда
            {
                bad_files++;
            } 
            FileIsFound = true;
        }*/
        if (FLine != KEY_FIND_ERROR)
        {
            NameFilesBad[j] = L"";
        }
        else
        {
            foundFile = NameFilesBad[j];
        }
        //NameFilesBad[j] = L"";
    }

    FindFolder(NameFolderBackup, PathFolderBackup);

    if (countFolderBackup > 0)
    {
        std::wcout << L"Selected folder \"" << NameFolderBackup[countFolderBackup - 1] << L"\", ";
    }
    else
    {
        std::wcout << L"Folders not founded";
        return 0;
    }


    // Z/Backup
    FindFilesInFolder(NameFilesBackup, NameFolderBackup[countFolderBackup - 1], foundFile, PathFolderBackup);

    FileIsFound = false;
    bad_files = 0;
    for (unsigned int j = 0; j < NameFilesBackup[j].size(); j++)
    {
        std::wstring FLine = CheckFirstLine(PathFolderBackup + NameFolderBackup[countFolderBackup - 1] + L"\\" + NameFilesBackup[j]);
        std::wcout << "First line: " << FLine << "\n";

        if (FLine != KEY_FIND_ERROR)
        {
            NameFilesBackup[j] = L"";
        }
    }

	return 0;
}

int CountFolder(std::wstring path)
{
    int res = 0;
    WIN32_FIND_DATAW wfd;
    path += L"*";
    HANDLE const hFind = FindFirstFileW(path.c_str(), &wfd);
    int i = 0;
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            std::wstring tmpNameFile = wfd.cFileName;

            if (!(tmpNameFile.find('.') != std::wstring::npos))
            {
                res++;
            }
        } while (NULL != FindNextFileW(hFind, &wfd));

        FindClose(hFind);
    }

    return res;
}

bool ComparingSubstrings(std::wstring first, std::wstring second)
{
    std::wstring subFirst, subSecond;   // Инициализация
    if (first.size() < 30 || second.size() < 30) { return false; }      // Если название файла меньше 30 символов выходим, что бы не вызвать оишбку в цикле
    for (int i = 0; i < 30; ++i)
    {
        subFirst  += first[i];
        subSecond += second[i];
    }
    if (subFirst == subSecond) { return true; }
    else { return false; }
}

std::wstring* FindFolder(std::wstring NameFolder[], std::wstring path)
{
    // Для просмотра файлов в директории
    WIN32_FIND_DATAW wfd;
    // Где искать и что  (* - все расширения, *.png - все png)
    path += L"*";
    HANDLE const hFind = FindFirstFileW(path.c_str(), &wfd);

    // Поиск файлов и папок и вывод
    int i = 0;
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            std::wstring tmpNameFile = wfd.cFileName;

            if (!(tmpNameFile.find('.') != std::wstring::npos))
            {
                NameFolder[i] = wfd.cFileName;
                i++;
            }
        } while (NULL != FindNextFileW(hFind, &wfd));

        FindClose(hFind);
    }
    NameFolder[i + 1] = L"\0";

    return NameFolder;
}

std::wstring* FindFilesInFolder(std::wstring NamesFiles[], std::wstring NameFolder, std::wstring path)
{
    // Для просмотра файлов в директории
    WIN32_FIND_DATAW wfd;
    // Где искать и что  (* - все расширения, *.png - все png)
    std::wstring allPath = path + NameFolder + L"\\*.err";
    HANDLE const hFind = FindFirstFileW(allPath.c_str(), &wfd);         //(L"D:\\1\\ТЕСТ_2\\2021-03-05\\*.err", &wfd);

    // Поиск файлов и папок и вывод
    int i = 0;
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            NamesFiles[i] = wfd.cFileName;
            i++;
        } while (NULL != FindNextFileW(hFind, &wfd));

        FindClose(hFind);
    }
    NamesFiles[i + 1] = L"\0";

    return NamesFiles;
}

std::wstring* FindFilesInFolder(std::wstring NamesFiles[], std::wstring NameFolder, std::wstring keyFind, std::wstring path)
{
    // Для просмотра файлов в директории
    WIN32_FIND_DATAW wfd;
    // Где искать и что  (* - все расширения, *.png - все png)
    std::wstring allPath = path + NameFolder + L"\\*.err";
    HANDLE const hFind = FindFirstFileW(allPath.c_str(), &wfd);         //(L"D:\\1\\ТЕСТ_2\\2021-03-05\\*.err", &wfd);

    // Поиск файлов и папок и вывод
    int i = 0;
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            if (ComparingSubstrings(keyFind, wfd.cFileName))
            {
                NamesFiles[i] = wfd.cFileName;
                return NamesFiles;
            }
            i++;
        } while (NULL != FindNextFileW(hFind, &wfd));

        FindClose(hFind);
    }
    NamesFiles[i + 1] = L"\0";

    return NamesFiles;
}

std::wstring CheckFirstLine(std::wstring AllPath)
{
    std::wifstream fs(AllPath);
    if (fs)
    {
        std::wstring FLine;
        getline(fs, FLine);
        fs.close();
        return FLine;
    }
    fs.close();
    return L"";
}
