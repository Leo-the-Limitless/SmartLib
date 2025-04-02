#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User(int id, const QString &name, const QString &email, bool isAdmin);
    int getId() const;
    QString getName() const;
    QString getEmail() const;
    bool isAdmin() const;

private:
    int id;
    QString name;
    QString email;
    bool admin;
};

#endif // USER_H
