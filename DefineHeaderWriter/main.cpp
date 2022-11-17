//TODO move funcs to classes
//TODO build release exec to artifacts folder
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>


typedef char byte;
typedef std::vector<char> binary;

// args: 1. file path
std::string parseArgFrom(const char** const pArgv, const size_t index)
{
    if ((pArgv == nullptr))
    {
        std::cout << "Pointer error\r\n";
    }

    std::string data;

    size_t j = 0;
    while (pArgv[index][j] != '\0')
    {
        data = data + pArgv[index][j];
        ++j;
    }
    return data;
};

std::string retrieveFilePathFrom(const char** const pArgv)
{
    if ((pArgv == nullptr))
    {
        std::cout << "Pointer error\r\n";
    }

    return parseArgFrom(pArgv, 1);
}

size_t retrieveFileSize(std::ifstream& rFile)
{
    rFile.seekg(0, std::ios::end);
    size_t size = rFile.tellg();
    rFile.seekg(0, std::ios::beg);

    return size;
}

std::string retrieveFileName(const std::string& rFilePath)
{
    std::string fileName = "";

        //find first '\' from right

        size_t index = rFilePath.length() - 1;
        while (index != 0)
        {
            if (rFilePath[index] == '\\')
            {
                //save file name
                fileName = rFilePath.substr(index + 1, rFilePath.length() - index);
                index = 0;
            }
            else
            {
                index--;
            }

        }

        return fileName;
}

std::string removeExtention(const std::string& rFileName)
{
    if (rFileName.length() <= 0)
    {
        return "";
    }

    size_t index = rFileName.length() - 1;
    while (index != 0)
    {
        if (rFileName[index] == '.')
        {
            //remove extention
            return rFileName.substr(0, index);
        }
        else
        {
            --index;
        }
    }

    return rFileName;
}

std::string toUpperCase(const std::string& rFileName)
{
    std::string n = rFileName;
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);
    return n;
}

std::string createDefineWithFileName(const std::string& rFileName)
{
    std::string result;
    result = removeExtention(rFileName);
    result = toUpperCase(result);

    //add _H
    result += u8"_H";

    return result;
}
std::string createHeadDefine(const std::string& rDefineFileName)
{
    return u8"#ifndef " + rDefineFileName + u8"\r\n" + u8"#define " + rDefineFileName + u8"\r\n";
}

std::string createEndif(const std::string& rDefineFileName)
{
    return u8"\r\n#endif //" + rDefineFileName;
}


binary* loadFile(const std::string& rFilePath, size_t& rOutSize)
{

    std::ifstream myFile(rFilePath, std::ios::binary);
    const size_t size = retrieveFileSize(myFile);
    rOutSize = size;

    //byte* buffer = new byte[size];

    binary* pBuffer = new std::vector<char>();

    if ((size > 0) && (myFile.is_open()))
    {

        pBuffer->resize(size);

        try {
            myFile.read(&pBuffer->at(0), size);
        }

        catch (std::exception& rE)
        {

            std::cout << "exeption read file " << rE.what() << "\r\n";
        }

        if (!myFile.eof())
        {
            std::cout << "End of file has not reached. Readed " << myFile.gcount() << " of " << retrieveFileSize(myFile) << " size" << "\r\n";
            myFile.clear();
            myFile.close();
            return pBuffer;
        }

        myFile.clear();
        myFile.close();
        return pBuffer;
    }

    myFile.clear();
    myFile.close();
    pBuffer->resize(0);
    return pBuffer;

    
}
bool hasBOM(const binary* const pBuffer)
{
    if ((pBuffer == nullptr))
    {
        std::cout << "Pointer error\r\n";
    }
    // has BOM bytes?

    if ((pBuffer->at(0) == static_cast<char>(0xEF)) && (pBuffer->at(1) == static_cast<char>(0xBB)) && (pBuffer->at(2) == static_cast<char>(0xBF)))
    {
        return true;
    }

    return false;
}

binary* createBOMBytes(size_t& rOutSize)
{

    binary* pBuffer = new std::vector<char>{ static_cast<char>(0xEF) , static_cast<char>(0xBB), static_cast<char>(0xBF) };
    rOutSize = pBuffer->size();

    return pBuffer;
}

