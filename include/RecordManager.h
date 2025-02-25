#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

class RecordManager {
private:
    int currentRecord;
    int bestRecord;

    void getBestRecord()
    {
        std::ifstream file("records/record.txt");
        if (file.is_open())
        {
            file >> bestRecord;
            file.close();
        }
        else
        {
            bestRecord = 0; // если файл не существует или ошибка чтения
        }
    }

    void setBestRecord(int scores)
    {
        currentRecord = scores;
        if (currentRecord > bestRecord)
        {
            bestRecord = currentRecord;
            std::ofstream file("record.txt");
            if (file.is_open())
            {
                file << bestRecord;
                file.close();
            }
            else
            {
                std::cerr << "Error: Unable to open file for writing" << std::endl;
            }
        }
    }

public:
    // Конструктор: получает текущий счёт и обновляет лучший рекорд
    RecordManager(int scores)
    {
        getBestRecord();
        setBestRecord(scores);
    }

    // Возвращает лучший рекорд
    int getRecord() const
    {
        return std::max(currentRecord, bestRecord);
    }
};
