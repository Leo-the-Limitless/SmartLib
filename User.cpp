#include "User.h"

User::User(int id, const QString &name, const QString &email, bool isAdmin)
    : id(id), name(name), email(email), admin(isAdmin) {}

int User::getId() const { return id; }
QString User::getName() const { return name; }
QString User::getEmail() const { return email; }
bool User::isAdmin() const { return admin; }
