### mongo-cpp
对Mongodb的简单封装，基于mongo-cxx-driver，使用简单~

### 构建项目
* 构建项目需要用到`CMake`，所以先下载CMake
```shell
sudo apt install CMake
```

* 然后构建项目
```shell
git clone https://github.com/na2co3hk/mongo-cpp.git
mkdir build
cd build
cmake ..
make 
```

### 快速开始
```cpp
#include<iostream>
#include"src/mongo-cpp.hpp"
#include"src/bson-cpp.hpp"

int main() {
    try {
        //连接Mongodb, 端口默认为27017
        //填入连接池数量则代表使用连接池
        MongoDB::MongoConnetcion conn(MongoDB::Option{
                .username = "username",
                .password = "password",
                .host = "hostname",
                .minPoolSize = 3,
                .maxPoolSize = 3,
        });

        //获取一个连接
        auto t = conn.Get();
        //加载集合
        MongoDB::MongoCollection col = t["admin"]["test"];

        //接下来是CRUD
        //除了查询，其他操作都是返回mongo-cxx-driver原生api的结果
  
        //插入
        col.Insert(Bson::D(
            Bson::P("name", "zhangsan"),
            Bson::P("age", 13),
            Bson::P("attribute", Bson::D(
                Bson::P("level", 1),
                Bson::P("uname", "luoxiang")
            ))
        ));

        //查询所有结果（这里建议不要忽略返回类型，因为用到了隐式转换）
        Bson::ResultList many = col.Find(Bson::D(
            Bson::P("age", Bson::D(
                    Bson::P("$gte", 13)
            ))
        ));

        //查询一条结果
        Bson::Result one = col.First(Bson::D(
                Bson::P("age", Bson::D(
                        Bson::P("$gte", 13)
                ))
        ));

        std::cout << one.String("name") << std::endl;
        std::cout << many[-1].Document("attribute").String("uname") << std::endl;

        //更新
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

        //删除
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
```

### 未来计划
其他功能还在开发中~
