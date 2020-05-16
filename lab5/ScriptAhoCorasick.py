import os

DIRECTORY = "Tests\\Tests Aho-Corasick"

tests = os.listdir(DIRECTORY)

for i in range(len(tests)):
    testPath = DIRECTORY + "\\" + tests[i]
    print()
    print("Test ", i+1, ":   ", testPath, sep="")
    print(*open(testPath, "r").readlines(), sep="")
    os.system("AhoCorasick.exe < \"" + testPath + "\"")
