#ifndef MONGO_CPP_HPP
#define MONGO_CPP_HPP

#include<optional>
#include<vector>
#include<string_view>

#include"mongocxx/client.hpp"
#include"mongocxx/instance.hpp"
#include"mongocxx/stdx.hpp"
#include"mongocxx/uri.hpp"
#include"mongocxx/pool.hpp"

namespace MongoDB {

struct Option {
    std::string username;
    std::string password;
    std::string host;
    int port = 27017;
    int minPoolSize = 0;
    int maxPoolSize = 0;
};

class MongoCollection {
public:
    MongoCollection(mongocxx::collection collection):
        col(std::move(collection)) {};

    auto Insert(const bsoncxx::document::value doc,
                const mongocxx::options::insert& option = {}) {
        return col.insert_one(std::move(doc.view()), std::move(option));
    }

    auto Insert(const std::vector<bsoncxx::document::value>& docs,
                const mongocxx::options::insert& option = {}) {
        return col.insert_many(std::move(docs), std::move(option));
    }

    auto Find(const bsoncxx::document::value& doc,
              const mongocxx::options::find& option = {}) {
        mongocxx::cursor res = col.find(std::move(doc.view()), std::move(option));
        std::vector<std::optional<bsoncxx::document::value>>resList{res.begin(), res.end()};
        return resList;
    }

    auto First(const bsoncxx::document::value& doc,
               const mongocxx::options::find& option = {}) {
        return col.find_one(std::move(doc.view()), std::move(option));
    }

    auto DeleteOne(const bsoncxx::document::value& doc,
                const mongocxx::options::delete_options& option = {}) {
        return col.delete_one(std::move(doc.view()), std::move(option));
    }

    auto DeleteAll(const bsoncxx::document::value& doc,
                   const mongocxx::options::delete_options& option = {}) {
        return col.delete_many(std::move(doc.view()), std::move(option));
    }

    auto UpdateOne(const bsoncxx::document::value& filter,
                   const bsoncxx::document::value& update,
                   const mongocxx::options::update& options = {}) {
        return col.update_one(std::move(filter.view()), std::move(update.view()), std::move(options));
    }

    auto UpdateAll(const bsoncxx::document::value& filter,
                   const bsoncxx::document::value& update,
                   const mongocxx::options::update& options = {}) {
        return col.update_many(std::move(filter.view()), std::move(update.view()), std::move(options));
    }

    mongocxx::collection& Get() {
        return col;
    }

private:
    mongocxx::collection col;
};

class MongoDatabase {
public:
    MongoDatabase(mongocxx::database database):
       db(std::move(database)) {};

    MongoCollection operator[] (const std::string_view colName) {
        return db.collection(colName);
    }

    MongoCollection collection(const std::string_view colName) {
        return db.collection(colName);
    }

    mongocxx::database& Get() {
        return db;
    }

private:
    mongocxx::database db;
};

template<typename T>
class MongoClient {
public:
    MongoClient(T clientPtr):
       cliPtr(std::move(clientPtr)) {};

    MongoDB::MongoDatabase operator[] (const std::string_view dbName) {
        return cliPtr->database(dbName);
    }

    MongoDB::MongoDatabase database(const std::string_view dbName) {
        return cliPtr->database(dbName);
    }

private:
    T cliPtr;
};

class MongoConnetcion {
public:
    MongoConnetcion(MongoDB::Option option):
            opt(option)
    {
        char s[80];
        sprintf(s, "mongodb://%s:%s@%s:%d/",
                option.username.data(),
                option.password.data(),
                option.host.data(),
                option.port);

        if (option.minPoolSize) {
            char mi[20];
            sprintf(mi, "?minPoolSize=%d", option.minPoolSize);
            strcat(s, mi);
        }

        if (option.maxPoolSize) {
            char mx[20];
            sprintf(mx, "&maxPoolSize=%d", option.maxPoolSize);
            strcat(s, mx);
        }

        connUri = std::move(mongocxx::uri{s});
        if (option.minPoolSize or option.maxPoolSize) {
            static mongocxx::pool pool{connUri};
            corp.p = &pool;
        }
        else {
            static mongocxx::client client{connUri};
            corp.c = &client;
        }
    }

    MongoDB::MongoClient<mongocxx::pool::entry> Get() {
        return corp.p->acquire();
    }

    MongoDB::MongoClient<mongocxx::client*> GetSingle() {
        return corp.c;
    }

private:
    MongoDB::Option opt;
    mongocxx::instance inst{};
    mongocxx::uri connUri;
    union corp{
        mongocxx::client* c;
        mongocxx::pool* p;
    } corp;
};

} //namespace coral

#endif //MONGO_CPP_HPP
