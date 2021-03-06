#include "WebConf.h"
#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>
#include <fstream>
#include "Paths.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"


using namespace rapidjson;
using namespace tet;

WebConf::WebConf()
{
    serverConfig = Paths::serverConfigPath().c_str();
    configuration = std::vector<ConfEntry>();
    serviceEnabled = false;
}
void WebConf::initiate()
{
    if (!readConf()) makeConf();
}

WebConf::~WebConf()
{
}

bool WebConf::readConf()
{
    FILE* conf =fopen(serverConfig.c_str(), "rb");
    if (!conf) {
        LOG("No config found at %s\n", serverConfig.c_str());
        return false;
    }
    Document doc;
    char readBuffer[10000];
    FileReadStream in(conf, readBuffer, sizeof(readBuffer));
    doc.ParseStream(in);
    fclose(conf);
    if (checkValidity(&doc)) setConf(&doc);
    else
    {
        makeConf();
        return false;
    }
    return true;
}

bool WebConf::checkValidity(Document *doc)
{
    bool valid = true;
    if (!doc->IsObject()) valid = false;
    Value::MemberIterator enabled = doc->FindMember("enabled");
    if (enabled == doc->MemberEnd()) valid = false;
    if (!enabled->value.IsBool()) valid = false;
    return valid;
}

void WebConf::setConf(Document *doc)
{
    serviceEnabled= doc->FindMember("enabled")->value.GetBool();
    for (const char* i : keys)
    {
        if (doc->HasMember(i))
        {
            auto thisValue = doc->FindMember(i);
            if (thisValue->value.IsString())
            {
               configuration.push_back({i, thisValue->value.GetString()});
            }
        }
    }
}

void WebConf::makeConf()
{
    serviceEnabled = false;
    std::stringstream defaultString;
    defaultString << "\{\n\"enabled\": false";
    for (auto i : keys)
    {
        defaultString << ",\n\"" << i << "\": \"\"";
    }
    defaultString << "}";
    remove (serverConfig.c_str());
    std::ofstream out(serverConfig.c_str());
    out << defaultString.str();
    out.close();
}
