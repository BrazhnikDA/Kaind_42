import datetime
import gzip
import io
import os
import shutil


def WriteLog(text):
    now = datetime.datetime.now()
    file = open('C:\\bin\\kaind_42.log', 'a')
    #print("Текущее время: ", now)
    file.seek(0, 2)  # перемещение курсора в конец файла

    month = str(now.month)
    if len(month) < 2:
        month = '0' + str(now.month)

    day = str(now.day)
    if len(day) < 2:
        day = '0' + str(now.day)

    hour = str(now.hour)
    if len(hour) < 2:
        hour = '0' + str(now.hour)

    minute = str(now.minute)
    if len(minute) < 2:
        minute = '0' + str(now.minute)

    second = str(now.second)
    if len(second) < 2:
        second = '0' + str(now.second)

    dated = str(now.year) + "/" + month + "/" + day + ", " + hour + ":" + minute + ":" + second
    file.write('[' + dated + '] ' + text + '\n')  # Записываем
    file.close()  # Закрываем файл


# Пути до папок
PathFolderBad = "Z:\\Bad\\"
PathFolderBackup = "Z:\\Backup\\"
PathFolderArchive = "Z:\\Archive\\"
PathFolderSource = "Z:\\Source\\"

# Первая строка в файлах Z:\Bad по которой ищем нужный файл
KEY_FIND_ERROR = "Пустое значение параметра: номер маршрута\n"


def Replace(_pathIn, _pathOut):
    os.replace(_pathIn, _pathOut)
    #print("Файл перемещён из " + _pathIn + " В " + _pathOut)


def Copy(_pathIn, _pathOut):
    shutil.copyfile(_pathIn, _pathOut)
    #print("Файл скопирован из " + _pathIn + " В " + _pathOut)


def ReadFile(_path):
    file = open(_path, "r")
    line = file.readline()
    if line == KEY_FIND_ERROR:
        file.close
        #print("FIND: " + line.strip() + "    PATH - " + _path)
        return True
    #print("No find: " + line.strip())
    file.close
    return False


def GetFilesInFolderBad(_path):
    files = GetListFiles(_path)
    return files


def GetFilesInFolderBackup(_path, _name):
    content = os.listdir(_path)

    #print("Все файлы в Backup: ")
    #for n in content:
        #print(n)

    error = []
    for file in content:
        if os.path.isfile(os.path.join(_path, file)) and file.endswith('.txt'):
            error.append(file)
    out = []
    for name in content:
        for minName in _name:
            if name[:30] == minName[:30]:
                #print("Bad: Name[30], BackUp: _Name[30] ", name[:30], minName[:30])
                out.append(name)
    return out


# Получить список файлов в папке
def GetListFiles(_path):
    content = os.listdir(_path)
    error = []
    for file in content:
        if os.path.isfile(os.path.join(_path, file)) and file.endswith('.err'):
            error.append(file)
    return error


# Получить список папок
def GetListFolders(_path):
    with os.scandir(_path) as files:
        subdir = [file.name for file in files if file.is_dir()]
    return subdir


def WorkInArchive(_path):
    file_mode = 'rb+'
    out = ""
    find = ""
    IsFind = False
    with gzip.open(_path, file_mode) as input_file:
        with io.TextIOWrapper(input_file, encoding='utf-8') as dec:
            while True:
                line = dec.readline()
                IsFind = False
                if not line:
                    break
                #print(line.strip())
                if line[1] == '\t':
                    if line[2] == '4' and line[3] == '2':
                        find = line
                        IsFind = True
                if line[2] == '\t':
                    if line[3] == '4' and line[4] == '2':
                        find = line
                        IsFind = True
                if line[3] == '\t':
                    if line[4] == '4' and line[5] == '2':
                        find = line
                        IsFind = True
                if IsFind == False:
                    out += line

    #print("Find - " + find)

    #print("Стало:")
    #print(out)

    file_mode = 'wb+'
    with gzip.open(_path, file_mode) as output:
        with io.TextIOWrapper(output, encoding='utf-8') as encode:
            encode.writelines(out)

    if find != "":
        return True
    else:
        return False


def checkNameFolder(name):
    # Good = 2021-10-05
    if len(name) == 10:
        if name[0] == '2':
            if name[1] == '0':
                if name[2] == '2':
                    if name[4] == '-':
                        if name[7] == '-':
                            return True
    return False


def main():
    foldersBad = GetListFolders(PathFolderBad)  # Получить названия папок в Z:/Bad
    freshFolderBad = foldersBad[len(foldersBad) - 1]  # Получить последнюю (самую свежую по дате) папку в Z:/Bad
    filesInFolder = GetFilesInFolderBad(PathFolderBad + freshFolderBad)  # Получить файлы в свежой папке

    textLog = 'Запуск, '

    currentFile = []  # Для хранения файлов которые подходят из Z:/Bad/Последняя_дата
    for name in filesInFolder:
        if ReadFile(PathFolderBad + freshFolderBad + "\\" + name):
            currentFile.append(name)

    if len(currentFile) < 1:
        textLog += 'Bad не содержит файлов.'
        WriteLog(textLog)
        return
    else:
        textLog += 'Файлы в Bad найдены (' + str(len(currentFile)) + '), '

    foldersBackup = GetListFolders(PathFolderBackup)  # Получить названия папок в Z:/Backup
    m = 1
    while True:
        if len(foldersBackup) >= m:
            freshFolderBackup = foldersBackup[len(foldersBackup) - m]  # Получить последнюю по дате папку в Z:/Backup
        else: break
        if checkNameFolder(freshFolderBackup):
            break
        m = m + 1
    #print("Самая свежая папка Backup - ", freshFolderBackup)
    filesInFolder = GetFilesInFolderBackup(PathFolderBackup + freshFolderBackup,
                                           currentFile)  # Получить файлы из Z:/Backup где первые 30 символов в названии совпадают с файлами из Z:/Bad
    #print("Найденные файлы с совпавшим названием: ")
    #for n in filesInFolder:
        #print(n)

    if len(filesInFolder) < 1:
        textLog += 'Backup не содержит нужных файлов'
        WriteLog(textLog)
        return
    else:
        textLog += 'Файлы в Backup найдены (' + str(len(filesInFolder)) + '), '
        #print("Файлы в backup найдены")

    # Зайти в архив gzip если строчка где во 2-ом столбце имеется цифра 42 удалить строку и так до конца, сохранить файл и заорхивировать его обратно в gzip
    i = 0
    for pathArchive in filesInFolder:
        IsFind = WorkInArchive(PathFolderBackup + freshFolderBackup + "\\" + pathArchive)
        if IsFind:
            Copy(PathFolderBackup + freshFolderBackup + "\\" + pathArchive,
                 PathFolderSource + pathArchive)  # Если хотя бы 1 строка была удалена, то создать копию файла по пути который указан 2-ым параметром
            Replace(PathFolderBad + freshFolderBad + "\\" + currentFile[i], PathFolderArchive + currentFile[i])
            i += 1

    if(i == 0):
        textLog += ' Ни один архив не содержит строку 42. '
    else:
        textLog += ' Архив ' + '(' + str(i) + ')' + ' исправлен, файлы скопированы и перенесены. '

    WriteLog(textLog)


if __name__ == "__main__":
    main()
    input()
