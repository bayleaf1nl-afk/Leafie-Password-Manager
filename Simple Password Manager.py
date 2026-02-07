import cryptography
import json

passwordExample = {
    "gmail": "bayleaf@gmail.john",
    "website": "https://www.bayleaf.com",
    "password": "IAMAVERYSECUREPASSWORDRAAAAAAAAAAAAAGH"
}

with open("password.json", "w") as PassFile:
    data = json.dump(passwordExample, PassFile)

print(data)
print(type(data))
