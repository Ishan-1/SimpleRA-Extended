#include "global.h"
#include "secondary_index.h"

//Syntax :  INSERT INTO table_name ( col1 = val1, col2 = val2, col3 = val3 … ) 


void updateIndexFileWithNewBP(const string& tableName, const string& columnName, int value, const string& newBPFileName)
{
    string indexDir = "../data/indices/";
    int blockNo = 0;
    bool updated = false;
    
    while (!updated)
    {
        string indexFileName = indexDir + tableName + "_" + columnName + "_Indexfile_" + to_string(blockNo);
        ifstream checkFile(indexFileName);
        
        if (!checkFile)
        {
            // No more index files to check
            logger.log("Failed to find index file for value: " + to_string(value));
            return;
        }
        
        // Read file contents into memory
        int distinctCount;
        checkFile >> distinctCount;
        
        vector<pair<int, string>> entries;
        string line;
        getline(checkFile, line); // Skip rest of first line
        
        while (getline(checkFile, line))
        {
            size_t spacePos = line.find(' ');
            if (spacePos != string::npos)
            {
                int entryValue = stoi(line.substr(0, spacePos));
                string bpList = line.substr(spacePos + 1);
                entries.push_back({entryValue, bpList});
            }
        }
        
        checkFile.close();
        
        // Find and update the entry for our value
        for (auto& entry : entries)
        {
            if (entry.first == value)
            {
                // Append the new BP file to the list
                entry.second += "," + newBPFileName;
                
                // Rewrite the index file
                ofstream writeFile(indexFileName);
                writeFile << distinctCount << endl;
                
                for (const auto& e : entries)
                {
                    writeFile << e.first << " " << e.second << endl;
                }
                
                writeFile.close();
                updated = true;
                logger.log("Updated index file: " + indexFileName + " with new BP file: " + newBPFileName);
                break;
            }
        }
        
        blockNo++;
    }
}

bool syntacticParseINSERT()
{
    logger.log("syntacticParseINSERT");
    if (tokenizedQuery.size() < 8 || tokenizedQuery[1] != "INTO" || tokenizedQuery[3] != "(" || tokenizedQuery[tokenizedQuery.size() - 1] != ")")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INSERT;
    parsedQuery.insertRelationName = tokenizedQuery[2];
    parsedQuery.insertColumnNames.clear();
    parsedQuery.insertValues.clear();  
    for (int i = 5; i < tokenizedQuery.size() - 1; i++)
    {
        if (tokenizedQuery[i] == "=")
        {
            if (i + 1 >= tokenizedQuery.size() - 1)
            {
                cout << i<<" SYNTAX ERROR" << endl;
                return false;
            }
            parsedQuery.insertColumnNames.push_back(tokenizedQuery[i - 1]);
            parsedQuery.insertValues.push_back(stoi(tokenizedQuery[i + 1]));
            i+=2;
        }
        else
        {
            cout << i<< " SYNTAX ERROR" << endl;
            return false;
        }
    }
    if (parsedQuery.insertColumnNames.size() != parsedQuery.insertValues.size())
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}




bool semanticParseINSERT()
{
    logger.log("semanticParseINSERT");
    if(!tableCatalogue.isTable(parsedQuery.insertRelationName))
    {
        cout << "SEMANTIC ERROR: Table does not exist" << endl;
        return false;
    }
    for(int i=0;i<parsedQuery.insertColumnNames.size() - 1; i++)
    {
        if(!tableCatalogue.isColumnFromTable(parsedQuery.insertColumnNames[i],parsedQuery.insertRelationName))
        {
            cout << "SEMANTIC ERROR: Column does not exist" << endl;
            return false;
        }
    }
    return true;
}

