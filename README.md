# Leafie's Password Manager

## I. Overview 
**A personal project that should probably not be actually be used as a standard password manager. Just use Bitwarden preferably.** 
However, it does come with decent features. Plus it's open-source & local, so. 
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

## III. Features

 1. Cryptographically secure password handling using `PasswordManager` & `libsodium`. (this took too long to learn) 
 2. Password editing, generation & strength gauging using `zxcvbn-cpp` (why is it named that)
 3. Nice UI™ (i like green)


## IV. Installation






## V. Upcoming

 - **Load multiple vaults at the same time.** Mostly for categorical purposes.
 - **Better UI.** I need it to be prettier :>
 - **Better compatibility with Hyprland/other tiling WMs, as well as better UI organization.** 
 - **Make import & export actually work with the encryption model.** forgot to keep it in mind when i did the refactor oops  
 - And more, possibly. PRs are welcome.