bool appendBuffersLeft(binary* const pOutDst, const size_t dSize, const binary* const pSrc, const size_t sSize)
{
    if ((pOutDst == nullptr) || (pSrc == nullptr))
    {
        std::cout << "Pointer error\r\n";
    }

    const size_t newSize = dSize + sSize;
    binary* pNewBuffer = new std::vector<char>(newSize);
    //newBuffer->resize(newSize);

    //add source
    for (size_t i = 0; i < sSize; ++i)
    {
        pNewBuffer->at(i) = pSrc->at(i);
    }

    //add dst
    for (size_t i = sSize; i < newSize; ++i)
    {
        pNewBuffer->at(i) = pOutDst->at(i - sSize);
    }

    pOutDst->swap(*pNewBuffer);

    delete pNewBuffer;

    return true;
}

bool appendBuffersRight(binary* const pOutDst, const size_t dSize, const binary* const pSrc, const size_t sSize)
{
    if ((pOutDst == nullptr) || (pSrc == nullptr))
    {
        std::cout << "Pointer error\r\n";
    }

    size_t newSize = dSize + sSize;
    binary* pNewBuffer = new std::vector<char>(newSize);
    //newBuffer->resize(newSize);
    

    // just fill up by dst
    for (size_t i = 0; i < dSize; ++i)
    {
        pNewBuffer->at(i) = pOutDst->at(i);
    }
    //add src
    for (size_t i = dSize; i < newSize;++i)
    {
        pNewBuffer->at(i) = pSrc->at(i - dSize);
    }

    pOutDst->swap(*pNewBuffer);

    delete pNewBuffer;

    return true;
}

bool appendBuffersMiddle(binary* const pOutDst, const size_t dSize, const size_t offset, const binary* const pSrc, const size_t sSize)
{
    if ((pOutDst == nullptr) || (pSrc == nullptr))
    {
        std::cout << "Pointer error\r\n";
    }

    size_t newSize = dSize + sSize;
    binary* pNewBuffer = new std::vector<char>(newSize);

    //size_t rightChopSize = newSize - offset;
    //binary* rightChop = new std::vector<char>(rightChopSize);

    //set left part os dst

    for (size_t i = 0; i < offset; ++i)
    {
        pNewBuffer->at(i) = pOutDst->at(i);
    }

    //set source data after left part

    for (size_t i = offset; i < sSize + offset; ++i)
    {
        pNewBuffer->at(i) = pSrc->at(i - offset);
    }

    //save right part of dst
    /*
    for (size_t i = 0; i < rightChopSize; ++i)
    {
        rightChop->at(i) = dst->at(i + offset);
    }
    */


    //set right part. must fit to newSize

    for (size_t i = (sSize + offset); i < newSize; ++i)
    {

        //new i -- i + dSize - offset
        pNewBuffer->at(i) = pOutDst->at(i - (sSize + offset) + offset); // + offset because it still exist in dst buffer

    }


    //delete rightChop;

    pOutDst->swap(*pNewBuffer);

    delete pNewBuffer;

    return true;
}

void insertBuffer(binary* const pOutDst, const size_t dSize, const size_t offset, const binary* const pSrc, const size_t sSize)
{
    if ((pOutDst == nullptr) || (pSrc == nullptr))
    {
        std::cout << "Pointer error\r\n";
    }
    //can't fit
    if (sSize > dSize)
    {
        return;
    }

    //out border
    if (offset >= dSize)
    {
        return;
    }

    //can't fit
    if (offset + sSize > dSize)
    {
        return;
    }


    const size_t newSize = dSize;
    binary* pNewBuffer = new std::vector<char>(newSize);

    //left
    if (offset == 0)
    {
        //add src
        for (size_t i = 0; i < sSize; ++i)
        {
            pNewBuffer->at(i) = pSrc->at(i);
        }

        
        //add dst
        for (size_t i = sSize; i < dSize; ++i)
        {
            pNewBuffer->at(i)  = pOutDst->at(i);
        }

        pOutDst->swap(*pNewBuffer);
        
        delete pNewBuffer;
    }

    // middle or right
    if (offset > 0)
    {
        //add dst

        for (size_t i = 0; i < offset; ++i)
        {
            pNewBuffer->at(i) = pOutDst->at(i);
        }

        //if middle
        if ((dSize - (offset + sSize)) > 0)
        {
            //add src

            for (size_t i = offset; i < sSize; ++i)
            {
                pNewBuffer->at(i) = pSrc->at(i);
            }

            //add dst
            for (size_t i = offset + sSize; i < dSize; ++i)
            {
                pNewBuffer->at(i) = pOutDst->at(i);
            }

            pOutDst->swap(*pNewBuffer);

            delete pNewBuffer;

        }
        //right
        else
        {
            //add src
            for (size_t i = offset; i < sSize; ++i)
            {
                pNewBuffer->at(i) = pSrc->at(i);
            }

            pOutDst->swap(*pNewBuffer);

            delete pNewBuffer;
        }
    }

}

