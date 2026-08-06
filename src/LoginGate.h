#pragma once
#include <QString>

class LoginGate {
public:
  static bool authenticate(QString &outMasterPassword);
};