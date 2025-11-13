#include "JsonFileManager.h"

void JsonFileManager::FindTargetDirectory()
{
    try {
        // We use a recursive iterator to go through all subdirectories as well. 
        for (const std::filesystem::directory_entry& Entry : std::filesystem::recursive_directory_iterator(SearchPath)) {
            // Check if the current entry is a directory 
            if (std::filesystem::is_directory(Entry.status())) {
                // Get the directory's name as a string and check for the substring 
                if (Entry.path().filename().string().find(SearchString) != std::string::npos) {
                    std::cout << "Found matching directory: " << std::filesystem::absolute(Entry.path()) << std::endl;
                    DestinationPath = std::filesystem::absolute(Entry.path()).string();
                }
            }
        }
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void JsonFileManager::WriteTheDialogueJson()
{
    std::string FileName = JsonFileName + ".json";
    DestinationPath /= FileName;

    std::ofstream DialogueJson(DestinationPath);

    DialogueJson << GenerateTheFinalJsonString();

    DialogueJson.close();
}

std::string JsonFileManager::GenerateTheFinalJsonString()
{
    std::ostringstream FinalJsonString;
    FinalJsonString
        << "{\n"
        << "\t\"NPCId\": \"" << DialogueTree::NPCId << "\",\n"
        << "\t\"Nodes\": \n"
        << "\t[\n";

    auto StringifyContainer = [&FinalJsonString]<typename T>(const std::vector<T>&V) 
    {
        size_t Size{ V.size() };
        for (size_t i = 0; i < Size; i++)
        {
            FinalJsonString << V[i].StringifyForJson();
            if (i != Size - size_t(1))
            {
                FinalJsonString << ",\n";
            }
            else
            {
                FinalJsonString << "\n";
            }
        }
    };

    StringifyContainer(DialogueTree::DialogueNodeContainer);

    FinalJsonString
        << "\t],\n"
        << "\t\"Edges\":\n"
        << "\t[\n";

    StringifyContainer(DialogueTree::DialogueEdgeContainer);

    FinalJsonString
        << "\t]\n"
        << "}";

    return FinalJsonString.str();
}

std::string JsonFileManager::ParserForJson(const std::string& inStringToParse)
{
    std::string ParsedString;

    for (const char& VectorLetter : inStringToParse)
    {
        switch (VectorLetter)
        {
        case '"':
        case '\\':
            ParsedString += '\\';
            ParsedString += VectorLetter;
            break;
        default:
            ParsedString += VectorLetter;
            break;
        }
    }

    return ParsedString;
}
