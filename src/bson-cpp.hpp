#ifndef BSON_CPP_HPP
#define BSON_CPP_HPP

#include<tuple>
#include<string_view>

#include"bsoncxx/builder/basic/document.hpp"
#include"bsoncxx/json.hpp"

namespace Bson {

class Result {
public:
    Result(std::optional<bsoncxx::document::value> result):
       res(std::move(result)) {};

    Result(const bsoncxx::document::view result):
       res(std::move(std::optional<bsoncxx::document::value>(result))) {};

    bool isNull() {
        return !res.has_value();
    }

    std::string_view Id(const std::string_view id = "_id") {
        return res->view()[id].get_oid().value.to_string();
    }

    int32_t Int32(const std::string_view objName) {
        return res->view()[objName].get_int32().value;
    }

    int64_t Int64(const std::string_view objName) {
        return res->view()[objName].get_int64().value;
    }

    double Double(const std::string_view objName) {
        return res->view()[objName].get_double();
    }

    std::string_view String(const std::string_view objName) {
        return res->view()[objName].get_string().value;
    }

    bool Bool(const std::string_view objName) {
        return res->view()[objName].get_bool();
    }

    Result Document(const std::string_view objName) {
        return res->view()[objName].get_document().view();
    }

    bsoncxx::document::element operator[] (const std::string_view objName) {
        return res->view()[objName];
    }

private:
    std::optional<bsoncxx::document::value> res;
};

class ResultList {
public:
    ResultList(std::vector<std::optional<bsoncxx::document::value>> result):
       resList(result.begin(), result.end()) {};

    Bson::Result& operator[] (int32_t index) {
        return index < 0 ? resList[resList.size() - abs(index)] : resList[index];
//        return resList[index];
    }

private:
    std::vector<Bson::Result>resList;
};

template<typename... Args>
bsoncxx::document::value D(Args&&...args) {
    bsoncxx::builder::basic::document doc;
    doc.append(std::forward<Args>(args)...);
    return doc.extract();
}

template<typename... Args>
bsoncxx::array::value A(Args&&...args) {
    bsoncxx::builder::basic::array arr;
    arr.append(std::forward<Args>(args)...);
    return arr.extract();
}

template<typename T, typename U>
inline std::tuple<T&&, U&&>P(T&&t, U&& u) {
    return std::tuple<T&&, U&&>(std::forward<T>(t), std::forward<U>(u));
}

} //namespace MongoDB

#endif //BSON_CPP_HPP
