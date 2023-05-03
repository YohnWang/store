

#include<string>
#include<exception>
#include<sqlite3.h>

template<typename T>
class storable
{

};


class sqlite
{
    sqlite3 *db;

public:

    class sqlite_exception:public std::exception
    {
        const char *errmsg;
    public:
        virtual const char* what() const noexcept override
        {
            return errmsg?errmsg:"sqlite something error";
        };
        sqlite_exception(const char *s=nullptr):errmsg{s}{}
    };

    sqlite(std::string s)
    {
        auto ret=sqlite3_open(s.c_str(), &db);
        if(ret)
        {
            throw sqlite_exception("open db failed");
        }
    }
    ~sqlite()
    {
        sqlite3_close(db);
    }
};

class sqlite_table
{
    const sqlite &db;
    std::string table_name;
public:
    sqlite_table(const sqlite &db, std::string s):db{db},table_name{std::move(s)}{}
    std::string operator[](std::string key)
    {
        
    }
};

#include<gmllib.h>
int main()
{
    scope_time_print()
    {
        sqlite("test.db");
    }

}