void executeINSERT()
{
    logger.log("executeINSERT");
    // Get the source table
    Table *sourceTable = tableCatalogue.getTable(parsedQuery.insertRelationName);
    vector<string> columns = sourceTable->getColumnNames();
    vector<int> values;
    // For columns not in the insert query, set the value to 0
    for (int i = 0; i < columns.size(); i++)
    {
        bool found = false;
        for (int j = 0; j < parsedQuery.insertColumnNames.size(); j++)
        {
            if (columns[i] == parsedQuery.insertColumnNames[j])
            {
                values.push_back(parsedQuery.insertValues[j]);
                found = true;
                break;
            }
        }
        if (!found)
        {
            values.push_back(0);
        }
    }
    // Insert the row into the table and get its location (page number and row number)
    pair<int, int> rowLocation = sourceTable->insertRow(values);
    int pageNo = rowLocation.first;
    int rowNo = rowLocation.second;
    if(pageNo == -1 || rowNo == -1)
    {
        cout << "ERROR: Unable to insert row" << endl;
        return;
    }
    // Check and update index for the indexed column
    if (sourceTable->indexed)
    {
        // Get the indexed column and its value
        string indexedColumn = sourceTable->indexedColumn;
        int columnIndex = sourceTable->getColumnIndex(indexedColumn);
        
        if (columnIndex != -1)
        {
            int valueToIndex = values[columnIndex];
            
            // Create the index object properly
            SecondaryIndex index(parsedQuery.insertRelationName, indexedColumn);
            
            // Find the relevant BP files for this value
            vector<pair<int, int>> existingLocations = index.search(valueToIndex);
            
            if (!existingLocations.empty())
            {
                // The value exists, we need to add this row to the appropriate BP file
                
                // Get the path to the BP file directory
                string indexDir = "../data/indices/";
                string bpFilePrefix = "BP_" + parsedQuery.insertRelationName + "_" + indexedColumn + "_" + to_string(valueToIndex) + "_";
                
                // Find the last BP file number
                int lastBPFileNum = 0;
                while (true)
                {
                    string testPath = indexDir + bpFilePrefix + to_string(lastBPFileNum);
                    ifstream testFile(testPath);
                    if (!testFile)
                        break;  // Found the end
                    
                    lastBPFileNum++;
                    testFile.close();
                }
                
                lastBPFileNum--; // Go back to the last valid file
                
                if (lastBPFileNum >= 0)
                {
                    // Check the last BP file to see if it has space
                    string lastBPPath = indexDir + bpFilePrefix + to_string(lastBPFileNum);
                    ifstream checkBP(lastBPPath);
                    
                    if (checkBP)
                    {
                        int recordCount;
                        checkBP >> recordCount;
                        checkBP.close();
                        
                        const int MAX_RECORDS_PER_BP_BLOCK = 1000;
                        
                        if (recordCount < MAX_RECORDS_PER_BP_BLOCK)
                        {
                            // Update existing BP file
                            fstream updateBP(lastBPPath, ios::in | ios::out);
                            updateBP.seekp(0);
                            updateBP << (recordCount + 1);  // Update record count
                            
                            // Go to end to append the new record location
                            updateBP.seekp(0, ios::end);
                            updateBP << pageNo << " " << rowNo << endl;
                            updateBP.close();
                        }
                        else
                        {
                            // Need to create a new BP file
                            int newBPFileNum = lastBPFileNum + 1;
                            string newBPPath = indexDir + bpFilePrefix + to_string(newBPFileNum);
                            
                            ofstream newBP(newBPPath);
                            newBP << 1 << endl;  // One record
                            newBP << pageNo << " " << rowNo << endl;
                            newBP.close();
                            
                            // Update the index file to include the new BP file
                            // Need to find and update the index file that contains this value
                            updateIndexFileWithNewBP(parsedQuery.insertRelationName, indexedColumn, valueToIndex, bpFilePrefix + to_string(newBPFileNum));
                        }
                    }
                }
            }
            else
            {
                index.updateIndex(valueToIndex);   
                // Now add the record location to the BP file
                string bpFileName = "BP_" + parsedQuery.insertRelationName + "_" + indexedColumn + "_" + to_string(valueToIndex) + "_0";
                string bpFilePath = "../data/indices/" + bpFileName;
                
                ofstream bpFile(bpFilePath);
                bpFile << 1 << endl;  // One record
                bpFile << pageNo << " " << rowNo << endl;
                bpFile.close();
            }
            
        }
        else
        {
            cout<<"Error: Cannot find indexed column in table schema"<<endl;
        }
    }
    // Print the inserted row
    cout << "Inserted row: ";
    for (int i = 0; i < values.size(); i++)
    {
        cout << values[i];
        if (i < values.size() - 1)
        {
            cout << ", ";
        }
    }
    cout << endl;
}