//#define _TEST
size_t stringToBuffer(const std::string& rStr, binary* const pOutBuffer)
{
    if (pOutBuffer == nullptr)
    {
        std::cout << "Pointer error\r\n";
    }

    binary* pBufferd = new std::vector<char>(rStr.begin(), rStr.end());

    size_t size = pBufferd->size();

    pOutBuffer->swap(*pBufferd);

    

    delete pBufferd;

    return size;
}
//TODO
bool isDefineExist(const binary* const pBuffer, size_t& rOutIndex)
{

    return false;
}



int main(int argc, char ** argv)
{
    
    if (argc != 3)
    {
        std::cout << "arg error. must be 2 args. 1. srcFilePath 2. dstFilePath\r\n";
        return 1;
    }

#ifdef  _TEST
    const size_t fileSize = 100;
    std::string filePath = "j:"; //getFilePath(argv);
    filePath += "\\";
    filePath += "programm files";
    filePath += "\\";
    filePath += "server.exe";
#else
    std::string filePath = retrieveFilePathFrom(const_cast<const char** const>(argv));
#endif //  _TEST


    if (filePath.length() <= 0)
    {
        return 1;
    }

    const std::string fileName = retrieveFileName(filePath);
    std::string modedFileName = createDefineWithFileName(fileName);

    if (fileName.length() <= 0)
    {
        return 1;
    }

    binary* pDefineBuffer = new std::vector<char>();
    const size_t defineSize = stringToBuffer(createHeadDefine(modedFileName), pDefineBuffer);

    binary* pEndifBuffer = new std::vector<char>();
    const size_t endifSize = stringToBuffer(createEndif(modedFileName), pEndifBuffer);

#ifdef _TEST
    byte* fileBuffer = new byte[fileSize]; //getFileBuffer(filePath, fileSize);
    for (size_t i = 0; i < fileSize; ++i)
    {
        fileBuffer[i] = u8'a';
        if (i == fileSize - 1)
        {
            fileBuffer[i] = u8'\n';
        }
    }

    byte* bomb = new byte;
    size_t siase = getBOMBuffer(bomb);
    insertBuffer(fileBuffer, fileSize, 0, bomb, siase);

#else
    size_t fileSize = 0;
    binary* pFileBuffer = loadFile(filePath, fileSize);
#endif // _TEST


    const size_t outputSize = fileSize + defineSize + endifSize;
    binary* pOutputBuffer = new std::vector<char>(outputSize);


    //filling up outoutBuffer
    const bool hasBom = hasBOM(pFileBuffer);
    size_t bomSize = 0;

    if (hasBom)
    {
        binary* pBomBuffer = createBOMBytes(bomSize);
        
        //push define to middle 
        appendBuffersMiddle(pFileBuffer, fileSize, bomSize, pDefineBuffer, defineSize);
    }
    else
    {
        //push define
        appendBuffersLeft(pFileBuffer, fileSize, pDefineBuffer, defineSize);
    }

    //push fileBuffer
    //middleAppendBuffers(outputBuffer, outputSize, bomSize + defineSize, fileBuffer, fileSize);

    //insert endif
    appendBuffersRight(pFileBuffer, pFileBuffer->size(), pEndifBuffer, endifSize);

    std::ofstream file(argv[2], std::ios::binary);
    //file << outputBuffer;

    if (file.is_open())
    {
        file.write(&pFileBuffer->at(0), pFileBuffer->size());
        std::cout << "Complete\r\n";
    }
    else
    {
        std::cout << "File is closed\r\n";
        return 1;
    }

    
}

