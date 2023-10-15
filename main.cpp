#include<iostream>
#include<chrono>
#include"src/mongo-cpp.hpp"
#include"src/bson-cpp.hpp"

int main() {
    try {
        MongoDB::MongoConnetcion conn(MongoDB::Option{
                .username = "your_username",
                .password = "your_password",
                .host = "your_hostname",
                .minPoolSize = 3,
                .maxPoolSize = 3,
        });

        auto t = conn.Get();
        MongoDB::MongoCollection col = t["admin"]["test"];

        col.Insert(Bson::D(
            Bson::P("name", "zhangsan"),
            Bson::P("age", 13),
            Bson::P("attribute", Bson::D(
                Bson::P("level", 1),
                Bson::P("uname", "luoxiang")
            ))
        ));

        Bson::ResultList many = col.Find(Bson::D(
            Bson::P("age", Bson::D(
                    Bson::P("$gte", 13)
            ))
        ));

        Bson::Result one = col.First(Bson::D(
                Bson::P("age", Bson::D(
                        Bson::P("$gte", 13)
                ))
        ));

        std::cout << one.String("name") << std::endl;
        std::cout << many[-1].Document("attribute").String("uname") << std::endl;

        auto update_res = col.UpdateOne(Bson::D(
            Bson::P("age", Bson::D(
                Bson::P("$gt", 30),
                Bson::P("$lt", 60)
            ))),
            Bson::D(
                Bson::P("$set", Bson::D(
                    Bson::P("name", "lisi")
                ))
            )
        );

        if (update_res->modified_count() == 1) {
            std::cout << "Update successfully!" << std::endl;
        }

        auto delete_result = col.DeleteOne(
            Bson::D(
                Bson::P("age", Bson::D(
                    Bson::P("$gt", 30)
                ))
            )
        );

        if (delete_result->deleted_count() == 1) {
            std::cout << "Delete successfully!" << std::endl;
        }
    }
    catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }

    return 0;
}
