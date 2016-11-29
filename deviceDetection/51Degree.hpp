/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   51Degree.hpp
 * Author: Hung
 *
 * Created on November 17, 2016, 11:44 AM
 */




class Match {
public:

    virtual ~Match();

    std::vector<std::string> getValues(const char *propertyName);
    std::vector<std::string> getValues(std::string &propertyName);
    std::vector<std::string> getValues(int propertyIndex);

    std::string getValue(const char *propertyName);
    std::string getValue(std::string &propertyName);
    std::string getValue(int propertyIndex);

    std::string getDeviceId();
    int getRank();
    int getDifference();
    int getMethod();
    std::string getUserAgent();
};

class Provider {
public:

    Provider(const std::string &fileName);
    Provider(const std::string &fileName, const std::string &propertyString);
    Provider(const std::string &fileName, const std::string &propertyString, int cacheSize, int poolSize);
    Provider(const std::string &fileName, std::vector<std::string> &propertiesArray);
    Provider(const std::string &fileName, std::vector<std::string> &propertiesArray, int cacheSize, int poolSize);
    Provider(const std::string &fileName, int cacheSize, int poolSize);
    virtual ~Provider();

    std::vector<std::string> getHttpHeaders();
    std::vector<std::string> getAvailableProperties();
    std::string getDataSetName();
    std::string getDataSetFormat();
    std::string getDataSetPublishedDate();
    std::string getDataSetNextUpdateDate();
    int getDataSetSignatureCount();
    int getDataSetDeviceCombinations();

    Match* getMatch(const std::string &userAgent);
    Match* getMatch(const std::map<std::string, std::string> &headers);

    std::string getMatchJson(const std::string &userAgent);
    std::string getMatchJson(const std::map<std::string, std::string> &headers);

    Match* getMatchForDeviceId(const std::string &deviceId);
    Profiles* findProfiles(const std::string propertyName, const std::string valueName);
    Profiles* findProfiles(const std::string propertyName, const std::string valueName, Profiles *profiles);

    void reloadFromFile();
    void reloadFromMemory(const std::string &source, int length);

    int getCacheHits();
    int getCacheMisses();
    int getCacheMaxIterations();

    Provider(const std::string &fileName, const std::string &propertyString, int cacheSize, int poolSize, bool validate);

};
