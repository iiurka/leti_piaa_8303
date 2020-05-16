import os

DIRECTORY = "Tests\\Tests Joker"

tests = os.listdir(DIRECTORY)

for i in range(len(tests)):
    testPath = DIRECTORY + "\\" + tests[i]
    print()
    print("Test ", i+1, ":   ", testPath, sep="")
    print(*open(testPath, "r").readlines(), sep="")
    os.system("Joker.exe < \"" + testPath + "\"")
