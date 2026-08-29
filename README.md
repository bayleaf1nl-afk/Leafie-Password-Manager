# Leafie's Password Manager

## I. Overview 
**A personal project that should probably not be actually be used as a standard password manager. Just use Bitwarden preferably.** 
However, it does come with decent features. 
Made in C++23 & C++14, uses Qt6.

## II. Architecture

Codebase is organized into 5 directories that handle low-level OS interactions and the UI.
  
  | Directory   | Responsibility    |
  |--------------- | --------------- |
  | core/   | Application logic, utilities, cryptography   |
  | vault/   | Passwords & the PasswordManager   |
  | ui/   | self-explanatory   |
  | platform/   | clipboard & per-OS window handling   |
  | third_party/   | zxcvbn-cpp for password strength    |
